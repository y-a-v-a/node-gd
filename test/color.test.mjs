import gd from '../index.js';
import { assert } from 'chai';

describe('Color functions', function () {
  describe('Palette images', function () {
    let img;

    beforeEach(async function () {
      img = await gd.create(20, 20);
    });

    afterEach(function () {
      img.destroy();
    });

    it('gd.Image#colorAllocate() -- returns increasing palette indexes', function () {
      assert.equal(img.colorsTotal, 0);
      assert.equal(img.colorAllocate(255, 255, 255), 0);
      assert.equal(img.colorAllocate(255, 0, 0), 1);
      assert.equal(img.colorAllocate(0, 255, 0), 2);
      assert.equal(img.colorsTotal, 3);
    });

    it('gd.Image#colorAllocate() -- returns -1 when the palette is full', function () {
      for (let i = 0; i < 256; i++) {
        assert.equal(img.colorAllocate(i, i, i), i);
      }
      assert.equal(img.colorsTotal, 256);
      assert.equal(img.colorAllocate(1, 2, 3), -1);
    });

    it('gd.Image#colorAllocate() -- defaults to black when no arguments are supplied', function () {
      const color = img.colorAllocate();
      assert.equal(img.red(color), 0);
      assert.equal(img.green(color), 0);
      assert.equal(img.blue(color), 0);
    });

    it('gd.Image#colorAllocate() -- throws a TypeError when an argument is not a Number', function () {
      assert.throws(() => img.colorAllocate('255', 0, 0), TypeError, /Optional argument 0 must be a Number/);
    });

    it('gd.Image#colorExact() -- returns the index of an exact match or -1', function () {
      img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);

      assert.equal(img.colorExact(255, 0, 0), red);
      assert.equal(img.colorExact(254, 0, 0), -1);
    });

    it('gd.Image#colorExactAlpha() -- takes the alpha channel into account', function () {
      const translucent = img.colorAllocateAlpha(255, 0, 0, 50);

      assert.equal(img.colorExactAlpha(255, 0, 0, 50), translucent);
      assert.equal(img.colorExactAlpha(255, 0, 0, 0), -1);
    });

    it('gd.Image#colorClosest() -- returns the index of the nearest color', function () {
      const white = img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);
      const blue = img.colorAllocate(0, 0, 255);

      assert.equal(img.colorClosest(250, 10, 10), red);
      assert.equal(img.colorClosest(10, 10, 200), blue);
      assert.equal(img.colorClosest(240, 240, 240), white);
    });

    it('gd.Image#colorClosest() -- returns -1 when no colors are allocated', function () {
      assert.equal(img.colorClosest(10, 10, 10), -1);
    });

    it('gd.Image#colorClosestAlpha() -- takes the alpha channel into account', function () {
      const opaque = img.colorAllocateAlpha(255, 0, 0, 0);
      const transparent = img.colorAllocateAlpha(255, 0, 0, 127);

      assert.equal(img.colorClosestAlpha(255, 0, 0, 10), opaque);
      assert.equal(img.colorClosestAlpha(255, 0, 0, 120), transparent);
    });

    it('gd.Image#colorClosestHWB() -- returns the index of the nearest color by hue, whiteness and blackness', function () {
      img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);
      const green = img.colorAllocate(0, 255, 0);

      assert.equal(img.colorClosestHWB(200, 20, 20), red);
      assert.equal(img.colorClosestHWB(20, 200, 20), green);
    });

    it('gd.Image#colorResolve() -- returns an existing index for an exact match', function () {
      img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);

      assert.equal(img.colorResolve(255, 0, 0), red);
      assert.equal(img.colorsTotal, 2);
    });

    it('gd.Image#colorResolve() -- allocates a new color when there is no exact match', function () {
      img.colorAllocate(255, 255, 255);
      const resolved = img.colorResolve(12, 34, 56);

      assert.equal(img.colorsTotal, 2);
      assert.equal(img.red(resolved), 12);
      assert.equal(img.green(resolved), 34);
      assert.equal(img.blue(resolved), 56);
    });

    it('gd.Image#colorResolve() -- returns the closest color when the palette is full', function () {
      for (let i = 0; i < 256; i++) {
        img.colorAllocate(i, i, i);
      }
      const resolved = img.colorResolve(100, 101, 100);
      assert.equal(img.colorsTotal, 256);
      assert.equal(resolved, 100);
    });

    it('gd.Image#colorResolveAlpha() -- allocates a color including alpha', function () {
      const resolved = img.colorResolveAlpha(1, 2, 3, 64);
      assert.equal(img.alpha(resolved), 64);
      assert.equal(img.colorResolveAlpha(1, 2, 3, 64), resolved);
      assert.equal(img.colorsTotal, 1);
    });

    it('gd.Image#colorAllocateAlpha() -- defaults alpha to 100', function () {
      const color = img.colorAllocateAlpha(10, 20, 30);
      assert.equal(img.alpha(color), 100);
    });

    it('gd.Image#colorDeallocate() -- frees a palette slot for reuse', function () {
      img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);
      img.colorAllocate(0, 0, 255);

      assert.strictEqual(img.colorDeallocate(red), img);
      const reused = img.colorAllocate(0, 255, 0);

      assert.equal(reused, red, 'deallocated index gets reused');
      assert.equal(img.green(reused), 255);
      assert.equal(img.colorsTotal, 3);
    });

    it('gd.Image#colorTransparent() and gd.Image#getTransparent() -- set and query the transparent color', function () {
      img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);

      assert.equal(img.getTransparent(), -1, 'no transparent color by default');
      assert.strictEqual(img.colorTransparent(red), img);
      assert.equal(img.getTransparent(), red);
      img.colorTransparent(-1);
      assert.equal(img.getTransparent(), -1);
    });

    it('gd.Image#colorTransparent() -- throws a TypeError when color is missing', function () {
      assert.throws(() => img.colorTransparent(), TypeError, /Argument 0 must be a Number/);
    });

    it('gd.Image#colorReplace() -- replaces the palette index of pixels', function () {
      const white = img.colorAllocate(255, 255, 255);
      const red = img.colorAllocate(255, 0, 0);
      img.filledRectangle(0, 0, 4, 4, red);

      const replaced = img.colorReplace(red, white);

      assert.equal(replaced, 25);
      assert.equal(img.getPixel(2, 2), white);
    });
  });

  describe('True color images', function () {
    let img;
    const red = gd.trueColor(255, 0, 0);
    const nearRed = gd.trueColor(250, 5, 5);
    const blue = gd.trueColor(0, 0, 255);
    const green = gd.trueColor(0, 255, 0);

    beforeEach(async function () {
      img = await gd.createTrueColor(10, 10);
    });

    afterEach(function () {
      img.destroy();
    });

    it('gd.Image#colorAllocate() -- returns a true color value', function () {
      assert.equal(img.colorAllocate(255, 0, 0), red);
      assert.equal(img.colorAllocateAlpha(255, 0, 0, 63), gd.trueColorAlpha(255, 0, 0, 63));
      assert.equal(img.colorsTotal, 0, 'true color images have no palette');
    });

    it('gd.Image#colorExact(), #colorClosest() and #colorResolve() -- return true color values', function () {
      assert.equal(img.colorExact(1, 2, 3), gd.trueColor(1, 2, 3));
      assert.equal(img.colorClosest(1, 2, 3), gd.trueColor(1, 2, 3));
      assert.equal(img.colorResolve(1, 2, 3), gd.trueColor(1, 2, 3));
      assert.equal(img.colorResolveAlpha(1, 2, 3, 4), gd.trueColorAlpha(1, 2, 3, 4));
    });

    it('gd.Image#red(), #green(), #blue() and #alpha() -- decompose a color', function () {
      const color = gd.trueColorAlpha(12, 34, 56, 78);
      assert.equal(img.red(color), 12);
      assert.equal(img.green(color), 34);
      assert.equal(img.blue(color), 56);
      assert.equal(img.alpha(color), 78);
    });

    it('gd.Image#red() -- throws a TypeError when no color is supplied', function () {
      assert.throws(() => img.red(), TypeError, /Argument 0 must be a Number/);
    });

    it('gd.Image#colorReplace() -- returns the amount of replaced pixels', function () {
      img.filledRectangle(0, 0, 9, 9, blue);
      img.filledRectangle(0, 0, 2, 2, red);

      assert.equal(img.colorReplace(red, green), 9);
      assert.equal(img.getTrueColorPixel(1, 1), green);
      assert.equal(img.getTrueColorPixel(5, 5), blue);
      assert.equal(img.colorReplace(red, green), 0, 'nothing left to replace');
    });

    it('gd.Image#colorReplaceThreshold() -- replaces colors similar to the source color', function () {
      img.filledRectangle(0, 0, 9, 9, blue);
      img.filledRectangle(0, 0, 1, 1, red);
      img.filledRectangle(8, 8, 9, 9, nearRed);

      // libgd compares the distance with a strict less-than
      assert.equal(img.colorReplaceThreshold(red, green, 0), 0, 'threshold 0 matches nothing');
      assert.equal(img.colorReplaceThreshold(red, green, 0.01), 4, 'tiny threshold matches exact color only');

      img.filledRectangle(0, 0, 1, 1, red);
      assert.equal(img.colorReplaceThreshold(red, green, 10), 8, 'similar colors within threshold');
      assert.equal(img.getTrueColorPixel(9, 9), green);
      assert.equal(img.getTrueColorPixel(5, 5), blue);
    });

    it('gd.Image#colorReplaceThreshold() -- throws an Error when too few arguments are supplied', function () {
      assert.throws(() => img.colorReplaceThreshold(red, green), Error, /Expected 3 argument/);
    });

    it('gd.Image#colorReplaceArray() -- replaces multiple colors at once', function () {
      img.filledRectangle(0, 0, 9, 9, blue);
      img.filledRectangle(0, 0, 1, 1, red);
      img.filledRectangle(8, 8, 9, 9, green);

      const replaced = img.colorReplaceArray([red, green], [green, red]);

      assert.equal(replaced, 8);
      assert.equal(img.getTrueColorPixel(0, 0), green);
      assert.equal(img.getTrueColorPixel(9, 9), red);
      assert.equal(img.getTrueColorPixel(5, 5), blue);
    });

    it('gd.Image#colorReplaceArray() -- throws an Error when arrays differ in length', function () {
      assert.throws(() => img.colorReplaceArray([red, green], [blue]), Error, /same length/);
    });

    it('gd.Image#colorReplaceArray() -- throws an Error when too few arguments are supplied', function () {
      assert.throws(() => img.colorReplaceArray([red]), Error, /Expected 2 argument/);
    });
  });

  describe('Alpha channel handling', function () {
    const halfRed = gd.trueColorAlpha(255, 0, 0, 64);
    const white = gd.trueColor(255, 255, 255);

    it('gd.Image#alphaBlending() -- blends colors by default on true color images', async function () {
      const img = await gd.createTrueColor(10, 10);
      img.filledRectangle(0, 0, 9, 9, white);
      img.setPixel(5, 5, halfRed);

      const pixel = img.getTrueColorPixel(5, 5);
      assert.equal(img.alpha(pixel), 0, 'result is opaque');
      assert.equal(img.red(pixel), 255);
      assert.isAbove(img.green(pixel), 0, 'white shines through');
      assert.isBelow(img.green(pixel), 255);
      img.destroy();
    });

    it('gd.Image#alphaBlending() -- writes alpha values directly when disabled', async function () {
      const img = await gd.createTrueColor(10, 10);
      img.filledRectangle(0, 0, 9, 9, white);

      assert.strictEqual(img.alphaBlending(0), img);
      img.setPixel(5, 5, halfRed);

      assert.equal(img.getTrueColorPixel(5, 5), halfRed);
      img.destroy();
    });

    it('gd.Image#saveAlpha() -- preserves the alpha channel in PNG output', async function () {
      const img = await gd.createTrueColor(10, 10);
      img.alphaBlending(0);
      img.filledRectangle(0, 0, 9, 9, halfRed);

      assert.strictEqual(img.saveAlpha(1), img);
      const withAlpha = gd.createFromPngPtr(img.pngPtr());
      assert.equal(withAlpha.getTrueColorPixel(3, 3), halfRed);

      img.saveAlpha(0);
      const withoutAlpha = gd.createFromPngPtr(img.pngPtr());
      assert.equal(withoutAlpha.alpha(withoutAlpha.getTrueColorPixel(3, 3)), 0);

      withAlpha.destroy();
      withoutAlpha.destroy();
      img.destroy();
    });

    it('gd.Image#alphaBlending() -- throws a TypeError when the argument is not a Number', async function () {
      const img = await gd.createTrueColor(10, 10);
      assert.throws(() => img.alphaBlending(true), TypeError);
      assert.throws(() => img.saveAlpha(), TypeError);
      img.destroy();
    });
  });

  describe('gd.Image#interlace', function () {
    it('is false by default and can be toggled', async function () {
      const img = await gd.createTrueColor(10, 10);
      assert.strictEqual(img.interlace, false);
      img.interlace = true;
      assert.strictEqual(img.interlace, true);
      img.interlace = false;
      assert.strictEqual(img.interlace, false);
      img.destroy();
    });

    it('ignores non-boolean values', async function () {
      const img = await gd.createTrueColor(10, 10);
      img.interlace = 1;
      assert.strictEqual(img.interlace, false);
      img.destroy();
    });

    it('is preserved when writing and reading a PNG', async function () {
      const img = await gd.createTrueColor(10, 10);
      img.interlace = true;
      const copy = gd.createFromPngPtr(img.pngPtr());
      assert.strictEqual(copy.interlace, true);
      copy.destroy();
      img.destroy();
    });
  });
});
