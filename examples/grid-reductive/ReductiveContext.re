// Code in this file was adapted from:
// https://github.com/MargaretKrutikova/reductive/blob/hooks-subscription/src/reductiveContext.re
module type Config = {
  type state;
  type action;

  let store: Reductive.Store.t(action, state);
};

module Make = (Config: Config) => {
  let storeContext = React.createContext(Config.store);

  module ContextProvider = {
    let make = React.Context.provider(storeContext);
    let makeProps = (~value, ~children, ()) => {
      "value": value,
      "children": children,
    };
  };

  module Provider = {
    [@react.component]
    let make = (~children) => {
      <ContextProvider value=Config.store> children </ContextProvider>;
    };
  };

  type source('a) = {
    subscribe: (unit => unit, unit) => unit,
    getCurrentValue: unit => 'a,
    value: 'a,
  };

  let useSelector = selector => {
    let storeFromContext = React.useContext(storeContext);
    let subscribe =
      React.useCallback1(
        Reductive.Store.subscribe(storeFromContext),
        [|storeFromContext|],
      );

    let getCurrentValue =
      React.useCallback2(
        () => selector(Reductive.Store.getState(storeFromContext)),
        (selector, storeFromContext),
      );

    let selectedState = Refractive__Subscription.useSubscription(getCurrentValue, subscribe);

    selectedState;
  };

  let useDispatch = () => {
    let storeFromContext = React.useContext(storeContext);
    Reductive.Store.dispatch(storeFromContext);
  };
};