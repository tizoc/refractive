const path = require("path")

module.exports = {
  mode: "development",
  entry: {
    counter: "./lib/js/examples/counter/CounterApp.bs.js",
    grid: "./lib/js/examples/grid/GridApp.bs.js",
    gridConcurrent: "./lib/js/examples/grid-concurrent/GridConcurrentApp.bs.js",
    gridReductive: "./lib/js/examples/grid-reductive/GridReductiveApp.bs.js"
  },
  output: {
    path: path.join(__dirname, "bundledOutputs"),
    filename: "[name].js"
  }
}