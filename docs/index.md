---
layout: default
title: Node-gd Documentation
---

# API

## Version information

This is the documentation for the 2.x version. The 2.x version dropped support for `gd.createSync` and `gd.createTrueColorSync`. Also, all functions that used to accept a callback are changed. They return a Promise now. Effort is put into respecting Node's asynchronous nature by implementing N-API's AsyncWorker classes for all gd functions that utilize file I/O.

# Creating and opening graphic images

### gd.create(width, height)

#### Parameters

- `width`
  - desired width of the image to create, expects a `Number` without a fraction, e.g. `200`.
- `height`
  - desired height of the image, expects a `Number` without a fraction.

#### Return value

- `Promise`
  - The resolved `Promise` contains an instance of `gd.Image`

Create a new image in memory of specified size and returns a `Promise` for an instance of `gd.Image`. This is a paletted image, thus has a white background and a limited color range of at most 256 colors.

```javascript
const gd = require('node-gd');
const img = await gd.create(100, 100);

await img.saveJpeg('./test.jpg', 100);
img.destroy();
```

### gd.createTrueColor(width, height)

#### Parameters

- `width`
  - desired width of the image to create. Expects a `Number` without fraction, e.g. `Math.round(100 / 6)`
- `height`
  - desired height of the image

#### Return value

- `Promise`
  - The resolved `Promise` contains an instance of `gd.Image`

This is a true color image, and thus has a black background and a palette of millions of colors.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(100, 100);

await img.saveJpeg('./test.jpg', 100);
img.destroy();
```

### gd.createSync(width, height)

Synchronous version of `gd.create()`.

### gd.createTrueColorSync(width, height)

Synchronous version of `gd.createTrueColor()`.

### gd.openJpeg(path)

#### Parameters

- `path`
  - A string referencing the JPEG image to open. Expects a `String`, e.g. `/path/to/image.jpg`

#### Return type

- `Promise`
  - The resolved `Promise` contains an instance of `gd.Image`

Open a JPG file. It will return a Promise which will resolve with an instance of `gd.Image`. This is a short hand of `gd.createFromJpeg`.

```javascript
const gd = require('node-gd');

// open Jpeg image
const image = await gd.openJpeg('/path/to/file.jpg');

// save it as a PNG image
await image.savePng('/path/to/file.png', 0);
img.destroy();
```

### gd.createFromJpeg(path)

#### Parameters

- `path`
  - A string referencing the JPEG image to open, e.g. `/path/to/image.jpg`

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Asynchronously load an image from disk.

```javascript
const gd = require('node-gd');
var img = await gd.createFromJpeg('./path/to/image.jpg');

// do something with img

img.destroy();
```

### gd.createFromJpegPtr(data)

#### Parameters

- `data` - Buffer containing JPEG image data

#### Return value

- `Promise<gd.Image>` - Promise that resolves to an image instance

Load an image from a resource which is an instance of `Buffer`.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const jpegData = fs.readFileSync('./input.jpg');
const img = await gd.createFromJpegPtr(jpegData);

await img.savePng('./output.png', 0);
img.destroy();
```

### gd.openPng(path)

Open a PNG file. Returns a Promise.

### gd.createFromPng(path)

Asynchronously load a Png from disk. Returns a Promise.

### gd.createFromPngPtr(data)

#### Parameters

- `data` - Buffer containing PNG image data

#### Return value

- `Promise<gd.Image>` - Promise that resolves to an image instance

Load an image from a resource.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const pngData = fs.readFileSync('./input.png');
const img = await gd.createFromPngPtr(pngData);

await img.saveJpeg('./output.jpg', 90);
img.destroy();
```

### gd.openGif(path)

Open a GIF file asynchronously. Returns a Promise.

### gd.createFromGif(path)

Asynchronously load a Gif from disk. Returns a Promise.

### gd.createFromGifPtr(data)

#### Parameters

- `data` - Buffer containing GIF image data

#### Return value

- `Promise<gd.Image>` - Promise that resolves to an image instance

Load a GIF image from a Buffer resource.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const gifData = fs.readFileSync('./input.gif');
const img = await gd.createFromGifPtr(gifData);

await img.savePng('./output.png', 0);
img.destroy();
```

### gd.openWBMP(path)

Open a WBMP image file. [WBMP](https://en.wikipedia.org/wiki/Wireless_Application_Protocol_Bitmap_Format) stands for Wireless Application Protocol Bitmap Format, a monochrome graphics file format. Returns a Promise.

### gd.createFromWBMP(path)

Asynchronously load a WBMP from disk. Returns a Promise.

### gd.createFromWBMPPtr(data)

#### Parameters

- `data` - Buffer containing WBMP image data

#### Return value

- `Promise<gd.Image>` - Promise that resolves to an image instance

Load a WBMP image from a Buffer resource.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const wbmpData = fs.readFileSync('./input.wbmp');
const img = await gd.createFromWBMPPtr(wbmpData);

await img.savePng('./output.png', 0);
img.destroy();
```

### gd.openBmp(path)

Open a BMP bitmap image file. Returns a Promise.

### gd.createFromBmp(path)

Asynchronously load a Bmp from disk. Returns a Promise.

### gd.createFromBmpPtr(data)

#### Parameters

- `data` - Buffer containing BMP image data

#### Return value

- `Promise<gd.Image>` - Promise that resolves to an image instance

Load a BMP image from a Buffer resource.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const bmpData = fs.readFileSync('./input.bmp');
const img = await gd.createFromBmpPtr(bmpData);

await img.saveJpeg('./output.jpg', 85);
img.destroy();
```

### gd.openTiff(path)

Returns a Promise.

### gd.createFromTiff(path)

Asynchronously load a Tiff from disk. Returns a Promise.

### gd.createFromTiffPtr(data)

#### Parameters

- `data` - Buffer containing TIFF image data

#### Return value

- `Promise<gd.Image>` - Promise that resolves to an image instance

Load a TIFF image from a Buffer resource.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const tiffData = fs.readFileSync('./input.tiff');
const img = await gd.createFromTiffPtr(tiffData);

await img.saveJpeg('./output.jpg', 90);
img.destroy();
```

### gd.openWebp(path)

#### Parameters

- `path`
  - A string referencing the WebP image to open, e.g. `/path/to/image.webp`

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Open a WebP file. WebP is a modern image format that provides superior lossless and lossy compression for images on the web. Returns a Promise.

```javascript
const gd = require('node-gd');

const img = await gd.openWebp('/path/to/image.webp');
// do something with img
await img.saveJpeg('/path/to/output.jpg', 85);
img.destroy();
```

### gd.createFromWebp(path)

#### Parameters

- `path`
  - A string referencing the WebP image to open

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Asynchronously load a WebP from disk. Returns a Promise.

```javascript
const gd = require('node-gd');

const img = await gd.createFromWebp('./input.webp');
// Apply some effects
img.gaussianBlur();
await img.savePng('./output.png', 6);
img.destroy();
```

### gd.createFromWebpPtr(data)

Load a WebP image from a Buffer resource.

```javascript
const gd = require('node-gd');
const fs = require('fs');

const webpData = fs.readFileSync('./input.webp');
const img = await gd.createFromWebpPtr(webpData);
await img.saveJpeg('./output.jpg', 90);
img.destroy();
```

### gd.openHeif(path)

#### Parameters

- `path`
  - A string referencing the HEIF image to open, e.g. `/path/to/image.heif`

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Open a HEIF file. HEIF (High Efficiency Image Format) is a modern image format that provides better compression than JPEG while maintaining high quality. Returns a Promise.

```javascript
const gd = require('node-gd');

const img = await gd.openHeif('/path/to/image.heif');
// Convert HEIF to PNG
await img.savePng('/path/to/output.png', 6);
img.destroy();
```

### gd.createFromHeif(path)

#### Parameters

- `path`
  - A string referencing the HEIF image to open

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Asynchronously load a HEIF from disk. Returns a Promise.

```javascript
const gd = require('node-gd');

const img = await gd.createFromHeif('./input.heif');
// Resize and save as JPEG
const resized = await gd.createTrueColor(200, 200);
img.copyResampled(resized, 0, 0, 0, 0, 200, 200, img.width, img.height);
await resized.saveJpeg('./thumbnail.jpg', 85);
img.destroy();
resized.destroy();
```

### gd.createFromHeifPtr(data)

Load a HEIF image from a Buffer resource.

### gd.openAvif(path)

#### Parameters

- `path`
  - A string referencing the AVIF image to open, e.g. `/path/to/image.avif`

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Open an AVIF file. AVIF (AV1 Image File Format) is a modern image format based on the AV1 video codec, providing excellent compression efficiency. Returns a Promise.

```javascript
const gd = require('node-gd');

const img = await gd.openAvif('/path/to/image.avif');
// Convert AVIF to WebP
await img.saveWebp('/path/to/output.webp', 80);
img.destroy();
```

### gd.createFromAvif(path)

#### Parameters

- `path`
  - A string referencing the AVIF image to open

#### Return value

- `Promise`
  - Promise resolving to an instance of `gd.Image`

Asynchronously load an AVIF from disk. Returns a Promise.

```javascript
const gd = require('node-gd');

const img = await gd.createFromAvif('./input.avif');
// Apply brightness and contrast adjustments
img.brightness(10);
img.contrast(-200);
await img.saveJpeg('./enhanced.jpg', 90);
img.destroy();
```

### gd.createFromAvifPtr(data)

Load an AVIF image from a Buffer resource.

### gd.openFile(path)

#### Parameters

- `path`
  - `String` referencing a file on disk, e.g. `/path/to/file.png`

#### Return value

- `Promise`
  - A `Promise` resolving an instance of `gd.Image`.

GD will try to find out of what type the supplied image is, and will open it likewise. This is a quite new convenience method for if you don't exactly know what the type is that you will try to open.

```javascript
const gd = require('node-gd');

const file = './image-with-some-extension';

const img = await gd.openFile(file);

// do something with img

img.destroy();
```

### gd.createFromFile(path)

Similar to `gd#openFile` but does not check if file exists. This may lead to unclear segmentation fault messages. Returns a Promise.

### gd.trueColor(red, green, blue)

#### Parameters

- `red`
- `green`
- `blue`
  - All expect a fractionless `Number` or hexadecimal number

#### Return value

- `Number`,
  - an integer representation of the supplied color values.

```javascript
const gd = require('node-gd');

var red = gd.trueColor(255, 0, 0); // 16711680
```

Alternatively, you can use the hex notation for color values for parameters for GD likewise:

```javascript
var red = 0xff0000; // red
```

### gd.trueColorAlpha(red, green, blue, alpha)

Returns an integer representation of the supplied `rgba` values. Be aware that in libgd the value for `alpha` ranges from `0` to `127`. From its source code:

```c++
[the alpha channel] occupies only the
LOWER 7 BITS of the MSB (most significant
bit). This very small loss of alpha channel
resolution allows gd 2.x to keep backwards
compatibility (...)
```

Example:

```javascript
const gd = require('node-gd');

var transparentRed = gd.trueColorAlpha(255, 0, 0, 63); // 1073676288
```

### gd.getGDVersion()

#### Return value

- `String`
  - Something like `2.3.0`

Will return a string representing the version of your currently installed GD version. Outputs something like `2.3.0`

```javascript
import gd from 'node-gd';

console.log(gd.getGDVersion());
```

# Manipulating graphic images

### gd.Image#destroy()

Free up allocated memory for image data, that is on the GD-level. The instance of gd.Image is not disposed of by this call. This is done by v8's garbage collection. In case you call any gd.Image function _after_ destroying the image, an `Error` is thrown telling you that the image has been destroyed.

```javascript
const image = await gd.create(100, 100);
image.destroy();
image.getPixel(50, 50); // will throw an error
```

### Drawing

### gd.Image#setPixel(x, y, color)

Set the color of a certain pixel. Use for color either `0xff00ff` or `gd.trueColor(255, 0, 255)`.

```javascript
const gd = require('node-gd');

const image = await gd.createTrueColor(101, 101);

image.setPixel(51, 51, 0xff00ff);

await image.savePng('./test.png', 0);
image.destroy();
```

### gd.Image#line(x1, y1, x2, y2, color)

#### Parameters

- `x1`, `y1` - Starting point coordinates
- `x2`, `y2` - Ending point coordinates  
- `color` - Line color

Draw a line from a certain point to a next point.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 200);
const white = gd.trueColor(255, 255, 255);
const red = gd.trueColor(255, 0, 0);

img.fill(0, 0, white);
img.line(10, 10, 190, 190, red);
img.line(10, 190, 190, 10, red);

await img.savePng('./crossed-lines.png', 6);
img.destroy();
```

### gd.Image#dashedLine(x1, y1, x2, y2, color)

#### Parameters

- `x1`, `y1` - Starting point coordinates
- `x2`, `y2` - Ending point coordinates  
- `color` - Line color

Draw a dashed line from a certain point to a next point.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(300, 100);
const white = gd.trueColor(255, 255, 255);
const blue = gd.trueColor(0, 0, 255);

img.fill(0, 0, white);
img.setThickness(3);
img.dashedLine(10, 50, 290, 50, blue);

await img.savePng('./dashed-line.png', 6);
img.destroy();
```

### gd.Image#polygon(array, color)

Draw a closed polygon. The first parameter shoud be an `Array` of `Object`s containing an x and y property.

```javascript
// draw a red triangle on a black background
const gd = require('node-gd');
var img = await gd.createTrueColor(100, 100);

var points = [
  { x: 10, y: 10 },
  { x: 70, y: 50 },
  { x: 30, y: 90 },
];

img.polygon(points, 0xff0000);
img.bmp('test.bmp', 0);
img.destroy();
```

### gd.Image#openPolygon(array, color)

Same as the above but start and end point will not be connected with a line.

### gd.Image#filledPolygon(array, color)

Same as the above but color value will be used to fill the polygon.

### gd.Image#rectangle(x1, y1, x2, y2, color)

Create a rectangle. The x and y values indicate upper left and lower right corner respectively.

```javascript
const gd = require('node-gd');
var img = await gd.create(300, 200);
var color0 = img.colorAllocate(255, 255, 255);
var color1 = img.colorAllocate(255, 0, 255);

img.rectangle(20, 20, 280, 180, color1);
await img.file('./test.png');

img.destroy();
```

### gd.Image#filledRectangle(x1, y1, x2, y2, color)

Create a filled rectangle i.e. a solid bar.

```javascript
const gd = require('node-gd');
var img = await gd.create(300, 200);

var color0 = img.colorAllocate(255, 255, 255);
var color1 = img.colorAllocate(255, 0, 255);

img.filledRectangle(20, 20, 280, 180, color1);

await img.file('./test.png');
img.destroy();
```

### gd.Image#arc(cx, cy, width, height, begin, end, color)

Draw an arc. `cx` and `cy` denote the center of the arc. The `begin` and `end` parameters are in degrees, from 0 to 360.

```javascript
const gd = require('node-gd');
var img = await gd.create(300, 200);
var color0 = img.colorAllocate(255, 255, 255);
var color1 = img.colorAllocate(255, 0, 255);
var color2 = img.colorAllocate(0, 255, 255);

img.arc(150, 100, 200, 160, 0, 270, color1);
img.arc(150, 100, 120, 120, 90, 0, color2);

await img.file('./test.png');
img.destroy();
```

### gd.Image#filledArc(cx, cy, width, height, begin, end, color, style)

Draw a filled arc. The `style` parameter can be an integer generated by a bitwise OR of values 0, 1, 2, 4. These values stand for `arc`, `chord`, `noFill` and `edged`. A combination is possible, but `arc` and `chord` are mutually exclusive (see also the libgd documentation).

```javascript
const image = await gd.create(100, 100);
var c1 = image.colorAllocate(200, 50, 0); // red
var c2 = image.colorAllocate(0, 200, 50); // green
var c3 = image.colorAllocate(0, 50, 200); // blue

image.filledArc(50, 50, 80, 80, 0, 225, c2, 4);
image.arc(46, 46, 80, 80, 0, -90, c3);

await image.saveGif('test.gif');
image.destroy();
```

### gd.Image#ellipse(cx, cy, width, height, color)

#### Parameters

- `cx`, `cy` - Center coordinates of the ellipse
- `width`, `height` - Dimensions of the ellipse
- `color` - Ellipse outline color

Draw an ellipse.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 150);
const white = gd.trueColor(255, 255, 255);
const green = gd.trueColor(0, 255, 0);

img.fill(0, 0, white);
img.setThickness(3);
img.ellipse(100, 75, 160, 100, green);

await img.savePng('./ellipse.png', 6);
img.destroy();
```

### gd.Image#filledEllipse(cx, cy, width, height, color)

#### Parameters

- `cx`, `cy` - Center coordinates of the ellipse
- `width`, `height` - Dimensions of the ellipse
- `color` - Fill color

Draw a filled ellipse.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 150);
const white = gd.trueColor(255, 255, 255);
const purple = gd.trueColor(128, 0, 128);

img.fill(0, 0, white);
img.filledEllipse(100, 75, 160, 100, purple);

await img.savePng('./filled-ellipse.png', 6);
img.destroy();
```

### gd.Image#fillToBorder(x, y, border, color)

Fill a drawn form up until its border.

```javascript
const gd = require('node-gd');
var img = await gd.create(300, 200);
var color0 = img.colorAllocate(255, 255, 255);
var color1 = img.colorAllocate(255, 0, 255);
var color2 = img.colorAllocate(0, 255, 255);

img.ellipse(150, 100, 160, 160, color2);
img.fillToBorder(150, 100, 2, color1);

await img.file('./test.png');
img.destroy();
```

### gd.Image#fill(x, y, color)

Create a white true color image

```javascript
var gd = requide('node-gd');
var img = await gd.createTrueColor(100, 100);
img.fill(0, 0, 0xffffff);
await img.file('./test.jpg');
img.destroy();
```

Create a transparent true color PNG

```javascript
var gd = requide('node-gd');
var img = await gd.createTrueColor(100, 100);
img.fill(0, 0, 0x7fffffff);
img.saveAlpha(1);
await img.file('./result.png');
img.destroy();
```

### gd.Image#setAntiAliased(color)

#### Parameters

- `color` - Color to be used for anti-aliased drawing

Enable anti-aliasing for subsequent drawing operations with the specified color.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 200);
const white = gd.trueColor(255, 255, 255);
const black = gd.trueColor(0, 0, 0);

img.fill(0, 0, white);
img.setAntiAliased(black);

// Draw anti-aliased lines
img.line(10, 10, 190, 100, gd.COLOR_ANTIALIASED);
img.line(10, 190, 190, 100, gd.COLOR_ANTIALIASED);

await img.savePng('./antialiased-lines.png', 6);
img.destroy();
```

### gd.Image#setAntiAliasedDontBlend(color, dontblend)

#### Parameters

- `color` - Color to be used for anti-aliased drawing
- `dontblend` - Boolean flag to disable blending

Enable anti-aliasing with blending control for subsequent drawing operations.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 200);
const white = gd.trueColor(255, 255, 255);
const red = gd.trueColor(255, 0, 0);

img.fill(0, 0, white);
img.setAntiAliasedDontBlend(red, true);

// Draw anti-aliased line without blending
img.line(50, 50, 150, 150, gd.COLOR_ANTIALIASED);

await img.savePng('./antialiased-no-blend.png', 6);
img.destroy();
```

### gd.Image#setBrush(image)

#### Parameters

- `image` - A gd.Image instance to use as a brush

Set a brush image for subsequent drawing operations.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(300, 200);
const white = gd.trueColor(255, 255, 255);
const red = gd.trueColor(255, 0, 0);

// Create a brush - a small filled circle
const brush = await gd.createTrueColor(20, 20);
brush.fill(0, 0, white);
brush.filledEllipse(10, 10, 18, 18, red);

img.fill(0, 0, white);
img.setBrush(brush);

// Draw with the brush
img.line(50, 50, 250, 150, gd.COLOR_BRUSHED);

await img.savePng('./brush-line.png', 6);
img.destroy();
brush.destroy();
```

### gd.Image#setTile(image)

#### Parameters

- `image` - A gd.Image instance to use as a tile pattern

Set a tile pattern for subsequent fill operations.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 200);
const white = gd.trueColor(255, 255, 255);
const blue = gd.trueColor(0, 0, 255);

// Create a tile pattern - a checkerboard
const tile = await gd.createTrueColor(20, 20);
tile.fill(0, 0, white);
tile.filledRectangle(0, 0, 10, 10, blue);
tile.filledRectangle(10, 10, 20, 20, blue);

img.fill(0, 0, white);
img.setTile(tile);

// Fill rectangle with tile pattern
img.filledRectangle(50, 50, 150, 150, gd.COLOR_TILED);

await img.savePng('./tiled-rectangle.png', 6);
img.destroy();
tile.destroy();
```

### gd.Image#setStyle(array)

#### Parameters

- `array` - Array of colors defining the line style pattern

Set a custom line style pattern for subsequent drawing operations.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(300, 100);
const white = gd.trueColor(255, 255, 255);
const red = gd.trueColor(255, 0, 0);
const blue = gd.trueColor(0, 0, 255);

img.fill(0, 0, white);

// Create a custom style: red-blue-red-transparent pattern
const style = [red, blue, red, gd.COLOR_TRANSPARENT];
img.setStyle(style);

img.setThickness(5);
img.line(10, 50, 290, 50, gd.COLOR_STYLED);

await img.savePng('./styled-line.png', 6);
img.destroy();
```

### gd.Image#setThickness(thickness)

Set the thickness for lines.

```javascript
const gd = require('node-gd');
var img = await gd.create(300, 200);
var color0 = img.colorAllocate(255, 255, 255);
var color1 = img.colorAllocate(255, 0, 255);

img.setThickness(30);
img.line(0, 0, 300, 200, color1);
img.line(0, 200, 300, 0, color1);

await img.file('./test.png');
img.destroy();
```

### gd.Image#alphaBlending(blending)

Turn on (`1`) or off (`0`) alphablending for what is being drawn next. This switch can be switched on or off during a drawing process in order to let some layer or drawn form blend or not with former drawn elements.

A newly created truecolor image for example has a black background. If you want to start out with a transparent canvas, you will first need to draw a non-blending fully transparent rectangle the size of that image. For example:

```javascript
const gd = require('node-gd');
var img = gd.createTrueColorSync(100, 100);

// turn off alpha blending
// you don't want your first filled rectangle to blend
// with the basic black background!
img.alphaBlending(0);

// you *do* want to save the full alpha channel
// i.e. I assume you want your image to be really transparent
img.saveAlpha(1);

// allocate any color as long as it's transparent!
// i.e. an alpha value of 127
var color = gd.trueColorAlpha(255, 255, 255, 127);
img.filledRectangle(0, 0, 100, 100, color);

// from here on you can add forms, texts or new images
// turn alpha blending on or off if you want the drawn
// elements to blend with other layers.
// img.alphaBlending(1);

await img.savePng('output.png', 0);
img.destroy();
```

### gd.Image#saveAlpha(saveFlag)

When set to `1`, the alpha information will be saved as a separate channel within the image. If turned off (`0`), all transparency will be handled within the image. Compare these two examples to see the difference:

```javascript
const gd = require('node-gd');
var img = await gd.createTrueColor(100, 100);
// save the full alpha channel
// the image will be transparent to e.g. a browser's background color
img.saveAlpha(1);
img.alphaBlending(0);

var color = gd.trueColorAlpha(255, 255, 255, 127);
img.filledRectangle(0, 0, 100, 100, color);

img.alphaBlending(1);

var red = gd.trueColorAlpha(255, 0, 0, 64);
var blue = gd.trueColorAlpha(0, 0, 255, 64);

img.filledRectangle(0, 0, 80, 80, red);
img.filledRectangle(20, 20, 100, 100, blue);

await img.savePng('output.png', 0);
img.destroy();
```

And with `saveAlpha` turned off

```javascript
const gd = require('node-gd');
var img = await gd.createTrueColor(100, 100);
// the image will have a white background
// but the red and blue squares will blend
// with each other and the white background
img.saveAlpha(0);
img.alphaBlending(0);

var color = gd.trueColor(255, 255, 255);
img.filledRectangle(0, 0, 100, 100, color);

img.alphaBlending(1);

var red = gd.trueColorAlpha(255, 0, 0, 64);
var blue = gd.trueColorAlpha(0, 0, 255, 64);

img.filledRectangle(0, 0, 80, 80, red);
img.filledRectangle(20, 20, 100, 100, blue);

await img.savePng('output2.png', 0);
img.destroy();
```

### gd.Image#setClip(x1, y1, x2, y2)

#### Parameters

- `x1, y1` - Top-left corner coordinates
- `x2, y2` - Bottom-right corner coordinates

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Set the clipping area for drawing operations.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(400, 300);
const red = gd.trueColor(255, 0, 0);

// Set clipping area to center rectangle
img.setClip(100, 75, 300, 225);

// Only draws within clipping area
img.filledRectangle(0, 0, 399, 299, red);

await img.saveJpeg('./clipped-drawing.jpg', 85);
img.destroy();
```

### gd.Image#getClip()

Returns an object containing the coordinates of the clipping area.

```js
{
  x1: 0,
  y1: 0,
  x2: 0,
  y2: 0
}
```

### gd.Image#setResolution(res_x, res_y)

#### Parameters

- `res_x` - Horizontal resolution in DPI
- `res_y` - Vertical resolution in DPI

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Set the resolution of the image. Unsigned integers should be used for `res_x` and `res_y`.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(400, 300);
const blue = gd.trueColor(0, 0, 255);
img.filledRectangle(0, 0, 399, 299, blue);

// Set resolution to 300 DPI
img.setResolution(300, 300);

await img.saveJpeg('./high-resolution.jpg', 95);
img.destroy();
```

### Query image information

### gd.Image#getPixel(x, y)

For paletted images, this will return the palette index of the color for that pixel. For true color images, it will return the color value as `Number`.

```javascript
var gd = requide('node-gd');
var trueColorImage = await gd.openFile('./input.jpg');
console.log(trueColorImage.getPixel(20, 20));
// outputs something like 12205619

var paletteImage = await gd.openFile('./input.gif');
console.log(paletteImage.getPixel(20, 20));
// outputs something like 240

trueColorImage.destroy();
paletteImage.destroy();
```

### gd.Image#getTrueColorPixel(x, y)

This will return the color integer representation.

```javascript
const gd = require('node-gd');
var img = await gd.createTrueColor(300, 200);
var color = img.colorAllocate(255, 0, 255);

img.setThickness(30);
img.line(0, 0, 300, 200, color);
img.line(0, 200, 300, 0, color);

// will output ff00ff
console.log(img.getTrueColorPixel(50, 50).toString(16));

await img.file('./test.png');
img.destroy();
```

### gd.Image#imageColorAt(x, y)

This is the implementation of the PHP-GD specific method [imagecolorat](https://www.php.net/manual/en/function.imagecolorat.php).

### gd.Image#getBoundsSafe(x, y)

Returns `0` if either x or y are out of the bounds of the image canvas, or `1` when within the bounds. Note that the pixel range starts at `0` for both x-axis and y-axis.

# Font and text

### gd.Image#stringFTBBox(color, font, size, angle, x, y, string)

`BBox` in this function name refers to the _bounding box_ of the supplied `string` parameter when it would be rendered on the image. This might be handy in case you want to check wether the text fits the image bounds. The return value of this function is an array of 8 elements, weirdly ordered: [xll, yll, xlr, ylr, xur, yur, xul, yul]. In ascii-art:

```
   xul           xur
yul ._____________. yur
    | Lorem ipsum |
    | dolor sit   |
yll ._____________. ylr
   xll           xlr

legend:
xul = x upper left
ylr = y lower right
```

The font color can be allocated with `img.colorAllocate(r, g, b)`. The `font` parameter should be an absolute path to a `ttf` font file.

### gd.Image#stringFT(color, font, size, angle, x, y, string, boundingbox)

`FT` refers to FreeType. Add text to the image. When the `boundingbox` parameter is set to be `true`, the return of this function is identical to `gd.Image#stringFTBBox`.

### Color handling

### gd.Image#colorAllocate(r, g, b)

Allocate a color in the color table.

### gd.Image#colorAllocateAlpha(r, g, b, a)

Allocate a color in the color table with transparency value.

### gd.Image#colorClosest(r, g, b)

#### Parameters

- `r`, `g`, `b` - RGB color values (0-255)

#### Return value

- `Number` - Color index for palette images, or closest color value for truecolor images

Find the closest color in the image palette to the specified RGB values.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);
const red = img.colorAllocate(255, 0, 0);
const blue = img.colorAllocate(0, 0, 255);

// Find closest color to green (should return red or blue)
const closest = img.colorClosest(0, 255, 0);
console.log('Closest color index:', closest);

img.destroy();
```

### gd.Image#colorClosestAlpha(r, g, b, a)

#### Parameters

- `r`, `g`, `b` - RGB color values (0-255)
- `a` - Alpha value (0-127, where 0 is opaque and 127 is transparent)

#### Return value

- `Number` - Color index for palette images, or closest color value for truecolor images

Find the closest color in the image palette to the specified RGBA values.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);
const transRed = img.colorAllocateAlpha(255, 0, 0, 64);

// Find closest color to semi-transparent blue
const closest = img.colorClosestAlpha(0, 0, 255, 64);
console.log('Closest alpha color index:', closest);

img.destroy();
```

### gd.Image#colorClosestHWB(r, g, b)

#### Parameters

- `r`, `g`, `b` - RGB color values (0-255)

#### Return value

- `Number` - Color index using HWB (Hue, Whiteness, Blackness) color space

Find the closest color using the HWB color space, which can provide better color matching in some cases.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);
const red = img.colorAllocate(255, 0, 0);
const blue = img.colorAllocate(0, 0, 255);

// Find closest color using HWB color space
const closest = img.colorClosestHWB(128, 128, 0);
console.log('Closest HWB color index:', closest);

img.destroy();
```

### gd.Image#colorExact(r, g, b)

#### Parameters

- `r`, `g`, `b` - RGB color values (0-255)

#### Return value

- `Number` - Color index if exact match found, -1 otherwise

Find an exact color match in the image palette.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);
const red = img.colorAllocate(255, 0, 0);

// Check if exact red exists
const exactRed = img.colorExact(255, 0, 0);
console.log('Exact red found:', exactRed !== -1);

// Check if exact green exists
const exactGreen = img.colorExact(0, 255, 0);
console.log('Exact green found:', exactGreen !== -1);

img.destroy();
```

### gd.Image#colorResolve(r, g, b)

#### Parameters

- `r`, `g`, `b` - RGB color values (0-255)

#### Return value

- `Number` - Color index (allocates new color if not found)

Find a color in the palette, or allocate it if it doesn't exist.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);

// This will allocate green if it doesn't exist
const green = img.colorResolve(0, 255, 0);
console.log('Green color index:', green);

// Fill with the resolved color
img.fill(0, 0, green);
await img.savePng('./green-fill.png', 6);
img.destroy();
```

### gd.Image#colorResolveAlpha(r, g, b, a)

#### Parameters

- `r`, `g`, `b` - RGB color values (0-255)
- `a` - Alpha value (0-127, where 0 is opaque and 127 is transparent)

#### Return value

- `Number` - Color index (allocates new color if not found)

Find a color with alpha in the palette, or allocate it if it doesn't exist.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);

// This will allocate semi-transparent blue if it doesn't exist
const transBlue = img.colorResolveAlpha(0, 0, 255, 64);
console.log('Transparent blue color index:', transBlue);

img.destroy();
```

### gd.Image#red(color)

#### Parameters

- `color` - Color value or index

#### Return value

- `Number` - Red component value (0-255)

Gets the red component value of a given color.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(100, 100);
const purple = gd.trueColor(128, 0, 128);

const redComponent = img.red(purple);
console.log('Red component:', redComponent); // 128

img.destroy();
```

### gd.Image#green(color)

#### Parameters

- `color` - Color value or index

#### Return value

- `Number` - Green component value (0-255)

Gets the green component value of a given color.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(100, 100);
const yellow = gd.trueColor(255, 255, 0);

const greenComponent = img.green(yellow);
console.log('Green component:', greenComponent); // 255

img.destroy();
```

### gd.Image#blue(color)

#### Parameters

- `color` - Color value or index

#### Return value

- `Number` - Blue component value (0-255)

Gets the blue component value of a given color.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(100, 100);
const cyan = gd.trueColor(0, 255, 255);

const blueComponent = img.blue(cyan);
console.log('Blue component:', blueComponent); // 255

img.destroy();
```

### gd.Image#alpha(color)

#### Parameters

- `color` - Color value or index

#### Return value

- `Number` - Alpha component value (0-127, where 0 is opaque and 127 is transparent)

Get the alpha value of a color. _Note_ after version node-gd@1.3.1 this method has been renamed to _lower case_ `alpha` to reflect its function in comparison with the `gd.Image#red()`, `gd.Image#green()` and `gd.Image#blue()` methods.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(100, 100);
const semiTransparentRed = gd.trueColorAlpha(255, 0, 0, 64);

const alphaComponent = img.alpha(semiTransparentRed);
console.log('Alpha component:', alphaComponent); // 64

// Extract all components
const r = img.red(semiTransparentRed);
const g = img.green(semiTransparentRed);
const b = img.blue(semiTransparentRed);
const a = img.alpha(semiTransparentRed);
console.log(`RGBA: ${r}, ${g}, ${b}, ${a}`);

img.destroy();
```

### gd.Image#getTransparent()

#### Return value

- `Number` - Color index of the transparent color, or -1 if no transparent color is set

Get the index of the transparent color in a palette image.

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);
const red = img.colorAllocate(255, 0, 0);

// Set red as transparent
img.colorTransparent(red);

const transparentIndex = img.getTransparent();
console.log('Transparent color index:', transparentIndex);

img.destroy();
```

### gd.Image#colorDeallocate(color)

#### Parameters

- `color` - Color index to deallocate

#### Return value

- `gd.Image` - Returns the image instance for method chaining

Deallocate a color from the palette (palette images only).

```javascript
const gd = require('node-gd');

const img = await gd.create(100, 100);
const white = img.colorAllocate(255, 255, 255);
const red = img.colorAllocate(255, 0, 0);
const blue = img.colorAllocate(0, 0, 255);

console.log('Colors before deallocation:', img.colorsTotal);

// Deallocate the red color
img.colorDeallocate(red);

console.log('Colors after deallocation:', img.colorsTotal);

img.destroy();
```

### Color Manipulation

### gd.Image#colorTransparent(color)

Specifies a color index (if a palette image) or an RGB color (if a truecolor image) which should be considered 100% transparent. For truecolor images, this is ignored if an alpha channel is being saved. Use `gd.Image#saveAlpha(0)` to turn off the saving of a full alpha channel in a truecolor image.

### gd.Image#colorReplace(fromColor, toColor)

Replace one color with another. This returns the amount of changed pixels.

### gd.Image#colorReplaceThreshold(fromColor, toColor, threshold)

Replace one color with another. The `threshold` will broaden the range of replaced colors and is a decimal `Number` between `0` and `100`.

### gd.Image#colorReplaceArray(fromColors, toColors)

Replace colors in `fromColors` array to `toColors` array. The arrays should be of equal length.

# Effects

### gd.Image#grayscale()

Remove all color from an image and create a grayscaled image. Only available from libgd2 version 2.1.x.

### gd.Image#gaussianBlur()

Apply gaussian blur. Can be applied multiple times to an image to get more blur.

### gd.Image#negate()

Invert the colors of the image.

### gd.Image#brightness(brightness)

Supply an integer between `0` and `100`.

### gd.Image#contrast(contrast)

The value for contrast is a bit weird. A value of `100` wil return a complete grey image, with grey being exactly `rgb(127, 127, 127)`. For best results, the range applied should be between `-900` and `1100` (so actually `100 + 1000` and `100 - 1000`).

### gd.Image#selectiveBlur()

#### Return value

- `gd.Image` - Returns the image instance for method chaining

Apply selective blur to the image. This blur effect is gentler than gaussian blur and preserves edges better.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');

// Apply selective blur for noise reduction without losing sharpness
img.selectiveBlur();

await img.saveJpeg('./selective-blur.jpg', 85);
img.destroy();
```

### gd.Image#emboss()

#### Return value

- `gd.Image` - Returns the image instance for method chaining

Apply an emboss effect to the image, creating a 3D-like appearance.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');

// Apply emboss effect
img.emboss();

await img.saveJpeg('./embossed.jpg', 85);
img.destroy();
```

### gd.Image#flipHorizontal()

#### Return value

- `gd.Image` - Returns the image instance for method chaining

Flip the image on the canvas over the vertical axis. This means that what's at the right side of the images will be reflected to the left side: pixels move only horizontally.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');

// Flip horizontally (mirror effect)
img.flipHorizontal();

await img.saveJpeg('./flipped-horizontal.jpg', 85);
img.destroy();
```

### gd.Image#flipVertical()

#### Return value

- `gd.Image` - Returns the image instance for method chaining

Flip the image on the canvas over the horizontal axis, the oposite of the above.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');

// Flip vertically (upside down)
img.flipVertical();

await img.saveJpeg('./flipped-vertical.jpg', 85);
img.destroy();
```

### gd.Image#flipBoth()

#### Return value

- `gd.Image` - Returns the image instance for method chaining

Flip the image on the canvas over both axes.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');

// Flip both horizontally and vertically (180 degree rotation)
img.flipBoth();

await img.saveJpeg('./flipped-both.jpg', 85);
img.destroy();
```

### gd.Image#crop(x, y, width, height)

Crop the supplied image from a certain point to a certain size. Will return a new instance of `gd.Image`. Negative numbers, i.e. when going out of the image bounds, can result in images with black parts. Cropping transparent images currently does not work due to a bug in libgd. An alternative is to create the destination image first with a transparent background and then copy a portion of the source image on top of it.

### gd.Image#cropAuto(mode)

#### Parameters

- `mode` - Crop mode (0-4)

#### Return value

- `gd.Image` - New cropped image instance

The mode parameter is a value from 0 to 4, so there are 5 modes:

- `0`: default: 4 corners or background;
- `1`: crop using the transparent color;
- `2`: crop black borders;
- `3`: crop white borders;
- `4`: crop using colors of the 4 corners.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');

// Auto crop white borders
const cropped = img.cropAuto(3);

await cropped.saveJpeg('./auto-cropped.jpg', 85);
img.destroy();
cropped.destroy();
```

### gd.Image#cropThreshold(color, threshold)

#### Parameters

- `color` - Color to crop around
- `threshold` - Threshold value (0-100)

#### Return value

- `gd.Image` - New cropped image instance

Crop by color, with a threshold. Use for color an integer and for threshold a number.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');
const white = gd.trueColor(255, 255, 255);

// Crop around white color with 10% threshold
const cropped = img.cropThreshold(white, 10);

await cropped.saveJpeg('./threshold-cropped.jpg', 85);
img.destroy();
cropped.destroy();
```

### gd.Image#sharpen(pct)

#### Parameters

- `pct` - Sharpening percentage (0-100)

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Apply sharpening effect to the image. Does not work on non-true color images. A value lower than 0 for `pct` is ignored.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(400, 300);
const blue = gd.trueColor(0, 0, 255);
img.filledRectangle(0, 0, 399, 299, blue);

// Apply 75% sharpening
img.sharpen(75);

await img.saveJpeg('./sharpened-image.jpg', 90);
img.destroy();
```

### gd.Image#createPaletteFromTrueColor(ditherFlag, colorsWanted)

#### Parameters

- `ditherFlag` - Dithering flag (0 or 1)
- `colorsWanted` - Number of colors in the palette (1-256)

#### Return value

- `gd.Image` - New palette-based image instance

Create and return a new palette based image instance of gd.Image. The `ditherFlag` is either `0` or `1` (a `Number`, not a `Boolean`), `colorsWanted` a value between 1 and 256.

```javascript
const gd = require('node-gd');

const trueColorImg = await gd.openJpeg('./photo.jpg');

// Create palette version with 64 colors and dithering
const paletteImg = trueColorImg.createPaletteFromTrueColor(1, 64);

await paletteImg.savePng('./palette-version.png', 0);
trueColorImg.destroy();
paletteImg.destroy();
```

### gd.Image#trueColorToPalette(ditherFlag, colorsWanted)

#### Parameters

- `ditherFlag` - Dithering flag (0 or 1)
- `colorsWanted` - Number of colors in the palette (1-256)

#### Return value

- `number` - Success status (implementation dependent)

Modify the current image into a palette based image. The `ditherFlag` is either `0` or `1` (a `Number`, not a `Boolean`), `colorsWanted` a value between 1 and 256.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./photo.jpg');

// Convert to palette with 128 colors, no dithering
const result = img.trueColorToPalette(0, 128);

await img.savePng('./converted-to-palette.png', 0);
img.destroy();
```

### gd.Image#paletteToTrueColor()

#### Return value

- `0 | 1` - Success status (libgd 2.1.0+) or void (libgd 2.0.26)

Modify the current image, if it is a palette based image, into a true color image. From libgd version `2.1.0` this returns either `0` or `1`, in version `2.0.26` it is void.

```javascript
const gd = require('node-gd');

// Create a palette-based image
const img = await gd.create(200, 200);
const red = img.colorAllocate(255, 0, 0);
img.filledRectangle(0, 0, 199, 199, red);

// Convert to true color
const result = img.paletteToTrueColor();

await img.saveJpeg('./converted-to-truecolor.jpg', 90);
img.destroy();
```

### gd.Image#colorMatch(gd.Image)

The parameter should be a palette based image, which will be modified and which can be saved afterwards. The return value is a `Number`. This method tries to better match the colors from the palette based image to those of its true color original. Will throw an `Error` in these cases:

- Images are not of identical dimensions
- The from image is not truecolor
- The target image is not a palette image
- Target image has no colors allocated in the palette

```javascript
const gd = require('node-gd');

// open a true color image
const trueColor = await gd.openFile('/path/to/image.jpg');

// create a new palette based image
var palette = trueColor.createPaletteFromTrueColor(1, 128);

// match colors to enhance the result of the palette image
trueColor.colorMatch(palette);

// save the palette based image
await palette.file('/path/to/result.gif');
palette.destroy();
```

### gd.Image#gifAnimBegin(anim, useGlobalColorMap, loops)

Create an animated GIF.

- `anim` is a `String` referencing a filename to be created, like `./anim.gif`
- `useGlobalColorMap` a value being `-1`, `0` or `1` which determines if the colorMap of the current image will be used as the global colorMap for the whole animation. `-1` indicates `do default`.
- The value for `loops` can be `-1` for no looping, `0` means infinite looping, any other value the amount of loops the animation should run.

### gd.Image#gifAnimAdd(anim, localColorMap, leftOffset, topOffset, delay, disposal, prevFrame)

- Add current image to the specified `anim`.
- `localColorMap` is a flag indicating wether GD should use this image's colorMap
- `leftOffset` and `topOffset` indicate the offset of this frame
- the `delay` is the delay before next frame (in 1/100 sec)
- `disposal` defines how this frame is handled when the next frame is loads. Usually this value should be set to `0`, quote from gd's source: _meaning that the pixels changed by this frame should remain on the display when the next frame begins to render_
- `prevFrame` should refer to the previous frame. If the current image is the first frame, supply `null`.

### gd.Image#gifAnimEnd(anim)

Write and close the GIF animation. A complete working example could look like this:

```javascript
const gd = require('node-gd');
var anim = './anim.gif';

// create first frame
var firstFrame = await gd.create(200, 200);

// allocate some colors
var whiteBackground = firstFrame.colorAllocate(255, 255, 255);
var pink = firstFrame.colorAllocate(255, 0, 255);

// create first frame and draw an ellipse
firstFrame.ellipse(100, -50, 100, 100, pink);
// start animation
firstFrame.gifAnimBegin(anim, 1, -1);
firstFrame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);

var totalFrames = [];
for (var i = 0; i < 30; i++) {
  totalFrames.push(i);
}

totalFrames.forEach(async (i, idx, arr) => {
  var frame = await gd.create(200, 200);
  arr[idx] = frame;
  frame.ellipse(100, i * 10 - 40, 100, 100, pink);
  var lastFrame = i === 0 ? firstFrame : arr[i - 1];
  frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, lastFrame);
  frame.destroy();
});

firstFrame.gifAnimEnd(anim);
firstFrame.destroy();
```

# Copying and resizing

### gd.Image#copy(dest, dx, dy, sx, sy, width, height)

Copy an image onto a destination image: `dest`. You'll have to save the destination image to see the resulting image. This method returns _the image on which it is called_ in order to be able to chain methods.

```javascript
const gd = require('node-gd');
var output = '../output/image-watermark.png';

var watermark = await gd.createFromPng('../input/watermark.png');
var input = await gd.createFromPng('../input/input.png');

watermark.alphaBlending(1);
watermark.saveAlpha(1);

// copy watermark onto input, i.e. onto the destination
watermark.copy(input, 0, 0, 0, 0, 100, 100);

// save the destination
await input.savePng(output, 0);
input.destroy();
watermark.destroy();
```

Example using chaining:

```javascript
const gd = require('node-gd');
var output = '../output/image-watermark.png';
var output2 = '../output/image-watermark2.png';

var watermark = await gd.createFromPng('../input/watermark.png');
var input = await gd.createFromPng('../input/input.png');
var input2 = await gd.createFromPng('../input/input2.png');

watermark
  .alphaBlending(1)
  .saveAlpha(1)
  .copy(input, 0, 0, 0, 0, 100, 100)
  .contrast(-900)
  .copy(input2, 0, 0, 0, 0, 100, 100);

await input.savePng(output);
await input2.savePng(output2, 0);
```

### gd.Image#copyResized(dest, dx, dy, sx, sy, dw, dh, sw, sh)

#### Parameters

- `dest` - Destination image
- `dx, dy` - Destination coordinates
- `sx, sy` - Source coordinates  
- `dw, dh` - Destination width and height
- `sw, sh` - Source width and height

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Copy and resize a portion of the image to a destination image using nearest-neighbor scaling.

```javascript
const gd = require('node-gd');

const source = await gd.openJpeg('./source.jpg');
const dest = await gd.createTrueColor(400, 300);

// Copy from source (50,50) 200x150 to dest (0,0) 400x300
source.copyResized(dest, 0, 0, 50, 50, 400, 300, 200, 150);

await dest.saveJpeg('./resized-copy.jpg', 85);
source.destroy();
dest.destroy();
```

### gd.Image#copyResampled(dest, dx, dy, sx, sy, dw, dh, sw, sh)

#### Parameters

- `dest` - Destination image
- `dx, dy` - Destination coordinates
- `sx, sy` - Source coordinates  
- `dw, dh` - Destination width and height
- `sw, sh` - Source width and height

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Copy and resize a portion of the image to a destination image using smooth interpolation for better quality.

```javascript
const gd = require('node-gd');

const source = await gd.openJpeg('./source.jpg');
const dest = await gd.createTrueColor(800, 600);

// Copy from source (0,0) 400x300 to dest (0,0) 800x600 with smooth scaling
source.copyResampled(dest, 0, 0, 0, 0, 800, 600, 400, 300);

await dest.saveJpeg('./resampled-copy.jpg', 90);
source.destroy();
dest.destroy();
```

### gd.Image#copyRotated(dest, dx, dy, sx, sy, sw, sh, angle)

#### Parameters

- `dest` - Destination image
- `dx, dy` - Destination coordinates
- `sx, sy` - Source coordinates  
- `sw, sh` - Source width and height
- `angle` - Rotation angle in degrees

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Copy and rotate a portion of the image to a destination image.

```javascript
const gd = require('node-gd');

const source = await gd.openJpeg('./source.jpg');
const dest = await gd.createTrueColor(600, 600);

// Copy from source (0,0) 400x300 to dest (100,100) rotated 45 degrees
source.copyRotated(dest, 100, 100, 0, 0, 400, 300, 45);

await dest.saveJpeg('./rotated-copy.jpg', 85);
source.destroy();
dest.destroy();
```

### gd.Image#copyMerge(dest, dx, dy, sx, sy, width, height, pct)

#### Parameters

- `dest` - Destination image
- `dx, dy` - Destination coordinates
- `sx, sy` - Source coordinates  
- `width, height` - Copy dimensions
- `pct` - Merge percentage (0-100)

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Copy and merge a portion of the image to a destination image with specified transparency.

```javascript
const gd = require('node-gd');

const source = await gd.openJpeg('./source.jpg');
const dest = await gd.openJpeg('./background.jpg');

// Copy from source (0,0) 200x150 to dest (50,50) with 50% transparency
source.copyMerge(dest, 50, 50, 0, 0, 200, 150, 50);

await dest.saveJpeg('./merged-copy.jpg', 85);
source.destroy();
dest.destroy();
```

### gd.Image#copyMergeGray(dest, dx, dy, sx, sy, width, height, pct)

#### Parameters

- `dest` - Destination image
- `dx, dy` - Destination coordinates
- `sx, sy` - Source coordinates  
- `width, height` - Copy dimensions
- `pct` - Merge percentage (0-100)

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Copy and merge a portion of the image to a destination image with grayscale blending.

```javascript
const gd = require('node-gd');

const source = await gd.openJpeg('./source.jpg');
const dest = await gd.openJpeg('./background.jpg');

// Copy from source (0,0) 200x150 to dest (50,50) with 75% gray merge
source.copyMergeGray(dest, 50, 50, 0, 0, 200, 150, 75);

await dest.saveJpeg('./gray-merged-copy.jpg', 85);
source.destroy();
dest.destroy();
```

### gd.Image#paletteCopy(dest)

#### Parameters

- `dest` - Destination image

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Copy the palette from the current image to the destination image.

```javascript
const gd = require('node-gd');

const source = await gd.create(200, 200);
const red = source.colorAllocate(255, 0, 0);
const green = source.colorAllocate(0, 255, 0);
source.filledRectangle(0, 0, 199, 199, red);

const dest = await gd.create(200, 200);

// Copy palette from source to dest
source.paletteCopy(dest);

await dest.savePng('./palette-copied.png', 0);
source.destroy();
dest.destroy();
```

### gd.Image#squareToCircle(radius)

#### Parameters

- `radius` - Radius of the circular transformation

#### Return value

- `gd.Image` - Returns the current image instance for method chaining

Transform the image from a square to a circle with specified radius.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 200);
const blue = gd.trueColor(0, 0, 255);
const red = gd.trueColor(255, 0, 0);

// Draw a square pattern
img.filledRectangle(0, 0, 199, 199, blue);
img.filledRectangle(50, 50, 149, 149, red);

// Transform to circle with radius 100
img.squareToCircle(100);

await img.saveJpeg('./square-to-circle.jpg', 85);
img.destroy();
```

# Misc

### gd.Image#compare(image)

#### Parameters

- `image` - Image to compare with

#### Return value

- `number` - Bitmask of comparison flags

Returns a bitmask of Image Comparison flags where each set flag signals which attributes of the images are different.

- `1`: Actual image IS different;
- `2`: Number of colors in pallette differ;
- `4`: Image colors differ;
- `8`: Image width differs;
- `16`: Image heights differ;
- `32`: Transparent color differs;
- `64`: Background color differs;
- `128`: Interlaced setting differs;
- `256`: Truecolor vs palette differs.

```javascript
const gd = require('node-gd');

const img1 = await gd.openJpeg('./image1.jpg');
const img2 = await gd.openJpeg('./image2.jpg');

const comparison = img1.compare(img2);

if (comparison === 0) {
    console.log('Images are identical');
} else {
    if (comparison & 1) console.log('Image content differs');
    if (comparison & 8) console.log('Image width differs');
    if (comparison & 16) console.log('Image height differs');
    if (comparison & 256) console.log('Color type differs');
}

img1.destroy();
img2.destroy();
```

### Saving graphic images

The functions `gd.Image#savePng`, `gd.Image#saveJpeg`, `gd.Image#saveGif`, etc. are convenience functions which will be processed asynchronously when a callback is supplied. All of the following have a counterpart like `gd.Image#png` and `gd.Image#pngPtr` which write to disk synchronously or store the image data in a memory pointer respectively. `gd.Image#jpeg` will return the instance of `gd.Image`, `gd.Image#jpgPtr` will return the newly created image data.

```javascript
const gd = require('node-gd');
gd.openPng('/path/to/input.png', async function (err, img) {
  if (err) {
    throw err;
  }

  // create jpg pointer from png
  var jpgImageData = img.jpegPtr(0); // jpeg quality 0
  // create gif pointer from png
  var gifImageData = img.gifPtr();

  // create instance of gd.Image() for jpg file
  var jpgImage = gd.createFromJpegPtr(jpgImageData);
  await jpgImage.file('./test01.jpg');

  // create instance of gd.Image() for gif file
  var gifImage = gd.createFromGifPtr(gifImageData);
  await gifImage.file('./test01.gif');
  img.destroy();
});
```

The above example shows how to create a JPEG and GIF file from a PNG file.

### gd.Image#savePng(path, level)

Save image data as a PNG file. The callback will receive an error object as a parameter, only if an error occurred. When a callback is supplied, the image will be written asynchronously by `fs.writeFile()`, using `gd.Image#pngPtr()` to first write it to memory in the given format. `level` can be value between `0` and `9` and refers to a zlib compression level. A level of `-1` will let libpng12 decide what the default is.

### gd.Image#saveJpeg(path, quality)

Save image data as a JPEG file. Returns Promise which resolves with true. Quality can be a `Number` between `0` and `100`.

### gd.Image#saveGif(path)

Save image data as a GIF file.

### gd.Image#saveWBMP(path, foreground)

Save image as a 2 color WBMP file. `foreground` is an integer value or `0x000000` value which defines the dark color of the image. All other colors will be changed into white.

### gd.Image#saveBmp(path, compression)

Only available from GD version 2.1.1. The compression parameter is eiterh `0` for no compression and `1` for compression. This value only affects paletted images.

### gd.Image#saveTiff(path)

As per libgd 2.2.4, opening TIFF files appears to be fixed, and saving image data as TIFF worked already fine. Therefore, `gd.openTiff()` is available again. Only available from GD version 2.1.1.

### gd.Image#saveWebp(path, quality)

#### Parameters

- `path`
  - A string referencing the file path where the WebP image should be saved
- `quality`
  - Optional. A number between 0 and 100 representing the WebP quality level. Default is -1 (use libgd default).

#### Return value

- `Promise`
  - Promise resolving to `true` if successful

Save image data as a WebP file. WebP provides better compression than PNG while maintaining high quality.

```javascript
const gd = require('node-gd');

const img = await gd.createTrueColor(200, 200);
const red = gd.trueColor(255, 0, 0);
img.filledRectangle(50, 50, 150, 150, red);

// Save with high quality
await img.saveWebp('./output.webp', 90);
img.destroy();
```

### gd.Image#saveHeif(path, quality)

#### Parameters

- `path`
  - A string referencing the file path where the HEIF image should be saved
- `quality`
  - Optional. A number between 0 and 100 representing the HEIF quality level. Default is -1 (use libgd default).

#### Return value

- `Promise`
  - Promise resolving to `true` if successful

Save image data as a HEIF file. HEIF provides excellent compression efficiency while maintaining high quality.

```javascript
const gd = require('node-gd');

const img = await gd.openJpeg('./input.jpg');
// Convert JPEG to HEIF with high quality
await img.saveHeif('./output.heif', 95);
img.destroy();
```

### gd.Image#saveAvif(path, quality)

#### Parameters

- `path`
  - A string referencing the file path where the AVIF image should be saved
- `quality`
  - Optional. A number between 0 and 100 representing the AVIF quality level. Default is -1 (use libgd default).

#### Return value

- `Promise`
  - Promise resolving to `true` if successful

Save image data as an AVIF file. AVIF provides superior compression efficiency, often outperforming both JPEG and WebP.

```javascript
const gd = require('node-gd');

const img = await gd.createFromPng('./input.png');
// Convert PNG to AVIF with medium quality for smaller file size
await img.saveAvif('./output.avif', 75);
img.destroy();
```

### gd.Image#file(path)

Lets GD decide in which format the image should be stored to disk, based on the supplied file name extension. Only available from GD version 2.1.1. Returns a Promise.

### Image properties

Any instance of `gd.Image()` has a basic set of instance properties accessible as read only values.

```javascript
const gd = require('node-gd');
var img = gd.creatTrueColor(100, 100);
console.log(img);
img.destroy();
```

Will yield to something like:

```javascript
{
  colorsTotal: 0,
  interlace: true,
  height: 100,
  width: 100,
  trueColor: 1
}
```

### gd.Image#colorsTotal

For paletted images, returns the amount of colors in the palette.

### gd.Image#interlace

`Boolean` value for if the image is interlaced or not. This property can also be set. When set to `true` for Jpeg images, GD will save it as a progressive Jpeg image.

```javascript
const gd = require('node-gd');
var img = await gd.createTrueColor(100, 100);
img.interlace = true; // set interlace to true

// will save jpeg as progressive jpeg image.
await img.saveJpeg('/path/to/output.jpg', 100);
img.destroy();
```

### gd.Image#height

Returns the height of the image as `Number`.

### gd.Image#width

Returns the width of the image as `Number`.

### gd.Image#trueColor

Returns nonzero if the image is a truecolor image, zero for a palette image.

# libgd2 version information

Be aware that since `node-gd` version 0.3.x libgd2 version 2.1.x is mostly supported. `node-gd` version 0.2.x is backed at best with libgd2 version 2.0.x. Run `gdlib-config --version` to check the version of libgd2 on your system. `node-gd` should build successfully for both libgd2 version 2.0.x as wel as for 2.1.x. The main difference is that some functions will not be available. These include:

- `toGrayscale()`
- `emboss()`
- `gaussianBlur()`
- `negate()`
- `brightness()`
- `contrast()`
- `selectiveBlur()`
- `openBmp()`
- `openFile()`
- `createFromBmp()`
- `createFromBmpPtr()`
- `createFromFile()`
- `saveBmp()`
- `saveTiff()`
- `saveImage()`

Another way to check the installed GD version on your system:

```javascript
import gd from 'node-gd';

const version = gd.getGDVersion();
console.log(version); // 2.3.1 or the like
```

# Test

```bash
$ npm test
```

The `test/output/` directory contains the resulting images of the test script. The tests use, in some cases, a GNU Freefont font, which is licensed under the GNU General Public License v3.

# Related

- [Original author's repo](https://github.com/taggon/node-gd)
- [node-canvas](https://github.com/LearnBoost/node-canvas) uses libcairo to emulate browser HTML5 Canvas' image manipulation abilities within Node.js
