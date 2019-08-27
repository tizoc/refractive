module Selector = Refractive__Selector;

module L = Belt.HashMap.String;
module M = Belt.Set.String;

module Make = (()) => {
  let listeners: L.t(list(unit => unit)) = L.make(~hintSize=250);
  let modifications = ref(M.empty);

  let touch = selector =>
    modifications :=
      Array.fold_left(
        M.add,
        modifications^,
        Selector.affectedPaths(selector),
      );

  let set = (selector, value, state) => {
    touch(selector);
    Selector.set(selector, value, state);
  };

  let modify = (selector, f, state) => {
    touch(selector);
    Selector.modify(selector, f, state);
  };

  let unsubscribe = (selector, listener, ()) => {
    selector
    |> Selector.observedPaths
    |> Array.iter(path =>
         switch (L.get(listeners, path)) {
         | None => ()
         | Some(pathListeners) =>
           let matchedListeners =
             List.filter(l => listener !== l, pathListeners);
           L.set(listeners, path, matchedListeners);
         }
       );
  };

  let subscribe = (selector, listener) => {
    selector
    |> Selector.observedPaths
    |> Array.iter(path => {
         let pathListeners =
           Belt.Option.getWithDefault(L.get(listeners, path), []);
         L.set(listeners, path, pathListeners @ [listener]);
       });
    unsubscribe(selector, listener);
  };

  let notifyAllInPath = path => {
    switch (L.get(listeners, path)) {
    | None => ()
    | Some(pathListeners) => List.iter(listener => listener(), pathListeners)
    };
  };

  let notify = () => {
    let paths = modifications^;
    modifications := M.empty;
    M.forEach(paths, notifyAllInPath);
  };
};