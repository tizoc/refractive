module Selectors = GridStore.Selectors;
let useDispatch = GridStore.Context.useDispatch;
let useSelector = GridStore.Context.useSelector;

module GridCell = {
  let colors = [|
    "White",
    "LightCyan",
    "LightBlue",
    "LightGreen",
    "LightPink",
    "LightSalmon",
    "Orange",
    "PaleGreen",
    "Yellow",
    "Tan",
  |];

  let style = n =>
    ReactDOMRe.Style.make(
      ~padding="3px 8px",
      ~backgroundColor=colors[n],
      ~color="Black",
      ~display="inline-block",
      ~borderRadius=string_of_int(Random.int(12)) ++ "px",
      (),
    );

  [@react.component]
  let make = (~index) => {
    let dispatch = useDispatch();
    let selector =
      React.useMemo1(() => Selectors.cellValue(index), [|index|]);
    let cycle =
      React.useCallback1(_ => dispatch(GridStore.Incr(index)), [|index|]);
    let value = useSelector(selector);
    let style = style(value);

    <span style onMouseEnter=cycle>
      {React.string(string_of_int(value))}
    </span>;
  };

  let make = React.memo(make);
};

module GridRow = {
  [@react.component]
  let make = (~row) => {
    <div>
      {React.array(
         Belt.Array.makeBy(GridStore.sideSize, col =>
           <GridCell
             index={row * GridStore.sideSize + col}
             key={string_of_int(col)}
           />
         ),
       )}
    </div>;
  };

  let make = React.memo(make);
};

module GridContainer = {
  let style = ReactDOMRe.Style.make(~fontFamily="Monospace", ());

  [@react.component]
  let make = () => {
    <div style>
      {React.array(
         Belt.Array.makeBy(GridStore.sideSize, row =>
           <GridRow row key={string_of_int(row)} />
         ),
       )}
    </div>;
  };

  let make = React.memo(make);
};

module App = {
  [@react.component]
  let make = () => {
    <GridStore.Context.Provider>
      <h1> {React.string("Grid example")} </h1>
      <GridContainer />
    </GridStore.Context.Provider>;
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app-root");