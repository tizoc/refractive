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

// TODO: raise exception when trying to set (or log a warning)
let const = (value, state) => (value, _ => state);

let pair = (leftLens, rightLens, parent) => {
  let (leftChild, setLeft) = leftLens(parent);
  let (rightChild, _) = rightLens(parent);
  let setPairs = ((l, r)) => {
    let parentLeftUpdated = setLeft(l);
    let (_, setRight) = rightLens(parentLeftUpdated);
    setRight(r);
  };
  ((leftChild, rightChild), setPairs);
};

let map = (f, lens, parent) => {
  let (value, _) = lens(parent);
  (f(value), _ => assert(false));
};

let map2 = (f, lens1, lens2, parent) => {
  let (value1, _) = lens1(parent);
  let (value2, _) = lens2(parent);
  (f(value1, value2), _ => assert(false));
};

let map3 = (f, lens1, lens2, lens3, parent) => {
  let (value1, _) = lens1(parent);
  let (value2, _) = lens2(parent);
  let (value3, _) = lens3(parent);
  (f(value1, value2, value3), _ => assert(false));
};

let map4 = (f, lens1, lens2, lens3, lens4, parent) => {
  let (value1, _) = lens1(parent);
  let (value2, _) = lens2(parent);
  let (value3, _) = lens3(parent);
  let (value4, _) = lens4(parent);
  (f(value1, value2, value3, value4), _ => assert(false));
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