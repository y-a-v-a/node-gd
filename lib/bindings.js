'use strict';

/**
 * Load C++ bindings for libgd
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */


const path = require('path');

const libPaths = [
  path.normalize(`${__dirname}/../build/Release/node_gd.node`),
  path.normalize(`${__dirname}/../build/default/node_gd.node`)
];

let bindings;

try {
  bindings = require(libPaths.shift());
} catch (e) {
  console.log(e.message);
  try {
    bindings = require(libPaths.shift());
  } catch (e) {
    console.log(e.message);
    console.log('Unable to find addon node_gd.node in build directory.');
    process.exit(1);
  }
}

module.exports = bindings;
