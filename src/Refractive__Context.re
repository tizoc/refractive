module Store = Reductive.Store;
module Selector = Refractive__Selector;

let useSubscription = Refractive__Subscription.useSubscription;

module type CONFIG = {
  type state;
  type action;

  let store: Store.t(action, state);
  let subscribeSelector: (Selector.t(state, 'a), unit => unit, unit) => unit;
};

module Make = (Config: CONFIG) => {
  let context = React.createContext(Config.store);

  module Provider = {
    [@react.component]
    let make = (~children) => {
      React.createElement(
        context->React.Context.provider,
        {"value": Config.store, "children": children},
      );
    };
  };

  let useDispatch = () => Store.dispatch(React.useContext(context));

  let useSelector = selector => {
    let store = React.useContext(context);
    let subscribe =
      React.useCallback1(
        handler => Config.subscribeSelector(selector, handler),
        [|selector.Selector.path|],
      );
    let getCurrentValue =
      React.useCallback1(
        () => Selector.read(Store.getState(store), selector),
        [|selector.path|],
      );
    useSubscription(getCurrentValue, subscribe);
    Selector.read(Store.getState(store), selector);
  };
};