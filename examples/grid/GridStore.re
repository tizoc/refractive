let sideSize = 40;

type state = {cells: Immutable.Vector.t(int)};
let initialValue = {
  cells: Immutable.Vector.init(sideSize * sideSize, _ => 0),
};

type action =
  | Incr(int)
  | Reset
  | Randomize;

module Lenses = {
  let cells =
    Refractive.Lens.make(
      ~get=x => x.cells,
      ~set=(newVal, _) => {cells: newVal},
    );
  let pvecIndex = idx =>
    Refractive.Lens.make(
      ~get=Immutable.Vector.getOrRaise(idx),
      ~set=Immutable.Vector.update(idx),
    );
};

module Selectors = {
  open Refractive.Selector;
  let (|-) = compose;
  let cells = make(~lens=Lenses.cells, ~path=[|"cells"|]);
  let pvecIndex = i =>
    make(
      ~lens=Lenses.pvecIndex(i),
      ~path=[|"get(" ++ string_of_int(i) ++ ")"|],
    );
  let cellValue = i => cells |- pvecIndex(i);
};

// Module for tracked selectors and modifications
// This module's `modify` and `set` functions must be used to update the state
include Refractive.TrackedSelector.Make({});

let reducer = (state, action) => {
  Selectors.(
    switch (action) {
    | Incr(idx) => modify(cellValue(idx), v => (v + 1) mod 10, state)
    | Reset => set(cells, initialValue.cells, state)
    | Randomize =>
      set(
        cells,
        Immutable.Vector.init(sideSize * sideSize, _ => Random.int(10)),
        state,
      )
    }
  );
};

let store =
  Reductive.Store.create(~reducer, ~preloadedState=initialValue, ());