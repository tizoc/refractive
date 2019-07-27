const path = require("path")

module.exports = {
  mode: "development",
  entry: {
    counter: "./lib/js/examples/counter/CounterApp.bs.js",
  },
  output: {
    path: path.join(__dirname, "bundledOutputs"),
    filename: "[name].js"
  }
}