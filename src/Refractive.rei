module Store = Reductive.Store;

module Subscription: {
  type subscriptionState('value) = {
    getCurrentValue: unit => 'value,
    subscribe: (unit => unit, unit) => unit,
    value: 'value,
  };
  let useSubscription:
    (unit => 'value, (unit => unit, unit) => unit) => 'value;
};

module Lense: {
  type t('state, 'value) = {
    get: 'state => 'value,
    set: ('value, 'state) => 'state,
  };

  // Operations
  let read: ('state, t('state, 'value)) => 'value;
  let change: ('value => 'value, 'state, t('state, 'value)) => 'state;
  let compose:
    (t('state, 'valueA), t('valueA, 'valueB)) => t('state, 'valueB);

  // Default lenses
  let arrayIndex: int => t(array('value), 'value);
  let arrayLength: 'filler => t(array('filler), int);
};

module Selector: {
  type t('state, 'value) = {
    lense: Lense.t('state, 'value),
    path: string,
  };

  // Operations
  let read: ('state, t('state, 'value)) => 'value;
  let change: ('value => 'value, 'state, t('state, 'value)) => 'state;
  let compose:
    (t('state, 'valueA), t('valueA, 'valueB)) => t('state, 'valueB);

  // Default lenses
  let arrayIndex: int => t(array('value), 'value);
  let arrayLength: 'filler => t(array('filler), int);
};

module TrackedSelector: {
  module Make:
    () =>
     {
      let change:
        (Selector.t('state, 'value), 'value => 'value, 'state) => 'state;
      let subscribe: (Selector.t('state, 'value), unit => unit, unit) => unit;
      let storeEnhancer:
        (Store.t('action, 'state), 'action => unit, 'action) => unit;
    };
};

module Context: {
  module type CONFIG = {
    type state;
    type action;
    let store: Store.t(action, state);
    let subscribeSelector:
      (Selector.t(state, 'a), unit => unit, unit) => unit;
  };

  module Make:
    (Config: CONFIG) =>
     {
      let context: React.Context.t(Store.t(Config.action, Config.state));

      module Provider: {
        [@react.component]
        let make: (~children: React.element) => React.element;
      };

      let useDispatch: (unit, Config.action) => unit;
      let useSelector: Selector.t(Config.state, 'value) => 'value;
    };
};