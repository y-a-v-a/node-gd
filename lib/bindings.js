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

// Windows binaries are published as optional platform packages, see .github/scripts/windows-packages.mjs
const platformPackage = `@node-gd/${process.platform}-${process.arch}`;

function load() {
  try {
    return require('node-gyp-build')(packageDir);
  } catch (error) {
    if (process.platform !== 'win32') {
      throw error;
    }
    try {
      return require(platformPackage);
    } catch (platformError) {
      if (platformError.code === 'MODULE_NOT_FOUND') {
        throw new Error(`${error.message}. On Windows the binary comes from the optional dependency ${platformPackage}, which is not installed`, { cause: error });
      }
      throw platformError;
    }
  }
}

let bindings;

try {
  bindings = load();
} catch (error) {
  throw new Error(`node-gd could not load its native addon: ${error.message}`, { cause: error });
}

export default bindings;
