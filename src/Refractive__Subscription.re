// Bindigs to useSubscription by @MargaretKrutikova
// https://github.com/MargaretKrutikova/reductive/blob/0c2f20acf399d195586f4f3eaf16008a70da3a76/src/subscription.re
[@bs.deriving {jsConverter: newType}]
type source('a) = {
  subscribe: (unit => unit, unit) => unit,
  getCurrentValue: unit => 'a,
};

[@bs.module "use-subscription"]
external useSubscriptionJs: abs_source('a) => 'a = "useSubscription";

let useSubscription = source => {
  let sourceJs = React.useMemo1(() => sourceToJs(source), [|source|]);
  useSubscriptionJs(sourceJs);
};