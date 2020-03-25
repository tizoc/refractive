module Lens = Refractive__Lens;

let (@.) = (g, f, x) => g(f(x));

let sep = ".";
let join = (a, b) => String.concat(sep, [a, b]);

type singlePath = array(string); // path fragments are stored in reverse

type selectorPath =
  | Single(singlePath)
  | Forked(singlePath, list(selectorPath));

type t('state, 'value) = {
  lens: Lens.t('state, 'value),
  path: selectorPath,
  pathString: string,
  affectedPaths: array(string),
  observedPaths: array(string),
};

let path = s => s.path;
let pathId = s => s.pathString;
let affectedPaths = s => s.affectedPaths;
let observedPaths = s => s.observedPaths;

let stringOfRawPath = String.concat(sep) @. List.rev;

let pathsEqual = (p1, p2) => Belt.Array.eq(p1, p2, String.equal);

let joinPath = (parent, child) =>
  switch (parent, child) {
  | (p, "") => p
  | ("", c) => c
  | (p, c) => join(p, c)
  };

// Given a path of array(string) fragments,
// create all subpaths from the full path to the root.
let rec unfoldFragments = fragments =>
  switch (fragments) {
  | [] => []
  | [fragment] => [fragment]
  | [_, ...rest] as fragments => [
      stringOfRawPath(fragments),
      ...unfoldFragments(rest),
    ]
  };

let lengthCmp = (a, b) =>
  switch (compare(String.length(a), String.length(b))) {
  | 0 => String.compare(a, b)
  | n => - n
  };

let rec unfoldPath = path =>
  switch (path) {
  | Single(path) => path |> Array.to_list |> unfoldFragments
  | Forked(basePath, forks) =>
    let unfoldedBasePath = basePath |> Array.to_list |> unfoldFragments;
    let unfoldedForkPaths = List.map(unfoldPath, forks) |> List.concat;
    let stringBasePath = stringOfRawPath(Array.to_list(basePath));
    let unfoldedPaths =
      List.map(joinPath(stringBasePath), unfoldedForkPaths);
    List.sort(lengthCmp, unfoldedPaths @ unfoldedBasePath);
  };

let rec validatePath = path =>
  switch (path) {
  | Single(path) when Array.length(path) < 1 =>
    invalid_arg("Selector path array must contain at least one element")
  | Forked(_basePath, forks) => List.iter(validatePath, forks)
  | _ => ()
  };

let rec stringOfPath = path => {
  switch (path) {
  | Single(path) => stringOfRawPath(Array.to_list(path))
  | Forked(basePath, forks) =>
    let forked =
      "{" ++ String.concat(", ", List.map(stringOfPath, forks)) ++ "}";
    if (Array.length(basePath) > 0) {
      stringOfRawPath(Array.to_list(basePath)) ++ sep ++ forked;
    } else {
      forked;
    };
  };
};

let _make = (~lens, ~path) => {
  validatePath(path);
  let pathAndParents = unfoldPath(path);
  let pathString = List.hd(pathAndParents);
  let affectedPaths =
    Array.of_list([join(pathString, "*"), ...pathAndParents]);
  let observedPaths =
    Array.of_list([
      pathString,
      ...List.map(p => join(p, "*"), List.tl(pathAndParents)),
    ]);
  {lens, path, pathString, affectedPaths, observedPaths};
};

let make = (~lens, ~path) => {
  _make(~lens, ~path=Single(path));
};

let view = s => Lens.view(s.lens);
let modify = s => Lens.modify(s.lens);
let set = s => Lens.set(s.lens);

let rec composePath = (outerPath, innerPath) =>
  switch (outerPath, innerPath) {
  | (Single(outerPath), Single(innerPath)) =>
    Single(Array.append(innerPath, outerPath))
  | (Single(outerPath), Forked(forkedBasePath, forks)) =>
    Forked(Array.append(forkedBasePath, outerPath), forks)
  | (
      Forked(outerBasePath, outerForkedPaths),
      Single(_innerPath) as innerPath,
    ) =>
    Forked(
      outerBasePath,
      List.map(composePath(_, innerPath), outerForkedPaths),
    )
  | (
      Forked(outerBasePath, outerForkedPaths),
      Forked(innerBasePath, innerForkedPaths),
    ) =>
    if (pathsEqual(outerBasePath, innerBasePath)) {
      Forked(
        outerBasePath,
        outerForkedPaths @ innerForkedPaths,
      );
    } else {
      invalid_arg("Cannot compose forked paths with non-matching base path");
    }
  };

let compose = (outerSelector, innerSelector) => {
  let lens = Lens.compose(outerSelector.lens, innerSelector.lens);
  let path = composePath(outerSelector.path, innerSelector.path);
  _make(~lens, ~path);
};

let const = value => make(~lens=Lens.const(value), ~path=[|"const()"|]);

let pair = (leftSelector, rightSelector) => {
  let lens = Lens.pair(leftSelector.lens, rightSelector.lens);
  let path = Forked([||], [leftSelector.path, rightSelector.path]);
  _make(~lens, ~path);
};

let map = (~name, f, selector) => {
  let lens = Lens.map(f, selector.lens);
  let path = composePath(selector.path, Single([|name|]));
  _make(~lens, ~path);
};

let map2 = (~name, f, selector1, selector2) => {
  let lens = Lens.map2(f, selector1.lens, selector2.lens);
  let basePath = Forked([||], [selector1.path, selector2.path]);
  let path = composePath(basePath, Single([|name|]));
  _make(~lens, ~path);
};

let map3 = (~name, f, selector1, selector2, selector3) => {
  let lens = Lens.map3(f, selector1.lens, selector2.lens, selector3.lens);
  let basePath =
    Forked([||], [selector1.path, selector2.path, selector3.path]);
  let path = composePath(basePath, Single([|name|]));
  _make(~lens, ~path);
};

let map4 = (~name, f, selector1, selector2, selector3, selector4) => {
  let lens =
    Lens.map4(
      f,
      selector1.lens,
      selector2.lens,
      selector3.lens,
      selector4.lens,
    );
  let basePath =
    Forked(
      [||],
      [selector1.path, selector2.path, selector3.path, selector4.path],
    );
  let path = composePath(basePath, Single([|name|]));
  _make(~lens, ~path);
};
// Selector wrappers for default lenses

let arrayIndex = i =>
  make(
    ~lens=Lens.arrayIndex(i),
    ~path=[|"get(" ++ string_of_int(i) ++ ")"|],
  );

let arrayLength = filler =>
  make(~lens=Lens.arrayLength(filler), ~path=[|"length"|]);