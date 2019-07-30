module Selectors = CounterStore.Selectors;
let useDispatch = CounterStore.Context.useDispatch;
let useSelector = CounterStore.Context.useSelector;

module CounterDisplay = {
  let style = ReactDOMRe.Style.make(~margin="1em", ());

  [@react.component]
  let make = (~index) => {
    let selector =
      React.useMemo1(() => Selectors.counterValue(index), [|index|]);
    let count = useSelector(selector);

    <span style> {React.string(string_of_int(count))} </span>;
  };

  let make = React.memo(make);
};

module CounterEditor = {
  [@react.component]
  let make = (~index) => {
    let dispatch = useDispatch();
    let increment =
      React.useCallback1(
        _ => dispatch(CounterStore.IncrementCounter(index)),
        [|index|],
      );
    let decrement =
      React.useCallback1(
        _ => dispatch(CounterStore.DecrementCounter(index)),
        [|index|],
      );

    <div>
      <CounterDisplay index />
      <button onClick=increment> {React.string("++")} </button>
      <button onClick=decrement> {React.string("--")} </button>
    </div>;
  };

  let make = React.memo(make);
};

module CountersControls = {
  [@react.component]
  let make = () => {
    let dispatch = useDispatch();
    let count = useSelector(Selectors.countersCount);
    let append = React.useCallback(_ => dispatch(CounterStore.AppendCounter));
    let removeLast =
      React.useCallback(_ => dispatch(CounterStore.RemoveLastCounter));

    <div>
      <button onClick=append> {React.string("append")} </button>
      <button onClick=removeLast> {React.string("remove last")} </button>
      <ul>
        {React.array(
           Belt.Array.makeBy(count, index =>
             <CounterEditor key={string_of_int(index)} index />
           ),
         )}
      </ul>
    </div>;
  };

  let make = React.memo(make);
};

module CountersSequence = {
  [@react.component]
  let make = () => {
    let count = useSelector(Selectors.countersCount);

    <div>
      {React.array(
         Belt.Array.makeBy(count, index =>
           <CounterDisplay index key={string_of_int(index)} />
         ),
       )}
    </div>;
  };

  let make = React.memo(make);
};

module CountersSum = {
  [@react.component]
  let make = () => {
    let counters = useSelector(Selectors.counters);
    let sum = Belt.Array.reduce(counters, 0, (+));

    <div> {React.string(string_of_int(sum))} </div>;
  };

  let make = React.memo(make);
};

module App = {
  [@react.component]
  let make = () => {
    <CounterStore.Context.Provider>
      <h1> {React.string("Counter example")} </h1>
      <h2> {React.string("Sum")} </h2>
      <CountersSum />
      <h2> {React.string("Counters sequence")} </h2>
      <CountersSequence />
      <h2> {React.string("Counter controls")} </h2>
      <CountersControls />
    </CounterStore.Context.Provider>;
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app-root");