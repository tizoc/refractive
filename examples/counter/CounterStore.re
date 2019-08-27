type state = {counters: array(int)};
let initialValue = {counters: [|0, 0, 0, 0, 0|]};

type action =
  | AppendCounter
  | RemoveLastCounter
  | IncrementCounter(int)
  | DecrementCounter(int);

module Lenses = {
  let counters =
    Refractive.Lens.make(
      ~get=x => x.counters,
      ~set=(newVal, _) => {counters: newVal},
    );
};

module Selectors = {
  let (|-) = Refractive.Selector.compose;
  let counters =
    Refractive.Selector.make(~lens=Lenses.counters, ~path=[|"counter"|]);
  let countersCount = counters |- Refractive.Selector.arrayLength(0);
  let counterValue = i => counters |- Refractive.Selector.arrayIndex(i);
};

// Module for tracked selectors and modifications
// This module's `modify` and `set` functions must be used to update the state
include Refractive.TrackedSelector.Make({});

let reducer = (state, action) => {
  Selectors.(
    switch (action) {
    | AppendCounter => modify(countersCount, count => count + 1, state)
    | RemoveLastCounter => modify(countersCount, count => count - 1, state)
    | IncrementCounter(index) =>
      modify(counterValue(index), n => n + 1, state)
    | DecrementCounter(index) =>
      modify(counterValue(index), n => n - 1, state)
    }
  );
};

let store =
  Reductive.Store.create(~reducer, ~preloadedState=initialValue, ());