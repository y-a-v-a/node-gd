const fs = require('fs');
const path = require('path');

let bindings;

let versionMessage = 'Node-gd: method __METHOD__ only available from libgd2 version 2.1.1. '
    + 'Current installed is ';

/**
 * @param {string} format
 */
function openFormatFn(format) {
  return function(path = '') {
    return bindings[`createFrom${format}`].call(this, path);
  };
}

/**
 * @param {string} format
 */
function saveFormatFn(format) {
  format = format.toLowerCase();

  return function() {
    const args = [...arguments];
    const filename = args.shift();

    return new Promise((resolve, reject) => {
      const data = this[`${format}Ptr`].apply(this, args);

      fs.writeFile(filename, data, 'latin1', error => {
        if (error) {
          return reject(error);
        }
        resolve(true);
      });
    });
  };
}

function exportFormats() {
  let valid;
  const version = bindings.getGDVersion();

  const formats = {
    Jpeg: [1, 1],
    Png: [1, 1],
    Gif: [1, 1],
    Gd: [1, 1],
    Gd2: [1, 1],
    Gd2Part: [1, 0],
    WBMP: [1, 1]
  };

  if (version >= '2.1.0') {
    formats.Bmp = [1, 1];
  }

  if (bindings.GD_TIFF && version >= '2.1.0') {
    formats.Tiff = [1, 1];
  }

  if (bindings.GD_WEBP && version >= '2.1.0') {
    formats.Webp = [1, 1];
  }

  for (const format in formats) {
    valid = formats[format];
    if (!!valid[0]) {
      bindings[`open${format}`] = openFormatFn(format);
    }
    if (!!valid[1]) {
      bindings.Image.prototype[`save${format}`] = saveFormatFn(format);
    }
  }
}

const libPaths = [
  path.normalize(`${__dirname}/../build/Release/node_gd.node`),
  path.normalize(`${__dirname}/../build/default/node_gd.node`)
];

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

versionMessage += bindings.getGDVersion();

exportFormats();

if (bindings.getGDVersion() >= '2.1.1') {
  /**
   * Wrapper around gdImageCreateFromFile
   * With safety check for file existence to mitigate
   * uninformative segmentation faults from libgd
   *
   * @param {string} file  Path of file to open
   * @returns Promise<gd.Image>
   */
  bindings.openFile = function openFile(file) {
    return new Promise((resolve, reject) => {
      const filePath = path.normalize(file);

      fs.access(filePath, fs.constants.F_OK, (error) => {
        if (error) {
          return reject(error);
        }

        resolve(this.createFromFile(filePath));
      });
    });
  };
} else {
  bindings.Image.prototype.file = function() {
    throw new Error(versionMessage.replace('__METHOD__', 'gd.Image#file()'));
  };

  bindings.openFile = function openFile() {
    throw new Error(versionMessage.replace('__METHOD__', 'gd.openFile()'));
  };
}

module.exports = bindings;
