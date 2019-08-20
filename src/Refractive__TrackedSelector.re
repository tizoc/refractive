module Selector = Refractive__Selector;

module Make = (()) => {
  let listeners: Belt.HashMap.String.t(list(unit => unit)) =
    Belt.HashMap.String.make(~hintSize=250);

  let modifications = ref(Belt.Set.String.empty);

  let modify = (selector, f, state) => {
    modifications :=
      Array.fold_left(
        Belt.Set.String.add,
        modifications^,
        Selector.affectedPaths(selector),
      );
    Selector.modify(f, state, selector);
  };

  let unsubscribe = (selector, listener, ()) => {
    selector
    |> Selector.observedPaths
    |> Array.iter(path =>
         switch (Belt.HashMap.String.get(listeners, path)) {
         | None => ()
         | Some(pathListeners) =>
           let matchedListeners =
             List.filter(l => listener !== l, pathListeners);
           Belt.HashMap.String.set(listeners, path, matchedListeners);
         }
       );
  };

  let subscribe = (selector, listener) => {
    selector
    |> Selector.observedPaths
    |> Array.iter(path => {
         let pathListeners =
           Belt.Option.getWithDefault(
             Belt.HashMap.String.get(listeners, path),
             [],
           );
         Belt.HashMap.String.set(
           listeners,
           path,
           pathListeners @ [listener],
         );
       });
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