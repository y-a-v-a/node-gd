[![node-gd logo](https://raw.githubusercontent.com/y-a-v-a/node-gd-artwork/master/node-gd-mini.png)](https://github.com/y-a-v-a/node-gd)

# node-gd

GD graphics library, [libgd](http://www.libgd.org/), C++ bindings for Node.js. This version is the community-maintained [official NodeJS node-gd repo](https://npmjs.org/package/node-gd). With `node-gd` you can easily create, manipulate, open and save paletted and true color images from and to a variety of image formats including JPEG, PNG, GIF and BMP.

## Installation

### Quick Installation (Recommended)

For most users, installation is now much simpler thanks to prebuilt binaries:

```bash
$ npm install node-gd
```

This will automatically download and install a precompiled binary for your platform (macOS x64/arm64, Linux x64/arm64) and Node.js version. No build tools or system dependencies required!

### Manual Installation

If prebuilt binaries are not available for your platform or you want to build from source:

#### Preconditions

Have environment-specific build tools available. Next to that: to take full advantage of node-gd, best is to ensure you install the latest version of libgd2, which can be found at the [libgd github repository](https://github.com/libgd/libgd/releases).

#### On Debian/Ubuntu

```bash
$ sudo apt-get install libgd-dev # libgd
$ npm install node-gd
```

#### On RHEL/CentOS

```bash
$ sudo yum install gd-devel
$ npm install node-gd
```

#### On Mac OS/X

Using Homebrew

```bash
$ brew install pkg-config gd
$ npm install node-gd
```

...or using MacPorts

```bash
$ sudo port install pkgconfig gd2
$ npm install node-gd
```

### Platform Support

- **✅ macOS** - x64 and arm64 (Apple Silicon) - prebuilt binaries available
- **✅ Linux** - x64 and arm64 - prebuilt binaries available  
- **✅ FreeBSD** - manual build only
- **❌ Windows** - Not supported

### Build from Source

If you need to build from source or want to customize the build:

```bash
$ git clone https://github.com/y-a-v-a/node-gd.git
$ cd node-gd
$ npm install # This will fallback to building from source if no prebuilt binary
```

### Supported Node.js Versions

Prebuilt binaries are available for Node.js versions 16, 18, 20, and 22. The package supports Node.js 14+ but prebuilt binaries are only provided for actively maintained versions.

## Usage

There are different flavours of images, of which the main ones are palette-based (up to 256 colors) and true color images (millions of colors). GIFs are always palette-based, PNGs can be both palette-based or true color. JPEGs are always true color images. `gd.create()` will create a palette-based base image while `gd.createTrueColor()` will create a true color image.

### API

Full API documentation and more examples can be found in the [docs](https://github.com/y-a-v-a/node-gd/blob/master/docs/index.md) directory or at [the dedicated github page](https://y-a-v-a.github.io/node-gd/).

### Examples

Example of creating a rectangular image with a bright green background and in magenta the text "Hello world!"

```javascript
// Import library
import gd from 'node-gd';

// Create blank new image in memory
const img = await gd.create(200, 80);

// Set background color
img.colorAllocate(0, 255, 0);

// Set text color
const txtColor = img.colorAllocate(255, 0, 255);

// Set full path to font file
const fontPath = '/full/path/to/font.ttf';

// Render string in image
img.stringFT(txtColor, fontPath, 24, 0, 10, 60, 'Hello world!');

// Write image buffer to disk
await img.savePng('output.png', 1);

// Destroy image to clean memory
img.destroy();
```

Example of drawing a red lined hexagon on a black background:

```javascript
import gd from 'node-gd';

const img = await gd.createTrueColor(200, 200);

const points = [
  { x: 100, y: 20 },
  { x: 170, y: 60 },
  { x: 170, y: 140 },
  { x: 100, y: 180 },
  { x: 30, y: 140 },
  { x: 30, y: 60 },
  { x: 100, y: 20 },
];

img.setThickness(4);
img.polygon(points, 0xff0000);
await img.saveBmp('test1.bmp', 0);
img.destroy();
```

Another example:

```javascript
import gd from 'node-gd';

const img = await gd.openFile('/path/to/file.jpg');

img.emboss();
img.brightness(75);
await img.file('/path/to/newFile.bmp');
img.destroy();
```

Some output functions are synchronous because they are handled by libgd. An example of this is the creation of animated GIFs.

## License & copyright

Since [December 27th 2012](https://github.com/andris9/node-gd/commit/ad2a80897efc1926ca505b511ffdf0cc1236135a), node-gd is licensed under an MIT license.

The MIT License (MIT)
Copyright (c) 2010-2020 the contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contributors

The current version is based on code created by taggon, [here the original author's repo](https://github.com/taggon/node-gd), and on the additions by [mikesmullin](https://github.com/mikesmullin). Porting node-gd to [node-addon-api](https://github.com/nodejs/node-addon-api) and extending the API is done by [y-a-v-a](https://github.com/y-a-v-a), on Twitter as [@\_y_a_v_a\_](https://twitter.com/_y_a_v_a_). See the `CONTRIBUTORS.md` [file](https://github.com/y-a-v-a/node-gd/blob/master/CONTRIBUTORS.md) for a list of all contributors.
