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

  let const: 'child => t('parent, 'child);

  let map: ('child => 'mapped, t('parent, 'child)) => t('parent, 'mapped);
  let map2:
    (
      ('child1, 'child2) => 'mapped,
      t('parent, 'child1),
      t('parent, 'child2)
    ) =>
    t('parent, 'mapped);
  let map3:
    (
      ('child1, 'child2, 'child3) => 'mapped,
      t('parent, 'child1),
      t('parent, 'child2),
      t('parent, 'child3)
    ) =>
    t('parent, 'mapped);
  let map4:
    (
      ('child1, 'child2, 'child3, 'child4) => 'mapped,
      t('parent, 'child1),
      t('parent, 'child2),
      t('parent, 'child3),
      t('parent, 'child4)
    ) =>
    t('parent, 'mapped);

  let pair:
    (t('parent, 'leftChild), t('parent, 'rightChild)) =>
    t('parent, ('leftChild, 'rightChild));

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

  let const: (~name: string=?, 'child) => t('parent, 'child);

  let map:
    (~name: string, 'child => 'mapped, t('parent, 'child)) =>
    t('parent, 'mapped);
  let map2:
    (
      ~name: string,
      ('child1, 'child2) => 'mapped,
      t('parent, 'child1),
      t('parent, 'child2)
    ) =>
    t('parent, 'mapped);
  let map3:
    (
      ~name: string,
      ('child1, 'child2, 'child3) => 'mapped,
      t('parent, 'child1),
      t('parent, 'child2),
      t('parent, 'child3)
    ) =>
    t('parent, 'mapped);
  let map4:
    (
      ~name: string,
      ('child1, 'child2, 'child3, 'child4) => 'mapped,
      t('parent, 'child1),
      t('parent, 'child2),
      t('parent, 'child3),
      t('parent, 'child4)
    ) =>
    t('parent, 'mapped);

  let pair:
    (t('parent, 'leftChild), t('parent, 'rightChild)) =>
    t('parent, ('leftChild, 'rightChild));

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