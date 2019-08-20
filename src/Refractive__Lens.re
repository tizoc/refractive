let (@.) = (g, f, x) => g(f(x));

type lens('childOut, 'childIn, 'parentOut, 'parentIn) =
  'parentIn => ('childIn, 'childOut => 'parentOut);
type t('parent, 'child) = lens('child, 'child, 'parent, 'parent);

let make = (~get, ~set, t) => (get(t), set(_, t));

let lens = f => f;

let view = lens => fst @. lens;

let modify = (lens, f, state) => {
  let (value, set) = lens(state);
  set(f(value));
};

let set = (lens, value, state) => {
  let (_, set) = lens(state);
  set(value);
};

let compose = (outerLens, innerLens, parent) => {
  let (outerChild, setOuter) = outerLens(parent);
  let (innerChild, setInner) = innerLens(outerChild);
  (innerChild, setOuter @. setInner);
};

// Default lenses

let arrayIndex = (i, arr) => (
  arr[i],
  value => {
    let result = Array.copy(arr);
    result[i] = value;
    result;
  },
);

let arrayLength = (filler, arr) => (
  Array.length(arr),
  length => {
    let current = Array.length(arr);
    if (length >= current) {
      Array.append(arr, Array.make(length - current, filler));
    } else {
      Array.sub(arr, 0, max(0, length));
    };
  },
);