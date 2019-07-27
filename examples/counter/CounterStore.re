type t = array(int);
let initialValue = [||];

type action =
  | AppendCounter
  | RemoveLastCounter
  | IncrementCounter(int)
  | DecrementCounter(int);

// Selectors
let countersCount = Refractive.Selector.arrayLength(0);
let counterValue = i => Refractive.Selector.arrayIndex(i);

// Module for tracked selectors and modifications
// This module's `change` function must be used to update the state
// and the `storeEnhancer` function should be used to enhance the store
module Tracked =
  Refractive.TrackedSelector.Make({});

let reducer = (state, action) => {
  let change = Tracked.change;
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