module Store = Reductive.Store;
module Selector = Refractive__Selector;

let useSubscription = Refractive__Subscription.useSubscription;

module type CONFIG = {
  type state;
  type action;

  let store: Store.t(action, state);

  let subscribe: (Selector.t(state, 'a), unit => unit, unit) => unit;
  let notify: unit => unit;
};

module Make = (Config: CONFIG) => {
  let context = React.createContext(Config.store);

  Store.subscribe(Config.store, Config.notify);

  module Provider = {
    [@react.component]
    let make = (~children) => {
      React.createElement(
        context->React.Context.provider,
        {"value": Config.store, "children": children},
      );
    };
  };

  module Hooks = {
    let useDispatch = () => Store.dispatch(React.useContext(context));

    let useSelector = selector => {
      let store = React.useContext(context);
      let subscribe =
        React.useCallback1(
          handler => Config.subscribe(selector, handler),
          [|Selector.pathId(selector)|],
        );
      let getCurrentValue =
        React.useCallback1(
          () => Selector.view(selector, Store.getState(store)),
          [|Selector.pathId(selector)|],
        );
      useSubscription({getCurrentValue, subscribe});
    };
  };
};