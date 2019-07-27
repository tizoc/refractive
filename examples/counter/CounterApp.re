let useDispatch = CounterStore.Context.useDispatch;
let useSelector = CounterStore.Context.useSelector;

module Counter = {
  [@react.component]
  let make = (~index) => {
    let dispatch = useDispatch();
    let selector =
      React.useMemo1(() => CounterStore.counterValue(index), [|index|]);
    let count = useSelector(selector);
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
      {React.string(string_of_int(count))}
      <button onClick=increment> {React.string("++")} </button>
      <button onClick=decrement> {React.string("--")} </button>
    </div>;
  };

  let make = React.memo(make);
};

module CountersList = {
  [@react.component]
  let make = () => {
    let dispatch = useDispatch();
    let count = useSelector(CounterStore.countersCount);
    let append = React.useCallback(_ => dispatch(CounterStore.AppendCounter));
    let removeLast =
      React.useCallback(_ => dispatch(CounterStore.RemoveLastCounter));

    <div>
      <button onClick=append> {React.string("append")} </button>
      <button onClick=removeLast> {React.string("remove last")} </button>
      <ul>
        {React.array(
           Belt.Array.makeBy(count, index =>
             <Counter key={string_of_int(index)} index />
           ),
         )}
      </ul>
    </div>;
  };

  let make = React.memo(make);
};

module App = {
  [@react.component]
  let make = () => {
    <CounterStore.Context.Provider>
      <h1> {React.string("Main")} </h1>
      <CountersList />
      <h1> {React.string("Mirror")} </h1>
      <CountersList />
    </CounterStore.Context.Provider>;
  };
};

ReactDOMRe.renderToElementWithId(<App />, "app-root");