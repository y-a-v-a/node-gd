/**
 * Load C++ bindings for libgd
 * Copyright (c) 2014-2021 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

import path from 'path';
import { fileURLToPath } from 'url';
import { createRequire } from 'module';

const require = createRequire(import.meta.url);
const packageDir = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');

let bindings;

try {
  bindings = require('node-gyp-build')(packageDir);
} catch (error) {
  throw new Error(`node-gd could not load its native addon: ${error.message}`, { cause: error });
}

export default bindings;
