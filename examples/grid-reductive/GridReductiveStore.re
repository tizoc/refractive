type state = GridStore.state;
type action = GridStore.action;

let sideSize = GridStore.sideSize;
let initialValue = GridStore.initialValue;

let reducer = (state: GridStore.state, action: GridStore.action) => {
  switch (action) {
  | Incr(idx) =>
    let value = (Immutable.Vector.getOrRaise(idx, state.cells) + 1) mod 10;
    GridStore.{cells: Immutable.Vector.update(idx, value, state.cells)};
  | Reset => initialValue
  | Randomize => {
      cells: Immutable.Vector.init(sideSize * sideSize, _ => Random.int(10)),
    }
  };
};

let store =
  Reductive.Store.create(~reducer, ~preloadedState=initialValue, ());