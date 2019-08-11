let sideSize = 30;

type t = {cells: array(array(int))};
let initialValue = {cells: Array.make(sideSize, Array.make(sideSize, 0))};

type action =
  | Incr(int);

module Lenses = {
  let cells =
    Refractive.Lens.make(
      ~get=x => x.cells,
      ~set=(newVal, _) => {cells: newVal},
    );
};

module Selectors = {
  let (|-) = Refractive.Selector.compose;
  let cells = Refractive.Selector.make(~lens=Lenses.cells, ~path="cells");
  let cellValue = i =>
    cells
    |- Refractive.Selector.arrayIndex(i / sideSize)
    |- Refractive.Selector.arrayIndex(i mod sideSize);
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