module Lens = Refractive__Lens;

type t('state, 'value) = {
  lens: Lens.t('state, 'value),
  paths: list(string),
};

let rec unfoldPath = path => {
  switch (String.rindex(path, '/')) {
  | exception Not_found => [path]
  | idx => [path, ...unfoldPath(String.sub(path, 0, idx))]
  };
};

let make = (~lens, ~path) => {lens, paths: unfoldPath(path)};

let path = ({paths}) => List.hd(paths);

let touchedPaths = ({paths}) => paths;

let view = (state, selector) => Lens.view(state, selector.lens);

let modify = (f, state, selector) => Lens.modify(f, state, selector.lens);

let compose = (outerSelector, innerSelector) => {
  let lens = Lens.compose(outerSelector.lens, innerSelector.lens);
  let paths =
    unfoldPath(
      List.hd(outerSelector.paths) ++ "/" ++ List.hd(innerSelector.paths),
    );
  {lens, paths};
};

// Selector wrappers for default lenses

let arrayIndex = i =>
  make(~lens=Lens.arrayIndex(i), ~path=string_of_int(i));

let arrayLength = filler =>
  make(~lens=Lens.arrayLength(filler), ~path="$length");