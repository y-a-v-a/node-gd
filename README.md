# node-gd

GD graphic library (libgd) C++ bindings for Node.js.

This version is the community-maintained [official NodeJS.org node-gd repo](https://npmjs.org/package/node-gd). Be aware that since node-gd version 0.3.x only libgd2 version 2.1.x is supported. Run `gdlib-config --version` to check the version of libgd2 on your system.

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

Please open an issue if you have the answer. i'm sure it works, i just don't have ready the exact commands.

## Usage

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

As usual, for the latest examples, review the easy-to-follow [./test/test.coffee](https://github.com/mikesmullin/node-gd/blob/master/test/test.coffee).

## Test
```bash
$ npm test
```
The `test/output` directory contains the resulting images of the test script. The tests use, in some cases, a GNU Freefont font, which is licensed under the GNU General Public License v3.

## Travis CI

I'm trying to get it built in Travis, but since we're dependent on libgd v2.1.x and Travis CI runs on Ubuntu 12, which has libgd2 v2.0.x, we'll have to wait until they've updated to Ubuntu 14.

## Related

* [Original author's repo](https://github.com/taggon/node-gd)
* [node-canvas](https://github.com/LearnBoost/node-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js
* [node-o3-canvas](https://github.com/ajaxorg/node-o3-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js
* [node-image](https://github.com/pkrumins/node-image)
