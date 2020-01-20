'use strict';

/**
 * Extend C++ bindings with some convenient sugar
 * Copyright 2015-2020 Vincent Bruijn <vebruijn@gmail.com>
 */

const fs = require('fs');
const path = require('path');
const bindings = require('./bindings');

const version = bindings.getGDVersion();
const formats = [
  "Jpeg",
  "Png",
  "Gif",
  "Gd",
  "Gd2",
  "Gd2Part",
  "WBMP"
];

if (version >= '2.1.0') {
  formats.push('Bmp');

  if (bindings.GD_TIFF) {
    formats.push('Tiff');
  }

  if (bindings.GD_WEBP) {
    formats.push('Webp');
  }
}

const versionMessage =
  'Node-gd: method __METHOD__ only available '
    + 'from libgd2 version 2.1.1. '
    + `Current installed is ${version}`;

/**
 * Create convenience functions for opening
 * specific image formats
 *
 * @param {string} format
 * @returns {function}     The function that will be called
 *                         when bindings.openJpeg is called
 */
function openFormatFn(format) {
  return function(path = '') {
    return bindings[`createFrom${format}`].call(bindings, path);
  };
}

/**
 * Create convience functions for saving
 * specific image formats
 *
 * @param {string} format
 * @returns {function}
 */
function saveFormatFn(format) {
  return {[`save${format}`]() {
    const args = [...arguments];
    const filename = args.shift();

    return new Promise((resolve, reject) => {
      const data = this[`${format.toLowerCase()}Ptr`].apply(this, args);

      fs.writeFile(filename, data, 'latin1', error => {
        if (error) {
          return reject(error);
        }
        resolve(true);
      });
    });
  }}[`save${format}`];
}

/**
 * Add convenience functions to bindings
 */
formats.forEach(format => {
  bindings[`open${format}`] = openFormatFn(format);
  if (format === 'Gd2Part') {
    return;
  }

  Object.defineProperty(bindings.Image.prototype, `save${format}`, {
    value: saveFormatFn(format)
  });
});

/**
 * Wrapper around gdImageCreateFromFile
 * With safety check for file existence to mitigate
 * uninformative segmentation faults from libgd
 *
 * @param {string} file  Path of file to open
 * @returns {Promise<gd.Image>}
 */
function openFile(file) {
  return new Promise((resolve, reject) => {
    const filePath = path.normalize(file);

    fs.access(filePath, fs.constants.F_OK, (error) => {
      if (error) {
        return reject(error);
      }

      resolve(bindings.createFromFile(filePath));
    });
  });
};

if (version >= '2.1.1') {
  bindings.openFile = openFile;
} else {
  bindings.Image.prototype.file = () => {
    throw new Error(versionMessage.replace('__METHOD__', 'gd.Image.prototype.file()'));
  };

  bindings.openFile = () => {
    throw new Error(versionMessage.replace('__METHOD__', 'gd.openFile()'));
  };
}

module.exports = bindings;
