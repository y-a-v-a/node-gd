/**
 * Load C++ bindings for libgd
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

import path from 'path';
import { fileURLToPath } from 'url';
import { createRequire } from 'module';
const require = createRequire(import.meta.url);

const filePath = fileURLToPath(import.meta.url);
const dirname = path.dirname(filePath);

const libPaths = [
  path.normalize(`${dirname}/../build/Release/node_gd.node`),
  path.normalize(`${dirname}/../build/default/node_gd.node`),
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

export default bindings;
