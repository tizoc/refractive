module Lens = Refractive__Lens;

type t('state, 'value) = {
  lens: Lens.t('state, 'value),
  path: string,
  touchedPaths: list(string),
  listenPaths: list(string),
};

let rec unfoldPath = path => {
  switch (String.rindex(path, '/')) {
  | exception Not_found => [path]
  | idx =>
    let subpath = String.sub(path, 0, idx);
    [path, ...unfoldPath(subpath)];
  };
};

let make = (~lens, ~path) => {
  let unfoldedPath = unfoldPath(path);
  let touchedPaths = [path ++ "/*", ...unfoldedPath];
  let parentPaths = List.tl(unfoldedPath);
  let listenPaths = [path, ...List.map(p => p ++ "/*", parentPaths)];
  {lens, path, touchedPaths, listenPaths};
};

let path = s => s.path;

let touchedPaths = s => s.touchedPaths;

let listenPaths = s => s.listenPaths;

let view = (state, selector) => Lens.view(state, selector.lens);

let modify = (f, state, selector) => Lens.modify(f, state, selector.lens);

let compose = (outerSelector, innerSelector) => {
  let lens = Lens.compose(outerSelector.lens, innerSelector.lens);
  let path = outerSelector.path ++ "/" ++ innerSelector.path;
  make(~lens, ~path);
};

// Selector wrappers for default lenses

let arrayIndex = i =>
  make(~lens=Lens.arrayIndex(i), ~path=string_of_int(i));

let arrayLength = filler =>
  make(~lens=Lens.arrayLength(filler), ~path="$length");