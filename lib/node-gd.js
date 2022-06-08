/**
 * Extend C++ bindings with some convenient sugar
 * Copyright 2014-2021 Vincent Bruijn <vebruijn@gmail.com>
 */

import fs from 'fs';
import path from 'path';
import gd from './bindings.js';

const version = gd.getGDVersion();

const formats = ['Jpeg', 'Png', 'Gif', 'WBMP', 'Bmp'];

if (version >= '2.3.2') {
  if (gd.GD_HEIF) {
    formats.push('Heif');
  }
  if (gd.GD_AVIF) {
    formats.push('Avif');
  }
}

if (gd.GD_TIFF) {
  formats.push('Tiff');
}

if (gd.GD_WEBP) {
  formats.push('Webp');
}

/**
 * Create convenience functions for opening
 * specific image formats
 *
 * @param {string} format
 * @returns {function}     The function that will be called
 *                         when gd.openJpeg is called
 */
function openFormatFn(format) {
  return function (path = '') {
    return gd[`createFrom${format}`].call(gd, path);
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
  return {
    [`save${format}`]() {
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
    },
  }[`save${format}`];
}

/**
 * Add convenience functions to gd
 */
formats.forEach(format => {
  gd[`open${format}`] = openFormatFn(format);

  Object.defineProperty(gd.Image.prototype, `save${format}`, {
    value: saveFormatFn(format),
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

    fs.access(filePath, fs.constants.F_OK, error => {
      if (error) {
        return reject(error);
      }

      resolve(gd.createFromFile(filePath));
    });
  });
}

gd.openFile = openFile;

gd.toString = function toString() {
  return '[object Gd]';
};

gd.Image.prototype.toString = function toString() {
  return '[object Image]';
};

export default gd;
