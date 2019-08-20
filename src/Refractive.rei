module Store = Reductive.Store;

module Lens: {
  type t('state, 'value);

  let make:
    (~get: 'state => 'value, ~set: ('value, 'state) => 'state) =>
    t('state, 'value);

  // Operations
  let view: ('state, t('state, 'value)) => 'value;
  let modify: ('value => 'value, 'state, t('state, 'value)) => 'state;
  let compose:
    (t('state, 'valueA), t('valueA, 'valueB)) => t('state, 'valueB);

  // Default lenses
  let arrayIndex: int => t(array('value), 'value);
  let arrayLength: 'filler => t(array('filler), int);
};

module Selector: {
  type t('state, 'value);

  let make:
    (~lens: Lens.t('state, 'value), ~path: array(string)) => t('state, 'value);

  // Operations
  let view: ('state, t('state, 'value)) => 'value;
  let modify: ('value => 'value, 'state, t('state, 'value)) => 'state;
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
      let modify:
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