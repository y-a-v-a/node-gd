# node-gd

GD graphic library (libgd) C++ bindings for Node.js.

This version is the community-maintained [official NodeJS.org node-gd repo](https://npmjs.org/package/node-gd).

## Installation on Debian/Ubuntu

```bash
sudo apt-get install libgd2-xpm-dev # libgd
npm install node-gd
```

## Installation on Mac OS/X

    please open an issue if you have the answer. i'm sure it works, i just don't have ready the exact commands.

see also: https://github.com/taggon/node-gd/issues/6

## Installation on Windows 7

    please open an issue if you have the answer. i'm sure it works, i just don't have ready the exact commands.

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

## Related

* [Original author's repo](https://github.com/taggon/node-gd)
* [node-canvas](https://github.com/LearnBoost/node-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js
* [node-o3-canvas](https://github.com/ajaxorg/node-o3-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js
* [node-image](https://github.com/pkrumins/node-image)
