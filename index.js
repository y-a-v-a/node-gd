/**
 * Set up module
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

import bindings from './lib/node-gd.js';
import GifAnim from './lib/GifAnim.js';

bindings.GifAnim = GifAnim;

export default bindings;
