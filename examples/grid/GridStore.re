let sideSize = 40;

type t = {cells: Immutable.Vector.t(int)};
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
      ~get=v => Immutable.Vector.getOrRaise(idx, v),
      ~set=(newVal, v) => Immutable.Vector.update(idx, newVal, v),
    );
};

module Selectors = {
  open Refractive.Selector;
  let (|-) = compose;
  let cells = make(~lens=Lenses.cells, ~path="cells");
  let pvecIndex = i =>
    make(~lens=Lenses.pvecIndex(i), ~path=string_of_int(i));
  let cellValue = i => cells |- pvecIndex(i);
};

// Module for tracked selectors and modifications
// This module's `modify` function must be used to update the state
// and the `storeEnhancer` function should be used to enhance the store
module Tracked =
  Refractive.TrackedSelector.Make({});

let reducer = (state, action) => {
  Selectors.(
    Tracked.(
      switch (action) {
      | Incr(idx) => modify(cellValue(idx), v => (v + 1) mod 10, state)
      | Reset => modify(cells, _ => initialValue.cells, state)
      | Randomize =>
        modify(
          cells,
          _ =>
            Immutable.Vector.init(sideSize * sideSize, _ => Random.int(10)),
          state,
        )
      }
    )
  );
};

module Context =
  Refractive.Context.Make({
    type state = t;
    type _action = action;
    type action = _action;

    let store =
      Reductive.Store.create(
        ~reducer,
        ~preloadedState=initialValue,
        ~enhancer=Tracked.storeEnhancer,
        (),
      );

    let subscribeSelector = Tracked.subscribe;
  });