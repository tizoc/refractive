open Jest;
open Expect;

describe("Unfold path", () => {
  open Refractive__Selector;

  test("Unfolds straight path correctly", () => {
    expect(Array.of_list @@ unfoldPath(Straight([|"c", "b", "a"|])))
    |> toEqual([|"a.b.c", "a.b", "a"|])
  });

  test("Unfolds forked path with empty prefix correctly", () => {
    expect(
      Array.of_list @@
      unfoldPath(
        Forked(
          [||],
          [Straight([|"c", "b", "a"|]), Straight([|"b"|])],
          "join",
          None,
        ),
      ),
    )
    |> toEqual([|"a.b.c", "a.b", "a", "b"|])
  });

  test("Unfolds forked path with prefix correctly", () => {
    expect(
      Array.of_list @@
      unfoldPath(
        Forked(
          [|"p1", "p0"|],
          [Straight([|"c", "b", "a"|]), Straight([|"f", "e"|])],
          "join",
          None,
        ),
      ),
    )
    |> toEqual([|
         "p0.p1.a.b.c",
         "p0.p1.a.b",
         "p0.p1.e.f",
         "p0.p1.a",
         "p0.p1.e",
         "p0.p1",
         "p0",
       |])
  });

  test("Unfolds more complicated forked path with prefix correctly", () => {
    expect(
      Array.of_list @@
      unfoldPath(
        Forked(
          [|"p1", "p0"|],
          [
            Straight([|"c", "b", "a"|]),
            Forked(
              [||],
              [Straight([|"h", "g"|]), Straight([|"f", "e"|])],
              "join",
              None,
            ),
          ],
          "join",
          None,
        ),
      ),
    )
    |> toEqual([|
         "p0.p1.a.b.c",
         "p0.p1.a.b",
         "p0.p1.e.f",
         "p0.p1.g.h",
         "p0.p1.a",
         "p0.p1.e",
         "p0.p1.g",
         "p0.p1",
         "p0",
       |])
  });
});

describe("Selector paths", () => {
  open Refractive__Selector;

  test("String rendering of straight paths", () => {
    expect(stringOfPath(Straight([|"b"|]))) |> toEqual("b")
  });

  test("String rendering of another straight path", () => {
    expect(stringOfPath(Straight([|"a", "b", "c"|]))) |> toEqual("c.b.a")
  });

  test("String rendering of forked path without prefix", () => {
    expect(
      stringOfPath(
        Forked(
          [||],
          [Straight([|"c", "b", "a"|]), Straight([|"b"|])],
          "join",
          None,
        ),
      ),
    )
    |> toEqual("join(a.b.c, b)")
  });

  test("String rendering of forked path with prefix", () => {
    expect(
      stringOfPath(
        Forked(
          [|"p1", "p0"|],
          [Straight([|"c", "b", "a"|]), Straight([|"b"|])],
          "join",
          None,
        ),
      ),
    )
    |> toEqual("p0.p1.join(a.b.c, b)")
  });

  test("Composing two straight paths", () => {
    expect(
      stringOfPath(
        composePath(Straight([|"p1", "p0"|]), Straight([|"c", "b", "a"|])),
      ),
    )
    |> toEqual("p0.p1.a.b.c")
  });

  test("Composing a straight path with a fork", () => {
    expect(
      stringOfPath(
        composePath(
          Straight([|"p1", "p0"|]),
          Forked(
            [||],
            [
              Straight([|"c", "b", "a"|]),
              Straight([|"b", "a"|]),
              Straight([|"b"|]),
            ],
            "join",
            None,
          ),
        ),
      ),
    )
    |> toEqual("p0.p1.join(a.b.c, a.b, b)")
  });

  test("Composing a forked path with a straight path", () => {
    expect(
      stringOfPath(
        composePath(
          Forked(
            [||],
            [
              Straight([|"c", "b", "a"|]),
              Straight([|"b", "a"|]),
              Straight([|"b"|]),
            ],
            "join",
            None,
          ),
          Straight([|"p1", "p0"|]),
        ),
      ),
    )
    |> toEqual("join(a.b.c, a.b, b).p0.p1")
  });

  test("Composing a forked path with a forked path", () => {
    expect(
      stringOfPath(
        composePath(
          Forked(
            [|"d"|],
            [
              Straight([|"c", "b", "a"|]),
              Straight([|"b", "a"|]),
              Straight([|"b"|]),
            ],
            "join",
            None,
          ),
          Forked(
            [|"z"|],
            [
              Straight([|"x"|]),
              Straight([|"y"|]),
            ],
            "join",
            None,
          ),
        ),
      ),
    )
    |> toEqual("d.join(a.b.c, a.b, b).z.join(x, y)")
  });

  test("Joined paths are constructed correctly", () => {
    let s1 = make(~lens=Lens.const(1), ~path=[|"p1", "p0"|]);
    let s2 = make(~lens=Lens.const(1), ~path=[|"p3", "p2"|]);
    let mapped = map2(~name="map2", (a, b) => a + b, s1, s2);
    expect(mapped.path)
    |> toEqual(
         Forked(
           [||],
           [Straight([|"p1", "p0"|]), Straight([|"p3", "p2"|])],
           "map2",
           None,
         ),
       );
  });

  test("Joined paths are printed correctly", () => {
    let s1 = make(~lens=Lens.const(1), ~path=[|"p1", "p0"|]);
    let s2 = make(~lens=Lens.const(1), ~path=[|"p3", "p2"|]);
    let mapped = map2(~name="map2", (a, b) => a + b, s1, s2);
    expect(stringOfPath(mapped.path)) |> toEqual("map2(p0.p1, p2.p3)");
  });

  test("Joined paths are joined correctly", () => {
    let s1 = make(~lens=Lens.const(1), ~path=[|"p1", "p0"|]);
    let s2 = make(~lens=Lens.const(1), ~path=[|"p3", "p2"|]);
    let mapped1 = map2(~name="map2<a>", (a, b) => a + b, s1, s2);
    let mapped2 = map2(~name="map2<b>", (a, b) => a - b, s1, s2);
    let mapped = map2(~name="join", (a, b) => a * b, mapped1, mapped2);
    expect(stringOfPath(mapped.path))
    |> toEqual("join(map2<a>(p0.p1, p2.p3), map2<b>(p0.p1, p2.p3))");
  });
});

module TestStore = {
  type state = {
    counters: array(int),
    textA: string,
    textB: string,
  };
  let initialValue = {counters: [|0, 0, 0, 0, 0|], textA: "A", textB: "B"};

  type action =
    | Reset
    | SetTextA(string)
    | SetTextB(string)
    | IncrementAll
    | IncrementCounter(int);

  module Lenses = {
    open Refractive.Lens;
    let counters =
      make(~get=x => x.counters, ~set=(counters, x) => {...x, counters});
    let textA =
      make(~get=x => x.textA, ~set=(newVal, x) => {...x, textA: newVal});
    let textB =
      make(~get=x => x.textB, ~set=(newVal, x) => {...x, textB: newVal});
  };

  module Selectors = {
    open Refractive.Selector;
    let (|-) = compose;
    let counters = make(~lens=Lenses.counters, ~path=[|"counter"|]);
    let countersCount = counters |- arrayLength(0);
    let counterValue = i => counters |- arrayIndex(i);
    let textA = make(~lens=Lenses.textA, ~path=[|"textA"|]);
    let textB = make(~lens=Lenses.textB, ~path=[|"textB"|]);
  };

  include Refractive.TrackedSelector.Make({});

  let reducer = (state, action) => {
    Selectors.(
      switch (action) {
      | Reset =>
        state
        |> set(counters, initialValue.counters)
        |> set(textA, initialValue.textA)
        |> set(textB, initialValue.textB)
      | SetTextA(s) => set(textA, s, state)
      | SetTextB(s) => set(textB, s, state)
      | IncrementCounter(index) =>
        modify(counterValue(index), n => n + 1, state)
      | IncrementAll =>
        modify(counters, c => Array.map(n => n + 1, c), state)
      }
    );
  };

  let store =
    Reductive.Store.create(~reducer, ~preloadedState=initialValue, ());
};

module TestStoreContext = Refractive.Context.Make(TestStore);

let revArray = list => List.rev(list) |> Array.of_list;

describe("useSelector", () => {
  open ReasonHooksTestingLibrary.Testing;
  open ReactTestingLibrary;

  module Wrapper = {
    [@react.component]
    let make = (~children) => {
      <TestStoreContext.Provider> children </TestStoreContext.Provider>;
    };
  };

  module Selectors = TestStore.Selectors;

  let useSelector = TestStoreContext.Hooks.useSelector;

  let renderHookWrapped = render =>
    renderHook(render, ~options=Options.t(~wrapper=Wrapper.make, ()), ());

  beforeEach(() =>
    Refractive.Store.dispatch(TestStore.store, TestStore.Reset)
  );

  test("returns the correct selector value", () => {
    let container = renderHookWrapped(() => useSelector(Selectors.textA));

    expect(Result.(container->result->current)) |> toEqual("A");
  });

  test("returns the correct selector value after a modification", () => {
    let container = renderHookWrapped(() => useSelector(Selectors.textA));

    act(() =>
      Reductive.Store.dispatch(TestStore.store, TestStore.SetTextA("Z"))
    );

    expect(Result.(container->result->current)) |> toEqual("Z");
  });

  test(
    "replacing parent value runs notifications for subscriptions on children",
    () => {
    let renderedStates = ref([]);

    module Component = {
      [@react.component]
      let make = () => {
        let value = useSelector(Selectors.counterValue(0));
        renderedStates := [value, ...renderedStates^];

        <div> {React.string(string_of_int(value))} </div>;
      };
    };

    ignore @@ render(<Wrapper> <Component /> </Wrapper>);

    act(() => {
      Reductive.Store.dispatch(TestStore.store, TestStore.IncrementAll);
      Reductive.Store.dispatch(TestStore.store, TestStore.IncrementAll);
    });

    expect(revArray(renderedStates^)) |> toEqual([|0, 1, 2|]);
  });

  // NOTE: we count calls to the component render functions
  // and equate that to the amount of times the notification
  // callback for that component was called. This works
  // because (for now at least) setState (which is always invoked by
  // the callback) will trigger a call to the render function,
  // even when the value returned by the update function is
  // the same of the original[1]. Because of this, we can assume
  // that if render was not called, then the notification
  // callback function wasn't called either.
  //
  // [1]: https://github.com/facebook/react/issues/14994

  test("only components subscribed to path get notified", () => {
    let renderCountA = ref(0);
    let renderCountB = ref(0);

    module ComponentA = {
      [@react.component]
      let make = () => {
        let value = useSelector(Selectors.textA);
        incr(renderCountA);

        <div> {React.string(value)} </div>;
      };
    };

    module ComponentB = {
      [@react.component]
      let make = () => {
        let value = useSelector(Selectors.textB);
        incr(renderCountB);

        <div> {React.string(value)} </div>;
      };
    };

    ignore @@ render(<Wrapper> <ComponentA /> <ComponentB /> </Wrapper>);

    act(() => {
      Reductive.Store.dispatch(TestStore.store, TestStore.SetTextB("C"));
      Reductive.Store.dispatch(TestStore.store, TestStore.SetTextB("D"));
    });

    expect((renderCountA^, renderCountB^)) |> toEqual((1, 3));
  });

  test("selector that uses props on instantiation works as expected", () => {
    let renderedStates = ref([]);
    let incrIndex = ref(() => ());

    module Component = {
      [@react.component]
      let make = (~index) => {
        let value = useSelector(Selectors.counterValue(index));
        renderedStates := [value, ...renderedStates^];

        <div> {React.string(string_of_int(value))} </div>;
      };
    };

    module Parent = {
      [@react.component]
      let make = () => {
        let (index, dispatch) = React.useReducer((s, _) => s + 1, 0);
        incrIndex := dispatch;
        <Component index />;
      };
    };

    ignore @@ render(<Wrapper> <Parent /> </Wrapper>);

    act(() => {
      Reductive.Store.dispatch(
        TestStore.store,
        TestStore.IncrementCounter(1),
      );
      Reductive.Store.dispatch(
        TestStore.store,
        TestStore.IncrementCounter(2),
      );
      Reductive.Store.dispatch(
        TestStore.store,
        TestStore.IncrementCounter(2),
      );
      incrIndex^();
      incrIndex^();
    });

    // States are duplicated because every time
    // the prop changes the selector changes too, which results
    // in one call to render because of the prop change and
    // another call because of the selector change
    expect(revArray(renderedStates^)) |> toEqual([|0, 1, 1, 2, 2|]);
  });

  test(
    "changing child runs notifications for subscriptions on parent paths", () => {
    let renderedStates = ref([]);

    module Component = {
      [@react.component]
      let make = () => {
        let counters = useSelector(Selectors.counters);
        let sum = Belt.Array.reduce(counters, 0, (+));

        renderedStates := [sum, ...renderedStates^];

        <div> {React.string(string_of_int(sum))} </div>;
      };
    };

    ignore @@ render(<Wrapper> <Component /> </Wrapper>);

    act(() => {
      Reductive.Store.dispatch(
        TestStore.store,
        TestStore.IncrementCounter(1),
      );
      Reductive.Store.dispatch(
        TestStore.store,
        TestStore.IncrementCounter(2),
      );
    });

    expect(revArray(renderedStates^)) |> toEqual([|0, 1, 2|]);
  });
});