type t = {counters: array(int)};
let initialValue = {counters: [|0, 0, 0, 0, 0|]};

type action =
  | AppendCounter
  | RemoveLastCounter
  | IncrementCounter(int)
  | DecrementCounter(int);

module Lenses = {
  let counters =
    Refractive.Lense.make(
      ~get=x => x.counters,
      ~set=(newVal, _) => {counters: newVal},
    );
};

module Selectors = {
  let (|-) = Refractive.Selector.compose;
  let counters =
    Refractive.Selector.make(~lense=Lenses.counters, ~path="counter");
  let countersCount = counters |- Refractive.Selector.arrayLength(0);
  let counterValue = i => counters |- Refractive.Selector.arrayIndex(i);
};

// Module for tracked selectors and modifications
// This module's `change` function must be used to update the state
// and the `storeEnhancer` function should be used to enhance the store
module Tracked =
  Refractive.TrackedSelector.Make({});

let reducer = (state, action) => {
  open Selectors;
  open Tracked;
  switch (action) {
  | AppendCounter => change(countersCount, count => count + 1, state)
  | RemoveLastCounter => change(countersCount, count => count - 1, state)
  | IncrementCounter(index) =>
    change(counterValue(index), n => n + 1, state)
  | DecrementCounter(index) =>
    change(counterValue(index), n => n - 1, state)
  };
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