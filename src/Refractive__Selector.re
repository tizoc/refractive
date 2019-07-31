module Lense = Refractive__Lense;

type t('state, 'value) = {
  lense: Lense.t('state, 'value),
  paths: list(string),
};

let rec unfoldPath = path => {
  switch (String.rindex(path, '/')) {
  | exception Not_found => [path]
  | idx => [path, ...unfoldPath(String.sub(path, 0, idx))]
  };
};

let make = (~lense, ~path) => {lense, paths: unfoldPath(path)};

let path = ({paths}) => List.hd(paths);

let touchedPaths = ({paths}) => paths;

let view = (state, selector) => Lense.view(state, selector.lense);

let modify = (f, state, selector) => Lense.modify(f, state, selector.lense);

let compose = (outerSelector, innerSelector) => {
  let lense = Lense.compose(outerSelector.lense, innerSelector.lense);
  let paths =
    unfoldPath(
      List.hd(outerSelector.paths) ++ "/" ++ List.hd(innerSelector.paths),
    );
  {lense, paths};
};

// Selector wrappers for default lenses

let arrayIndex = i =>
  make(~lense=Lense.arrayIndex(i), ~path=string_of_int(i));

let arrayLength = filler =>
  make(~lense=Lense.arrayLength(filler), ~path="$length");