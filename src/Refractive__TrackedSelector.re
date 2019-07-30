module Selector = Refractive__Selector;

module Make = (()) => {
  let listeners: Belt.HashMap.String.t(list(unit => unit)) =
    Belt.HashMap.String.make(~hintSize=32);

  let modifications = ref(Belt.Set.String.empty);

  let change = (selector, f, state) => {
    modifications :=
      List.fold_left(Belt.Set.String.add, modifications^, Selector.touchedPaths(selector));
    Selector.change(f, state, selector);
  };

  let unsubscribe = (selector, listener, ()) => {
    let path = Selector.path(selector);
    switch (Belt.HashMap.String.get(listeners, path)) {
    | None => ()
    | Some(pathListeners) =>
      let matchedListeners = List.filter(l => listener !== l, pathListeners);
      Belt.HashMap.String.set(listeners, path, matchedListeners);
    };
  };

  let subscribe = (selector, listener) => {
    let path = Selector.path(selector);
    let pathListeners =
      Belt.Option.getWithDefault(
        Belt.HashMap.String.get(listeners, path),
        [],
      );
    Belt.HashMap.String.set(listeners, path, pathListeners @ [listener]);
    unsubscribe(selector, listener);
  };

  let notifyAllInPath = path => {
    switch (Belt.HashMap.String.get(listeners, path)) {
    | None => ()
    | Some(pathListeners) => List.iter(listener => listener(), pathListeners)
    };
  };

  let storeEnhancer = (_store, dispatch, action) => {
    dispatch(action);
    let paths = modifications^;
    modifications := Belt.Set.String.empty;
    Belt.Set.String.forEach(paths, notifyAllInPath);
  };
};