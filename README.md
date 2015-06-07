# node-gd


[![Build Status](https://api.travis-ci.org/y-a-v-a/node-gd.svg?branch=stable)](http://travis-ci.org/y-a-v-a/node-gd)

GD graphic library ([libgd](http://www.libgd.org/)) C++ bindings for Node.js.

This version is the community-maintained [official NodeJS node-gd repo](https://npmjs.org/package/node-gd). Be aware that since node-gd version 0.3.x libgd2 version 2.1.x is mostly supported. node-gd version 0.2.x is backed at best with libgd2 version 2.0.x. Run `gdlib-config --version` to check the version of libgd2 on your system.

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

Using Homebrew
```bash
$ brew install gd
$ npm install node-gd
```

or using MacPorts
```bash
$ sudo port install gd2
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

```javascript
var gd = require('node-gd');
gd.openFile('/path/to/file.jpg', function(err, img) {
  if (err) {
    throw err;
  }
  img.emboss();
  img.brightness(75);
  img.saveFile('/path/to/newFile.bmp', function(err) {
    if (err) {
      thorw err;
    }
  });
});
```

As usual, for the latest examples, review the easy-to-follow [./test/test.coffee](https://github.com/mikesmullin/node-gd/blob/master/test/test.coffee).

# API
##Opening and creating graphic images
###gd.create(width, height)
Create a new image in memory of specified size and return an instance of `gd.Image`. This is a paletted image, thus has a white background and a limited color range of at most 256 colors.
###gd.createTrueColor(width, height)
Returns an object of type `gd.Image` . This is a true color image, and thus has a black background and a palette of millions of colors.
###gd.openJpeg(path[, callback])
Open a JPG file. If no callback is supplied, it will return an instance of `gd.Image`. In case of a supplied callback, it will receive two parameters: `error` and an instance of `gd.Image`. Uses `fs.readFile()` and `gd.createFromJpegPtr()` to load the image asynchronously.
```javascript
var gd = require('node-gd');
// open Jpeg image
gd.openJpeg('/path/to/file.jpg', function(error, image) {
  if (error) {
    throw error;
  }
  // save it as a PNG image
  image.savePng('/path/to/file.png', 0, function(error) {
    if (error) {
      throw error;
    }
  });
});
```
###gd.createFromJpeg(path)
Synchronous version of `gd.openJpeg()`.
###gd.createFromJpegPtr(data)
Load an image from a resource which is either a `String` or a `Buffer`.
###gd.openPng(path[, callback])
Open a PNG file. The callback, if supplied, gets `error` and `image` as parameter. The image is an instance of `gd.Image`.
###gd.createFromPng(path)
Synchronous version of `gd.openPng()`.
###gd.createFromPngPtr(data)
Load an image from a resource.
###gd.openGif(path[, callback])
Open a GIF file asynchronously.
###gd.createFromGif(path)
Synchronous version of `gd.openGif()`.
###gd.createFromGifPtr(data)
###gd.openGd2(path[, callback])
###gd.createFromGd2(path)
###gd.createFromGd2Ptr(data)
###gd.openGd2Part(path, srcX, srcY, width, height[, callback])
###gd.createFromGd2Part(path, srcX, srcY, width, height)
###gd.createFromGd2PartPtr(data, srcX, srcY, width, height)
###gd.openWBMP(path[, callback])
Open a WBMP image file. WBMP stands for Wireless Application Protocol Bitmap Format, a monochrome graphics file format.
###gd.createFromWBMP(path[, callback])
###gd.createFromWBMPPtr(data)
###gd.openBmp(path[, callback])
Open a BMP bitmap image file.
###gd.createFromBmp(path)
###gd.createFromBmpPtr(data)
###gd.openFile(path[, callback])
GD will try to find out of what type the supplied image is, and will open it likewise. This is a quite new convenience method for if you don't exactly know what the type is that you will try to open.
###gd.createFromFile(path)
###gd.trueColor(red, green, blue)
Returns an integer representation of the supplied color values.
```javascript
var gd = require('node-gd');
var red = gd.trueColor(255,0,0); // 16711680
```
Alternatively, you can use the hex notation for color values for parameters for GD likewise:
```javascript
var red = 0xff0000; // red
```
###gd.trueColorAlpha(red, green, blue, alpha)
Returns an integer representation of the supplied `rgba` values.
```javascript
var gd = require('node-gd');
var transparentRed = gd.trueColorAlpha(255,0,0,127); // 2147418112
```
###gd.getGDVersion()
Will return a string representing the version of your currently installed GD version. Outputs something like `2.1.1`

## Manipulating graphic images

## Saving graphic images
###gd.Image#savePng(path, callback)
Save image data as a PNG file. The callback will receive an error object as a parameter, only if an error occurred.


## libgd2 version information
node-gd should build successfully for both libgd2 version 2.0.x as wel as for 2.1.x. The main difference is that some functions will not be available. These include:

* `gdImageGrayScale`
* `gdImageEmboss`
* `gdImageGaussianBlur`
* `gdImageNegate`
* `gdImageBrightness`
* `gdImageContrast`
* `gdImageSelectiveBlur`
* `gdImageCreateFromBmp`
* `gdImageCreateFromBmpPtr`
* `gdImageCreateFromFile`
* `gdImageBmp`
* `gdImageBmpPtr`
* `gdImageTiff`
* `gdImageTiffPtr`
* `gdImageFile`

In node-gd, these are their equivalents
* `toGrayscale()`
* `emboss()`
* `gaussianBlur()`
* `negate()`
* `brightness()`
* `contrast()`
* `selectiveBlur()`
* `openBmp()`
* `openFile()`
* `createFromBmp()`
* `createFromBmpPtr()`
* `createFromFile()`
* `saveBmp()`
* `saveTiff()`
* `saveImage()`

```javascript
var gd = require('../js/node-gd.js');

// as of soon:
var version = gd.getGDVersion();
console.log(version); // 2.0.1 or 2.1.1 or the like
```

There are different flavours of images, of which the main ones are palette-based (up to 256 colors) and true color images (millions of colors). GIFs are always palette-based, PNGs can be both palette-based or true color. JPEGs are always true color images. `gd.create()` will create a palette-based base image while `gd.createTrueColor()` will create a true color image.

## Test
```bash
$ npm test
```
The `test/output` directory contains the resulting images of the test script. The tests use, in some cases, a GNU Freefont font, which is licensed under the GNU General Public License v3.


## Related

* [Original author's repo](https://github.com/taggon/node-gd)
* [node-canvas](https://github.com/LearnBoost/node-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js

