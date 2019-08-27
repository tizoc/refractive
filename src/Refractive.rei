module Store = Reductive.Store;

module Lens: {
  type lens('childOut, 'childIn, 'parentOut, 'parentIn);
  type t('parent, 'child) = lens('child, 'child, 'parent, 'parent);

  let make:
    (~get: 'parent => 'child, ~set: ('child, 'parent) => 'parent) =>
    t('parent, 'child);

  let lens:
    ('parentIn => ('childIn, 'childOut => 'parentOut)) =>
    lens('childOut, 'childIn, 'parentOut, 'parentIn);

  // Operations
  let view: (t('parent, 'child), 'parent) => 'child;
  let set: (t('parent, 'child), 'child, 'parent) => 'parent;
  let modify: (t('parent, 'child), 'child => 'child, 'parent) => 'parent;
  let compose:
    (t('parent, 'child), t('child, 'grandchild)) => t('parent, 'grandchild);

  // Default lenses
  let arrayIndex: int => t(array('child), 'child);
  let arrayLength: 'filler => t(array('filler), int);
};

module Selector: {
  type t('parent, 'child);

  let make:
    (~lens: Lens.t('parent, 'child), ~path: array(string)) =>
    t('parent, 'child);

  // Operations
  let view: (t('parent, 'child), 'parent) => 'child;
  let set: (t('parent, 'child), 'child, 'parent) => 'parent;
  let modify: (t('parent, 'child), 'child => 'child, 'parent) => 'parent;
  let compose:
    (t('parent, 'child), t('child, 'grandchild)) => t('parent, 'grandchild);

  // Default lenses
  let arrayIndex: int => t(array('a), 'a);
  let arrayLength: 'filler => t(array('filler), int);
};

module TrackedSelector: {
  module Make:
    () =>
     {
      let set: (Selector.t('parent, 'child), 'child, 'parent) => 'parent;
      let modify:
        (Selector.t('parent, 'child), 'child => 'child, 'parent) => 'parent;
      let subscribe:
        (Selector.t('parent, 'child), unit => unit, unit) => unit;
      let notify: unit => unit;
    };
};

module Context: {
  module type CONFIG = {
    type state;
    type action;
    let store: Store.t(action, state);
    let subscribe: (Selector.t(state, 'a), unit => unit, unit) => unit;
    let notify: unit => unit;
  };

  module Make:
    (Config: CONFIG) =>
     {
      module Provider: {
        [@react.component]
        let make: (~children: React.element) => React.element;
      };

      module Hooks: {
        let useDispatch: (unit, Config.action) => unit;
        let useSelector: Selector.t(Config.state, 'value) => 'value;
      };
    };
};