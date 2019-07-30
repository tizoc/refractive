// Temporary solution to automate the generation of lenses and selectors
// for the fields of a record definition.

// The PPX rewriters story in bucklescript seems a bit complicated at the moment.
// This script is a poor man's substitute for a rewriter that derives
// lenses and selectors from a type definition.
// To use pass a list of field names to stdin, one per line.
// This will output implementations for Lenses and Selectors modules.
// To generate interface files for such modules, the method described here
// can be used:
// https://bucklescript.github.io/docs/en/automatic-interface-generation
const fs = require('fs');

const emitLense = name => {
  console.log(`  let ${name} = Refractive.Lense.make(~get=x => x.${name}, ~set=(newVal, x) => { ...x, ${name}: newVal });`);
}

const emitSelector = name => {
  console.log(`  let ${name} = Refractive.Selector.make(~lense=Lenses.${name}, ~path=\"${name}\");`);
}

const emitLensesModule = names => {
    console.log("module Lenses = {");
    names.forEach(emitLense);
    console.log("};");
}

const emitSelectorsModule = names => {
    console.log("module Selectors = {");
    names.forEach(emitSelector, names);
    console.log("};");
}

const stdinBuffer = fs.readFileSync(0);
const names = stdinBuffer.toString().split("\n").filter(s => s !== '');

emitLensesModule(names);
console.log();
emitSelectorsModule(names);