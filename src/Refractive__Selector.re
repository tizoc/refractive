module Lense = Refractive__Lense;

type t('state, 'value) = {
  lense: Lense.t('state, 'value),
  path: string,
};

let read = (state, selector) => Lense.read(state, selector.lense);

let change = (f, state, selector) => Lense.change(f, state, selector.lense);

let compose = (outerSelector, innerSelector) => {
  let lense = Lense.compose(outerSelector.lense, innerSelector.lense);
  let path = outerSelector.path ++ "/" ++ innerSelector.path;
  {lense, path};
};

// Selector wrappers for default lenses

let arrayIndex = i => {
  lense: Refractive__Lense.arrayIndex(i),
  path: "[" ++ string_of_int(i) ++ "]",
};

let arrayLength = filler => {
  lense: Refractive__Lense.arrayLength(filler),
  path: "$length",
};