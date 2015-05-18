# node-gd


[![Build Status](https://api.travis-ci.org/y-a-v-a/node-gd.svg?branch=stable)](http://travis-ci.org/y-a-v-a/node-gd)

GD graphic library (libgd) C++ bindings for Node.js.

This version is the community-maintained [official NodeJS.org node-gd repo](https://npmjs.org/package/node-gd). Be aware that since node-gd version 0.3.x only libgd2 version 2.1.x is fully supported. Run `gdlib-config --version` to check the version of libgd2 on your system.

[![NPM](https://nodei.co/npm/node-gd.png?downloads=true&downloadRank=true&stars=true)](https://nodei.co/npm/node-gd/) [![NPM](https://nodei.co/npm-dl/node-gd.png?months=6&height=3)](https://nodei.co/npm/node-gd/)

## Installation on Debian/Ubuntu

```bash
$ sudo apt-get install libgd2-xpm-dev # libgd
$ npm install node-gd
```

## Installation on RHEL/CentOS

```bash
$ sudo yum install gd-devel
$ npm install node-gd
```

## Installation on Mac OS/X

```bash
$ brew install gd
$ npm install node-gd
```

## Installation on Windows 7

Please open an issue if you have the answer. I'm sure it works, I just don't have ready the exact commands.

## Usage

### Using coffeescript

```coffeescript
# Require library
gd = require 'node-gd'

# Create blank new image in memory
output_img = gd.create width, height

# Load existing image file on disk into memory
gd.openPng "test.png", (err, input_img) ->
  console.log "width: ", input_img.width
  console.log "height: ", input_img.width

# Render input over the top of output
input_img.copyResampled output_img, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH

# Write image buffer to disk
output_img.savePng "out.png", 0, (err) ->
  console.log "image saved!"
```

### Using javascript

```javascript
// Require library
var gd = require('node-gd');

// Create blank new image in memory
var img = gd.create(200, 80);

// Set background color
img.colorAllocate(0, 255, 0);

// Set text color
var txtColor = img.colorAllocate(255, 0, 255);
// Set full path to font file
var fontPath = '/full/path/to/font.ttf';

// Render string in image
img.stringFT(txtColor, fontPath, 24, 0, 10, 60, 'Hello world!');

// Write image buffer to disk
img.savePng('output.png', 1, function(err) {
  if(err) {
    throw err;
  }
});

// Destroy buffer
img.destroy();

```

As usual, for the latest examples, review the easy-to-follow [./test/test.coffee](https://github.com/mikesmullin/node-gd/blob/master/test/test.coffee).

## libgd2 version information
The function `gdImageGrayScale`, `gdImageEmboss`, etc. are not available in libgd2 version 2.0.x. Therefore, the following script although it works, will output a green image on systems with that version. This means: the function call to `img.toGrayscale()` just doesn't do anything and returns only `this`.

```javascript
var gd = require('../js/node-gd.js');

var img = gd.create(200, 80);

img.colorAllocate(0, 255, 0);

img.toGrayscale();

img.savePng('output.png', 1, function(err) {
  if(err) {
    throw err;
  }
  img.destroy();
});
```

## Test
```bash
$ npm test
```
The `test/output` directory contains the resulting images of the test script. The tests use, in some cases, a GNU Freefont font, which is licensed under the GNU General Public License v3.


## Related

* [Original author's repo](https://github.com/taggon/node-gd)
* [node-canvas](https://github.com/LearnBoost/node-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js

