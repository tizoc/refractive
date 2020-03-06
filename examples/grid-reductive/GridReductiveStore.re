type state = GridStore.state;
type action = GridStore.action;

let sideSize = GridStore.sideSize;
let initialValue = GridStore.initialValue;

let reducer = (state: GridStore.state, action: GridStore.action) => {
  switch (action) {
  | Incr(idx) =>
    let value = (Belt.Map.Int.getExn(state.cells, idx) + 1) mod 10;
    GridStore.{cells: Belt.Map.Int.set(state.cells, idx, value)};
  | Reset => initialValue
  | Randomize => {
      cells: Belt.Map.Int.fromArray(Belt.Array.makeBy(sideSize * sideSize, idx => (idx, Random.int(10)))),
    }
  };
};

let store =
  Reductive.Store.create(~reducer, ~preloadedState=initialValue, ());