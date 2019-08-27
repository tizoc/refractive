# Refractive

Lenses and tracked selectors hooks for [reductive](https://github.com/reasonml-community/reductive).

## NOTE

**This is proof of concept at the moment, and while it works, the API is not stable.**

## Preview

```reasonml
module Store = {
  type state = {counter: int};

  type action =
    | Increment
    | Decrement;

  // Lenses specify how to read and update part
  // of the state in a functional way.
  module Lenses = {
    let counter =
      Refractive.Lens.make(
        ~get=s => s.counter,
        ~set=(newVal, s) => {...s, counter: newVal},
      );
  };

  // Selectors are the combination of a Lense and
  // a path to the value it affects.
  module Selectors = {
    let counter =
      Refractive.Selector.make(~lens=Lenses.counter, ~path=[|"counter"|]);
  };

  // Module for tracked selectors and updates
  // This module's `modify` and `set` functions must be used to update the state.
  include Refractive.TrackedSelector.Make({});

  // Reducer function, updates must be made using the `modify` and `set` functinos
  // from the instantiated tracked selectors module.
  let reducer = (state, action) => {
    Selectors.(
      switch (action) {
      | Increment => modify(counter, n => n + 1, state)
      | Decrement => modify(counter, n => n - 1, state)
      }
    );
  };

  let store =
    Reductive.Store.create(~reducer, ~preloadedState={counter: 0}, ());
};

// This module contains the `useDispatch` and `useSelector` hooks
// and a Provider component that provides the necessary context
// for those hooks.
// Instantiating this module also creates a new subscription
// for the selectors.
module StoreContext = Refractive.Context.Make(Store);

module AppComponent = {
  open StoreContext.Hooks; // useDispatch and useSelector

  [@react.component]
  let make = () => {
    let dispatch = useDispatch();
    let handleIncr = React.useCallback0(_ => dispatch(Store.Increment));
    let handleDecr = React.useCallback0(_ => dispatch(Store.Decrement));
    let value = useSelector(Store.Selectors.counter);

    <StoreContext.Provider>
      <span> {React.string(string_of_int(value))} </span>
      <button onClick=handleIncr> {React.string("+")} </button>
      <button onClick=handleDecr> {React.string("-")} </button>
    </StoreContext.Provider>;
  };
};

ReactDOMRe.renderToElementWithId(<AppComponent />, "app-root");
```

See `examples/` directory for more examples.

## The problem

The way Redux and Reductive work, every component that is interested in part of the store's state value subscribes to changes to the store, and receives a notification after an action has been dispatched. But most components care only about a small part of the store state, and most of the time it doesn't change often. This means that a lot of unnecessary notifications are executed. When there aren't too many components subscribed to the store, this is not an issue, but as more components get subscribed to the store, more overhead accumulates.

## Refractive's solution

Refractive avoids this problem by subscribing to changes to the stoure through **selectors** that encode a path to the part of the store's state that the component is interested in. Reducer functions update the store using **selectors** and the special `set` and `modify` functions. Modifications made using these functions are tracked, and only the components that are subscribed to values affected during the reducing function execution are notified of the change.

## Examples

To run the example:

    yarn install
    yarn build
    yarn webpack

and then open `index.html` of any of the examples under the `examples/` directory in your browser.
