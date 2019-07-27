type t('state, 'value) = {
  get: 'state => 'value,
  set: ('value, 'state) => 'state,
};

let read = (state, lense) => lense.get(state);

let change = (f, state, lense) => lense.set(f(lense.get(state)), state);

let compose = (outerLense, innerLense) => {
  let get = state => state |> outerLense.get |> innerLense.get;
  let set = (value, state) => {
    let updated = state |> outerLense.get |> innerLense.set(value);
    outerLense.set(updated, state);
  };
  {get, set};
};

// Default lenses

let arrayIndex = i => {
  get: arr => arr[i],
  set: (value, arr) => {
    let result = Array.copy(arr);
    result[i] = value;
    result;
  },
};

let arrayLength = filler => {
  get: Array.length,
  set: (length, arr) => {
    let current = Array.length(arr);
    if (length >= current) {
      Array.append(arr, Array.make(length - current, filler));
    } else {
      Array.sub(arr, 0, max(0, length));
    };
  },
};