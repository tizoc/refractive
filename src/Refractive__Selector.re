module Lens = Refractive__Lens;

let sep = ".";
let join = (a, b) => String.concat(sep, [a, b]);

type t('state, 'value) = {
  lens: Lens.t('state, 'value),
  path: array(string), // path fragments are stored in reverse
  pathString: string,
  affectedPaths: array(string),
  observedPaths: array(string),
};

let path = s => s.path;
let pathId = s => s.pathString;
let affectedPaths = s => s.affectedPaths;
let observedPaths = s => s.observedPaths;

let string_of_path = path => path |> List.rev |> String.concat(sep);

let rec unfoldFragments = fragments =>
  switch (fragments) {
  | [] => []
  | [fragment] => [fragment]
  | [_, ...rest] as fragments => [
      string_of_path(fragments),
      ...unfoldFragments(rest),
    ]
  };

let unfoldPath = path => unfoldFragments(Array.to_list(path));

let make = (~lens, ~path) => {
  if (Array.length(path) < 1) {
    invalid_arg("Selector path array must contain at least one element");
  };
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

let view = (state, selector) => Lens.view(state, selector.lens);

let modify = (f, state, selector) => Lens.modify(f, state, selector.lens);

let compose = (outerSelector, innerSelector) => {
  let lens = Lens.compose(outerSelector.lens, innerSelector.lens);
  let path = Array.append(innerSelector.path, outerSelector.path);
  make(~lens, ~path);
};

// Selector wrappers for default lenses

let arrayIndex = i =>
  make(
    ~lens=Lens.arrayIndex(i),
    ~path=[|"get(" ++ string_of_int(i) ++ ")"|],
  );

let arrayLength = filler =>
  make(~lens=Lens.arrayLength(filler), ~path=[|"length"|]);