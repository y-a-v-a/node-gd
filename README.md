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

## API
###Opening and creating graphic images
####gd.create(width, height)
Create a new image in memory of specified size and return an instance of `gd.Image`. This is a paletted image, thus has a white background and a limited color range of at most 256 colors.
####gd.createTrueColor(width, height)
Returns an object of type `gd.Image` . This is a true color image, and thus has a black background and a palette of millions of colors.
####gd.openJpeg(path[, callback])
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
####gd.createFromJpeg(path)
Synchronous version of `gd.openJpeg()`.
####gd.createFromJpegPtr(data)
Load an image from a resource which is either a `String` or a `Buffer`.
####gd.openPng(path[, callback])
Open a PNG file. The callback, if supplied, gets `error` and `image` as parameter. The image is an instance of `gd.Image`.
####gd.createFromPng(path)
Synchronous version of `gd.openPng()`.
####gd.createFromPngPtr(data)
Load an image from a resource.
####gd.openGif(path[, callback])
Open a GIF file asynchronously.
####gd.createFromGif(path)
Synchronous version of `gd.openGif()`.
####gd.createFromGifPtr(data)
####gd.openGd2(path[, callback])
####gd.createFromGd2(path)
####gd.createFromGd2Ptr(data)
####gd.openGd2Part(path, srcX, srcY, width, height[, callback])
####gd.createFromGd2Part(path, srcX, srcY, width, height)
####gd.createFromGd2PartPtr(data, srcX, srcY, width, height)
####gd.openWBMP(path[, callback])
Open a WBMP image file. WBMP stands for Wireless Application Protocol Bitmap Format, a monochrome graphics file format.
####gd.createFromWBMP(path[, callback])
####gd.createFromWBMPPtr(data)
####gd.openBmp(path[, callback])
Open a BMP bitmap image file.
####gd.createFromBmp(path)
####gd.createFromBmpPtr(data)
####gd.openFile(path[, callback])
GD will try to find out of what type the supplied image is, and will open it likewise. This is a quite new convenience method for if you don't exactly know what the type is that you will try to open.
####gd.createFromFile(path)
####gd.trueColor(red, green, blue)
Returns an integer representation of the supplied color values.
```javascript
var gd = require('node-gd');
var red = gd.trueColor(255,0,0); // 16711680
```
Alternatively, you can use the hex notation for color values for parameters for GD likewise:
```javascript
var red = 0xff0000; // red
```
####gd.trueColorAlpha(red, green, blue, alpha)
Returns an integer representation of the supplied `rgba` values.
```javascript
var gd = require('node-gd');
var transparentRed = gd.trueColorAlpha(255,0,0,127); // 2147418112
```
####gd.getGDVersion()
Will return a string representing the version of your currently installed GD version. Outputs something like `2.1.1`

### Manipulating graphic images
####gd.Image#destroy()
Free up allocated memory for image data.
### Saving graphic images
The functions `gd.Image#savePng`, `gd.Image#saveJpeg`, `gd.Image#saveGif`, etc. are convenience functions which will be processed asynchronously when a callback is supplied. All of the following have a counterpart like `gd.Image#png` and `gd.Image#pngPtr` which write to disk synchronously or store the image data in a memory pointer respectively. `gd.Image#jpeg` will return the instance of `gd.Image`, `gd.Image#jpgPtr` will return the newly created image data.

```javascript
var gd = require('node-gd');
gd.openPng('/path/to/input.png', function(err, img) {
  if (err) {
    throw err;
  }
  // create jpg pointer from png
  var jpgImageData = img.jpegPtr(0); // jpeg quality 0
  // create gif pointer from png
  var gifImageData = img.gifPtr();

  // create instance of gd.Image() for jpg file
  var jpgImage = gd.createFromJpegPtr(jpgImageData);
  jpgImage.saveFile('./test01.jpg', function(err) {
    if (err) throw err;
  });
  // create instance of gd.Image() for gif file
  var gifImage = gd.createFromGifPtr(gifImageData);
  gifImage.saveFile('./test01.gif', function(err) {
    if (err) throw err;
  });

 img.destroy();
});
```
The above example shows how to create a JPEG and GIF file from a PNG file.

####gd.Image#savePng(path, level[, callback])
Save image data as a PNG file. The callback will receive an error object as a parameter, only if an error occurred. When a callback is supplied, the image will be written asynchronously by `fs.writeFile()`, using `gd.Image#pngPtr()` to first write it to memory in the given format.
####gd.Image#saveJpeg(path, quality[, callback])
Save image data as a JPEG file. The callback will receive an error object as a parameter. Quality can be a `Number` between `0` and `100`.
####gd.Image#saveGif(path[, callback])
Save image data as a GIF file.
####gd.Image#saveGd(path[, callback])
Save image data as a GD file.
####gd.Image#saveGd2(path, chunkSize[, format][, callback])
Save image data as a GD2 file.
####gd.Image#saveWBMP(path, foreground[, callback])
Save image as a 2 color WBMP file. `foreground` is an integer value or `0x000000` value which defines the dark color of the image. All other colors will be changed into white.
####gd.Image#saveBmp(path, compression[, callback])
Only available from GD version 2.1.1. The compression parameter is eiterh `0` for no compression and `1` for compression. This value only affects paletted images.
####gd.Image#saveTiff(path[, callback])
Currently, opening TIFF files with GD has some errors, but saving image data as TIFF works fine. Therefore, there is no `gd.openTiff()` at this moment. Only available from GD version 2.1.1.
####gd.Image#saveFile(path[, callback])
Lets GD decide in which format the image should be stored to disk, based on the supplied file name extension. Only available from GD version 2.1.1.

### Image properties
Any instance of `gd.Image()` has a basic set of instance properties accessible as readonly values.
```javascript
var gd = require('node-gd');
var img = gd.creatTrueColor(100,100);
console.log(img);
```
Will yield to something like:
```javascript
{ colorsTotal: 0,
  interlace: true,
  height: 100,
  width: 100,
  trueColor: 1 }
```
####gd.Image#colorsTotal
For paletted images, returns the amount of colors in the pallete.
####gd.Image#interlace
`Boolean` value for if the image is interlaced or not. This property can also be set. When set to `true` for Jpeg images, GD will save it as a progressive Jpeg image.
```javascript
var gd = require('node-gd');
var img = gd.createTrueColor(100,100);
img.interlace = true; // set interlace to true
// will save jpeg as progressive jpeg image.
img.saveJpeg('/path/to/output.jpg', 100, function(err) {
  if (err) throw err;
});
```
####gd.Image#height
Returns the height of the image as `Number`.
####gd.Image#width
Returns the width of the image as `Number`.
####gd.Image#trueColor
Returns nonzero if the image is a truecolor image, zero for a palette image.

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

// as of node-gd 0.4.x
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

