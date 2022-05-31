/**
 * Set up module
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

const gd = require('./lib/node-gd');
const GifAnim = require('./lib/GifAnim');

gd.GifAnim = GifAnim;

module.exports = gd;
