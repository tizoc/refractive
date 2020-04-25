module Lens = Refractive__Lens;

let (@.) = (g, f, x) => g(f(x));

let sep = ".";
let join = (a, b) => String.concat(sep, [a, b]);

type straightPath = array(string); // path fragments are stored in reverse

// Selector paths are either straight or forked
// Forked paths have:
// - a base
// - that has many forks
// - which are combined back by a joiner
// - followed by an optional inner path
type selectorPath =
  | Straight(straightPath)
  | Forked(straightPath, list(selectorPath), string, option(selectorPath));

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
  | Straight(path) => path |> Array.to_list |> unfoldFragments
  | Forked(basePath, forks, _joiner, _innerPath) =>
    // TODO: joiner and innerPath?
    let unfoldedBasePath = basePath |> Array.to_list |> unfoldFragments;
    let unfoldedForkPaths = List.map(unfoldPath, forks) |> List.concat;
    let stringBasePath = stringOfRawPath(Array.to_list(basePath));
    let unfoldedPaths =
      List.map(joinPath(stringBasePath), unfoldedForkPaths);
    List.sort_uniq(lengthCmp, unfoldedPaths @ unfoldedBasePath);
  };

let rec validatePath = path =>
  switch (path) {
  | Straight(path) =>
    if (Array.length(path) < 1) {
      invalid_arg("Selector path array must contain at least one element");
    }
  | Forked(_basePath, forks, joiner, innerPath) =>
    List.iter(validatePath, forks);
    Belt.Option.forEach(innerPath, validatePath);
    if (String.length(joiner) < 1) {
      invalid_arg("Selector joiner cannot be empty");
    };
  };

let rec stringOfPath = path => {
  switch (path) {
  | Straight(path) => stringOfRawPath(Array.to_list(path))
  | Forked(basePath, forks, joiner, innerPath) =>
    let forked = String.concat(", ", List.map(stringOfPath, forks));
    let joined = joiner ++ "(" ++ forked ++ ")";
    let innerPathString =
      switch (innerPath) {
      | None => joined
      | Some(innerPath) => joined ++ sep ++ stringOfPath(innerPath)
      };
    switch (Array.to_list(basePath)) {
    | [] => innerPathString
    | rawPath => stringOfRawPath(rawPath) ++ sep ++ innerPathString
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
  _make(~lens, ~path=Straight(path));
};

let view = s => Lens.view(s.lens);
let modify = s => Lens.modify(s.lens);
let set = s => Lens.set(s.lens);

let rec composePath = (outerPath, innerPath) =>
  switch (outerPath, innerPath) {
  | (Straight(outerPath), Straight(innerPath)) =>
    Straight(Array.append(innerPath, outerPath))
  | (Straight(outerPath), Forked(forkedBasePath, forks, joiner, innerPath)) =>
    Forked(Array.append(forkedBasePath, outerPath), forks, joiner, innerPath)
  | (Forked(outerBasePath, outerForkedPaths, joiner, None), moreInnerPath) =>
    Forked(outerBasePath, outerForkedPaths, joiner, Some(moreInnerPath))
  | (
      Forked(outerBasePath, outerForkedPaths, joiner, Some(innerPath)),
      moreInnerPath,
    ) =>
    Forked(
      outerBasePath,
      outerForkedPaths,
      joiner,
      Some(composePath(innerPath, moreInnerPath)),
    )
  };

let compose = (outerSelector, innerSelector) => {
  let lens = Lens.compose(outerSelector.lens, innerSelector.lens);
  let path = composePath(outerSelector.path, innerSelector.path);
  _make(~lens, ~path);
};

let const = value => make(~lens=Lens.const(value), ~path=[|"const()"|]);

let pair = (leftSelector, rightSelector) => {
  let lens = Lens.pair(leftSelector.lens, rightSelector.lens);
  let path =
    Forked([||], [leftSelector.path, rightSelector.path], "pair", None);
  _make(~lens, ~path);
};

let map = (~name, f, selector) => {
  let lens = Lens.map(f, selector.lens);
  let path = composePath(selector.path, Straight([|name|]));
  _make(~lens, ~path);
};

let map2 = (~name, f, selector1, selector2) => {
  let lens = Lens.map2(f, selector1.lens, selector2.lens);
  let path = Forked([||], [selector1.path, selector2.path], name, None);
  _make(~lens, ~path);
};

let map3 = (~name, f, selector1, selector2, selector3) => {
  let lens = Lens.map3(f, selector1.lens, selector2.lens, selector3.lens);
  let path =
    Forked(
      [||],
      [selector1.path, selector2.path, selector3.path],
      name,
      None,
    );
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
  let path =
    Forked(
      [||],
      [selector1.path, selector2.path, selector3.path, selector4.path],
      name,
      None,
    );
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