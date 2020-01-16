'use strict';

/**
 * Set up module
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

const bindings = require('./js/node-gd');
const GifAnim = require('./js/GifAnim');

bindings.GifAnim = GifAnim;

module.exports = bindings;
