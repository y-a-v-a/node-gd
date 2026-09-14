import fs from 'fs';

import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);
const source = currentDir + '/fixtures/';
const target = currentDir + '/output/';

const red = gd.trueColor(255, 0, 0);
const blue = gd.trueColor(0, 0, 255);
const white = gd.trueColor(255, 255, 255);

async function sample() {
  const img = await gd.createTrueColor(16, 12);
  img.filledRectangle(0, 0, 15, 11, white);
  img.filledRectangle(2, 2, 7, 7, red);
  img.filledEllipse(11, 6, 6, 6, blue);
  return img;
}

async function paletteSample() {
  const img = await gd.create(16, 12);
  img.colorAllocate(255, 255, 255);
  const paletteRed = img.colorAllocate(255, 0, 0);
  img.filledRectangle(2, 2, 7, 7, paletteRed);
  return img;
}

function signature(buffer, length) {
  return buffer.subarray(0, length).toString('latin1');
}

describe('Encoding and decoding images', function () {
  describe('PNG', function () {
    it('gd.Image#pngPtr() -- returns a Buffer with a PNG signature', async function () {
      const img = await sample();
      const data = img.pngPtr();
      assert.instanceOf(data, Buffer);
      assert.equal(signature(data, 8), '\x89PNG\r\n\x1a\n');
      img.destroy();
    });

    it('gd.createFromPngPtr() -- restores an identical image', async function () {
      const img = await sample();
      const copy = gd.createFromPngPtr(img.pngPtr());
      assert.instanceOf(copy, gd.Image);
      assert.equal(copy.compare(img), 0);
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#pngPtr() -- a higher compression level results in a smaller Buffer', async function () {
      const img = await gd.openJpeg(source + 'input.jpg');
      const fast = img.pngPtr(0);
      const best = img.pngPtr(9);
      assert.isBelow(best.length, fast.length);
      assert.equal(gd.createFromPngPtr(best).compare(gd.createFromPngPtr(fast)), 0);
      img.destroy();
    });

    it('gd.Image#pngPtr() -- throws a TypeError for a non numeric level', async function () {
      const img = await sample();
      assert.throws(() => img.pngPtr('9'), TypeError, /Optional argument 0 must be a Number/);
      img.destroy();
    });

    it('gd.createFromPngPtr() -- returns null for invalid data', function () {
      assert.isNull(gd.createFromPngPtr(Buffer.from('this is not a png')));
    });

    it('gd.createFromPngPtr() -- throws an Error without arguments', function () {
      assert.throws(() => gd.createFromPngPtr(), Error, /Expected 1 argument/);
    });

    it('gd.Image#savePng() and gd.openPng() -- write and read a file', async function () {
      const t = target + 'output-roundtrip.png';
      const img = await sample();
      assert.isTrue(await img.savePng(t, 1));

      const copy = await gd.openPng(t);
      assert.equal(copy.compare(img), 0);
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#png() -- writes a file asynchronously', async function () {
      const t = target + 'output-async.png';
      const img = await sample();
      assert.isTrue(await img.png(t, 1));
      assert.isTrue(fs.existsSync(t));
      img.destroy();
    });

    it('gd.Image#png() -- rejects with a message when the file cannot be written', async function () {
      const img = await sample();
      let reason;
      try {
        await img.png(target + 'does/not/exist.png');
      } catch (e) {
        reason = e;
      }
      assert.equal(reason, 'Cannot save PNG file');
      img.destroy();
    });

    it('gd.Image#png() -- throws a TypeError when the path is not a String', async function () {
      const img = await sample();
      assert.throws(() => img.png(42), TypeError, /Argument 0 must be a string/);
      img.destroy();
    });
  });

  describe('JPEG', function () {
    it('gd.Image#jpegPtr() -- returns a Buffer with a JPEG signature', async function () {
      const img = await sample();
      const data = img.jpegPtr(90);
      assert.equal(data[0], 0xff);
      assert.equal(data[1], 0xd8);
      img.destroy();
    });

    it('gd.Image#jpegPtr() -- a lower quality results in a smaller Buffer', async function () {
      const img = await gd.openPng(source + 'input.png');
      assert.isBelow(img.jpegPtr(10).length, img.jpegPtr(95).length);
      img.destroy();
    });

    it('gd.createFromJpegPtr() -- restores an image of the same dimensions', async function () {
      const img = await sample();
      const copy = gd.createFromJpegPtr(img.jpegPtr(100));
      assert.equal(copy.width, img.width);
      assert.equal(copy.height, img.height);
      assert.equal(copy.trueColor, 1);
      const pixel = copy.getTrueColorPixel(4, 4);
      assert.closeTo(copy.red(pixel), 255, 10);
      assert.closeTo(copy.green(pixel), 0, 10);
      copy.destroy();
      img.destroy();
    });

    it('gd.openJpeg() -- rejects with a message when the file does not exist', async function () {
      let reason;
      try {
        await gd.openJpeg(source + 'does-not-exist.jpg');
      } catch (e) {
        reason = e;
      }
      assert.equal(reason, 'Cannot open JPEG file');
    });

    it('gd.openJpeg() -- rejects with a message when the file is not a JPEG', async function () {
      let reason;
      try {
        await gd.openJpeg(source + 'input.png');
      } catch (e) {
        reason = e;
      }
      assert.equal(reason, 'Cannot read JPEG file');
    });

    it('gd.createFromJpeg() -- throws a TypeError when the path is not a String', function () {
      assert.throws(() => gd.createFromJpeg(null), TypeError, /Argument 0 must be a string/);
    });
  });

  describe('GIF', function () {
    it('gd.Image#gifPtr() -- returns a Buffer with a GIF signature', async function () {
      const img = await paletteSample();
      assert.equal(signature(img.gifPtr(), 6), 'GIF87a');
      img.destroy();
    });

    it('gd.createFromGifPtr() -- restores an identical palette image', async function () {
      const img = await paletteSample();
      const copy = gd.createFromGifPtr(img.gifPtr());
      assert.equal(copy.trueColor, 0);
      assert.equal(copy.width, 16);
      assert.equal(copy.getTrueColorPixel(4, 4), red);
      assert.equal(copy.getTrueColorPixel(10, 10), white);
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#gifPtr() -- keeps the transparent color', async function () {
      const img = await paletteSample();
      img.colorTransparent(0);
      const copy = gd.createFromGifPtr(img.gifPtr());
      assert.notEqual(copy.getTransparent(), -1);
      assert.equal(copy.getTransparent(), copy.getPixel(10, 10));
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#gif() and gd.createFromGif() -- write and read a file', async function () {
      const t = target + 'output-roundtrip.gif';
      const img = await paletteSample();
      assert.isTrue(await img.gif(t));
      const copy = await gd.createFromGif(t);
      assert.equal(copy.getTrueColorPixel(4, 4), red);
      copy.destroy();
      img.destroy();
    });
  });

  describe('BMP', function () {
    it('gd.Image#bmpPtr() -- returns a Buffer with a BMP signature', async function () {
      const img = await sample();
      assert.equal(signature(img.bmpPtr(), 2), 'BM');
      img.destroy();
    });

    it('gd.createFromBmpPtr() -- restores the image', async function () {
      const img = await sample();
      const copy = gd.createFromBmpPtr(img.bmpPtr(0));
      assert.equal(copy.width, img.width);
      assert.equal(copy.height, img.height);
      assert.equal(copy.getTrueColorPixel(4, 4), red);
      assert.equal(copy.getTrueColorPixel(11, 6), blue);
      copy.destroy();
      img.destroy();
    });

    it('gd.createFromBmpPtr() -- restores a compressed palette image', async function () {
      const img = await paletteSample();
      const copy = gd.createFromBmpPtr(img.bmpPtr(1));
      assert.equal(copy.getTrueColorPixel(4, 4), red);
      assert.equal(copy.getTrueColorPixel(10, 10), white);
      copy.destroy();
      img.destroy();
    });
  });

  describe('WBMP', function () {
    it('gd.createFromWBMPPtr() -- restores a black and white image', async function () {
      const img = await paletteSample();
      const foreground = img.colorExact(255, 0, 0);
      const copy = gd.createFromWBMPPtr(img.wbmpPtr(foreground));

      assert.equal(copy.width, 16);
      assert.equal(copy.height, 12);
      assert.equal(copy.colorsTotal, 2);
      assert.notEqual(copy.getPixel(4, 4), copy.getPixel(10, 10));
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#wbmpPtr() -- throws a TypeError when the foreground color is missing', async function () {
      const img = await paletteSample();
      assert.throws(() => img.wbmpPtr(), TypeError, /foreground color/);
      img.destroy();
    });

    it('gd.Image#wbmp() and gd.createFromWBMP() -- write and read a file', async function () {
      const t = target + 'output-roundtrip.wbmp';
      const img = await paletteSample();
      assert.isTrue(await img.wbmp(t, 1));
      const copy = await gd.createFromWBMP(t);
      assert.equal(copy.width, 16);
      copy.destroy();
      img.destroy();
    });
  });

  describe('WebP', function () {
    before(function () {
      if (!gd.GD_WEBP) {
        this.skip();
      }
    });

    it('gd.Image#webpPtr() -- returns a Buffer with a WebP signature', async function () {
      const img = await sample();
      const data = img.webpPtr();
      assert.equal(signature(data, 4), 'RIFF');
      assert.equal(data.subarray(8, 12).toString('latin1'), 'WEBP');
      img.destroy();
    });

    it('gd.createFromWebpPtr() -- restores a losslessly encoded image', async function () {
      const img = await sample();
      // quality >= 101 means lossless in libgd
      const copy = gd.createFromWebpPtr(img.webpPtr(101));
      assert.equal(copy.width, img.width);
      assert.equal(copy.getTrueColorPixel(4, 4), red);
      assert.equal(copy.getTrueColorPixel(11, 6), blue);
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#saveWebp() and gd.openWebp() -- write and read a file', async function () {
      const t = target + 'output-roundtrip.webp';
      const img = await sample();
      assert.isTrue(await img.saveWebp(t, 80));
      const copy = await gd.openWebp(t);
      assert.equal(copy.height, img.height);
      copy.destroy();
      img.destroy();
    });
  });

  describe('TIFF', function () {
    before(function () {
      if (!gd.GD_TIFF) {
        this.skip();
      }
    });

    it('gd.createFromTiffPtr() -- restores an identical image from gd.Image#tiffPtr()', async function () {
      const img = await sample();
      const data = img.tiffPtr();
      assert.include(['II*\x00', 'MM\x00*'], signature(data, 4));

      const copy = gd.createFromTiffPtr(data);
      assert.equal(copy.getTrueColorPixel(4, 4), red);
      assert.equal(copy.getTrueColorPixel(11, 6), blue);
      copy.destroy();
      img.destroy();
    });
  });

  describe('gd.Image#file() and gd.createFromFile()', function () {
    it('determines the format from the file extension', async function () {
      const img = await sample();
      for (const ext of ['png', 'gif', 'jpg', 'bmp']) {
        const t = `${target}output-by-extension.${ext}`;
        assert.isTrue(await img.file(t), ext);
        const copy = await gd.createFromFile(t);
        assert.equal(copy.width, 16, ext);
        copy.destroy();
      }
      img.destroy();
    });

    it('rejects when the extension is unknown', async function () {
      const img = await sample();
      let reason;
      try {
        await img.file(target + 'output-unknown.xyz');
      } catch (e) {
        reason = e;
      }
      assert.equal(reason, 'Cannot save file');
      img.destroy();
    });

    it('gd.createFromFile() -- rejects when the file cannot be read', async function () {
      let reason;
      try {
        await gd.createFromFile(source + 'does-not-exist.png');
      } catch (e) {
        reason = e;
      }
      assert.equal(reason, 'Cannot read image file');
    });
  });

  describe('Save convenience wrappers', function () {
    it('gd.Image#saveJpeg() -- rejects when the file cannot be written', async function () {
      const img = await sample();
      let error;
      try {
        await img.saveJpeg(target + 'does/not/exist.jpg', 80);
      } catch (e) {
        error = e;
      }
      assert.instanceOf(error, Error);
      assert.equal(error.code, 'ENOENT');
      img.destroy();
    });

    it('gd.Image#saveBmp() -- writes the same data as gd.Image#bmpPtr()', async function () {
      const t = target + 'output-wrapper.bmp';
      const img = await sample();
      await img.saveBmp(t, 0);
      assert.isTrue(fs.readFileSync(t).equals(img.bmpPtr(0)));
      img.destroy();
    });

    it('save wrappers are not enumerable', async function () {
      const img = await sample();
      assert.notInclude(Object.keys(gd.Image.prototype), 'savePng');
      assert.isFunction(img.savePng);
      img.destroy();
    });
  });

  describe('Resolution', function () {
    it('gd.Image#resX and gd.Image#resY -- default to 96 DPI', async function () {
      const img = await sample();
      assert.equal(img.resX, 96);
      assert.equal(img.resY, 96);
      img.destroy();
    });

    it('gd.Image#setResolution() -- changes the resolution', async function () {
      const img = await sample();
      assert.strictEqual(img.setResolution(300, 150), img);
      assert.equal(img.resX, 300);
      assert.equal(img.resY, 150);
      img.destroy();
    });

    it('gd.Image#setResolution() -- resolution is stored in PNG files', async function () {
      const img = await sample();
      img.setResolution(300, 300);
      const copy = gd.createFromPngPtr(img.pngPtr());
      assert.closeTo(copy.resX, 300, 1);
      assert.closeTo(copy.resY, 300, 1);
      copy.destroy();
      img.destroy();
    });

    it('gd.Image#setResolution() -- throws an Error when too few arguments are supplied', async function () {
      const img = await sample();
      assert.throws(() => img.setResolution(300), Error, /Expected 2 argument/);
      img.destroy();
    });
  });

  describe('String representation', function () {
    it('gd.toString() -- returns [object Gd]', function () {
      assert.equal(String(gd), '[object Gd]');
    });

    it('gd.Image#toString() -- returns [object Image]', async function () {
      const img = await sample();
      assert.equal(`${img}`, '[object Image]');
      img.destroy();
    });
  });

  describe('Encoding a destroyed image', function () {
    it('throws an Error', async function () {
      const img = await sample();
      img.destroy();
      assert.throws(() => img.pngPtr(), Error, /already destroyed/);
      assert.throws(() => img.jpegPtr(), Error, /already destroyed/);
      assert.throws(() => img.gifPtr(), Error, /already destroyed/);
      assert.throws(() => img.bmpPtr(), Error, /already destroyed/);
    });

    it('can be destroyed twice without error', async function () {
      const img = await sample();
      img.destroy();
      assert.doesNotThrow(() => img.destroy());
    });
  });
});
