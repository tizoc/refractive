[@bs.obj]
external makeProps:
  (~children: React.element=?, unit) => {. "children": option(React.element)} =
  "";
[@bs.module "react"]
external make: React.component({. "children": option(React.element)}) =
  "unstable_ConcurrentMode";