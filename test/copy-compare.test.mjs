import gd from '../index.js';
import { assert } from 'chai';

/**
 * Values of libgd compare flags which are not exported by node-gd
 */
const GD_CMP_IMAGE = 1;
const GD_CMP_NUM_COLORS = 2;
const GD_CMP_COLOR = 4;
const GD_CMP_SIZE_X = 8;
const GD_CMP_SIZE_Y = 16;
const GD_CMP_TRANSPARENT = 32;
const GD_CMP_INTERLACE = 128;
const GD_CMP_TRUECOLOR = 256;

const red = gd.trueColor(255, 0, 0);
const blue = gd.trueColor(0, 0, 255);
const white = gd.trueColor(255, 255, 255);
const black = gd.trueColor(0, 0, 0);

async function filled(width, height, color) {
  const img = await gd.createTrueColor(width, height);
  img.filledRectangle(0, 0, width - 1, height - 1, color);
  return img;
}

describe('Copying and comparing images', function () {
  describe('gd.Image#copy()', function () {
    it('copies a region of the source onto the destination', async function () {
      const src = await filled(10, 10, red);
      const dst = await filled(30, 30, white);

      assert.strictEqual(src.copy(dst, 5, 5, 0, 0, 10, 10), src);
      assert.equal(dst.getTrueColorPixel(5, 5), red);
      assert.equal(dst.getTrueColorPixel(14, 14), red);
      assert.equal(dst.getTrueColorPixel(4, 4), white);
      assert.equal(dst.getTrueColorPixel(15, 15), white);
      src.destroy();
      dst.destroy();
    });

    it('copies only the requested part of the source', async function () {
      const src = await filled(10, 10, red);
      src.filledRectangle(5, 5, 9, 9, blue);
      const dst = await filled(10, 10, white);

      src.copy(dst, 0, 0, 5, 5, 5, 5);
      assert.equal(dst.getTrueColorPixel(0, 0), blue);
      assert.equal(dst.getTrueColorPixel(4, 4), blue);
      assert.equal(dst.getTrueColorPixel(5, 5), white);
      src.destroy();
      dst.destroy();
    });

    it('maps true color pixels to the palette of a palette destination', async function () {
      const src = await filled(4, 4, red);
      const dst = await gd.create(4, 4);
      dst.colorAllocate(255, 255, 255);

      src.copy(dst, 0, 0, 0, 0, 4, 4);
      const index = dst.getPixel(1, 1);
      assert.equal(dst.red(index), 255);
      assert.equal(dst.green(index), 0);
      assert.equal(dst.blue(index), 0);
      src.destroy();
      dst.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, white);
      assert.throws(() => src.copy(dst, 0, 0, 0, 0, 4), Error, /Expected 7 argument/);
      src.destroy();
      dst.destroy();
    });

    it('throws a TypeError when the destination is not an object', async function () {
      const src = await filled(4, 4, red);
      assert.throws(() => src.copy(1, 0, 0, 0, 0, 4, 4), TypeError, /must be an Image object/);
      src.destroy();
    });
  });

  describe('gd.Image#copyResized()', function () {
    it('scales the source region into the destination region', async function () {
      const src = await filled(2, 2, red);
      src.setPixel(1, 1, blue);
      const dst = await filled(20, 20, white);

      assert.strictEqual(src.copyResized(dst, 0, 0, 0, 0, 10, 10, 2, 2), src);
      assert.equal(dst.getTrueColorPixel(0, 0), red);
      assert.equal(dst.getTrueColorPixel(4, 4), red);
      assert.equal(dst.getTrueColorPixel(9, 9), blue);
      assert.equal(dst.getTrueColorPixel(6, 6), blue);
      assert.equal(dst.getTrueColorPixel(10, 10), white);
      src.destroy();
      dst.destroy();
    });

    it('does not blend colors', async function () {
      const src = await filled(2, 1, black);
      src.setPixel(1, 0, white);
      const dst = await gd.createTrueColor(8, 1);

      src.copyResized(dst, 0, 0, 0, 0, 8, 1, 2, 1);
      for (let x = 0; x < 8; x++) {
        const pixel = dst.getTrueColorPixel(x, 0);
        assert.include([black, white], pixel, `pixel ${x} is not blended`);
      }
      src.destroy();
      dst.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, white);
      assert.throws(() => src.copyResized(dst, 0, 0, 0, 0, 4, 4, 4), Error, /Expected 9 argument/);
      src.destroy();
      dst.destroy();
    });
  });

  describe('gd.Image#copyResampled()', function () {
    it('blends colors when scaling', async function () {
      const src = await filled(2, 1, black);
      src.setPixel(1, 0, white);
      const dst = await gd.createTrueColor(1, 1);

      assert.strictEqual(src.copyResampled(dst, 0, 0, 0, 0, 1, 1, 2, 1), src);
      const pixel = dst.getTrueColorPixel(0, 0);
      assert.closeTo(dst.red(pixel), 127, 2);
      src.destroy();
      dst.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, white);
      assert.throws(() => src.copyResampled(dst, 0, 0, 0, 0, 4, 4, 4), Error, /Expected 9 argument/);
      src.destroy();
      dst.destroy();
    });
  });

  describe('gd.Image#copyRotated()', function () {
    it('copies the source centered on the destination point without rotation', async function () {
      const src = await filled(10, 10, red);
      const dst = await filled(40, 40, white);

      assert.strictEqual(src.copyRotated(dst, 20, 20, 0, 0, 10, 10, 0), src);
      assert.equal(dst.getTrueColorPixel(20, 20), red);
      assert.equal(dst.getTrueColorPixel(16, 16), red);
      assert.equal(dst.getTrueColorPixel(10, 10), white);
      src.destroy();
      dst.destroy();
    });

    it('rotates the source around the destination point', async function () {
      const src = await filled(20, 2, red);
      const dst = await filled(40, 40, white);

      src.copyRotated(dst, 20, 20, 0, 0, 20, 2, 90);
      assert.equal(dst.getTrueColorPixel(20, 13), red, 'vertical after rotation');
      assert.equal(dst.getTrueColorPixel(20, 27), red, 'vertical after rotation');
      assert.equal(dst.getTrueColorPixel(13, 20), white, 'no longer horizontal');
      src.destroy();
      dst.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, white);
      assert.throws(() => src.copyRotated(dst, 0, 0, 0, 0, 4, 4), Error, /Expected 8 argument/);
      src.destroy();
      dst.destroy();
    });
  });

  describe('gd.Image#copyMerge()', function () {
    it('merges the source into the destination using a percentage', async function () {
      const src = await filled(4, 4, white);
      const dst = await filled(4, 4, black);

      assert.strictEqual(src.copyMerge(dst, 0, 0, 0, 0, 4, 4, 50), src);
      const pixel = dst.getTrueColorPixel(1, 1);
      assert.closeTo(dst.red(pixel), 127, 2);
      assert.equal(dst.red(pixel), dst.blue(pixel));
      src.destroy();
      dst.destroy();
    });

    it('acts like a copy with 100 percent', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, black);

      src.copyMerge(dst, 0, 0, 0, 0, 4, 4, 100);
      assert.equal(dst.getTrueColorPixel(1, 1), red);
      src.destroy();
      dst.destroy();
    });

    it('leaves the destination untouched with 0 percent', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, black);

      src.copyMerge(dst, 0, 0, 0, 0, 4, 4, 0);
      assert.equal(dst.getTrueColorPixel(1, 1), black);
      src.destroy();
      dst.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, white);
      assert.throws(() => src.copyMerge(dst, 0, 0, 0, 0, 4, 4), Error, /Expected 8 argument/);
      src.destroy();
      dst.destroy();
    });
  });

  describe('gd.Image#copyMergeGray()', function () {
    it('converts the destination region to gray before merging', async function () {
      const src = await gd.create(4, 4);
      const srcRed = src.colorAllocate(255, 0, 0);
      src.filledRectangle(0, 0, 3, 3, srcRed);
      const dst = await gd.create(4, 4);
      dst.colorAllocate(0, 0, 255);

      assert.strictEqual(src.copyMergeGray(dst, 0, 0, 0, 0, 4, 4, 0), src);
      const index = dst.getPixel(1, 1);
      assert.equal(dst.red(index), dst.green(index));
      assert.equal(dst.green(index), dst.blue(index));
      src.destroy();
      dst.destroy();
    });

    it('acts like a copy with 100 percent', async function () {
      const src = await gd.create(4, 4);
      const srcRed = src.colorAllocate(255, 0, 0);
      src.filledRectangle(0, 0, 3, 3, srcRed);
      const dst = await gd.create(4, 4);
      dst.colorAllocate(0, 0, 255);

      src.copyMergeGray(dst, 0, 0, 0, 0, 4, 4, 100);
      const index = dst.getPixel(1, 1);
      assert.equal(dst.red(index), 255);
      assert.equal(dst.blue(index), 0);
      src.destroy();
      dst.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const src = await filled(4, 4, red);
      const dst = await filled(4, 4, white);
      assert.throws(() => src.copyMergeGray(dst, 0, 0, 0, 0, 4, 4), Error, /Expected 8 argument/);
      src.destroy();
      dst.destroy();
    });
  });

  describe('gd.Image#paletteCopy()', function () {
    it('copies the palette of the source to the destination', async function () {
      const src = await gd.create(4, 4);
      src.colorAllocate(255, 255, 255);
      src.colorAllocate(255, 0, 0);
      src.colorAllocate(0, 0, 255);

      const dst = await gd.create(4, 4);
      dst.colorAllocate(250, 10, 10);

      assert.strictEqual(src.paletteCopy(dst), src);
      assert.equal(dst.colorsTotal, 3);
      assert.equal(dst.colorExact(0, 0, 255), 2);
      // pixels are remapped to the closest color in the new palette
      const index = dst.getPixel(1, 1);
      assert.equal(dst.red(index), 255);
      assert.equal(dst.green(index), 0);
      src.destroy();
      dst.destroy();
    });

    it('throws a TypeError when the destination is missing', async function () {
      const src = await gd.create(4, 4);
      assert.throws(() => src.paletteCopy(), TypeError, /must be an Image object/);
      src.destroy();
    });
  });

  describe('gd.Image#compare()', function () {
    it('returns 0 for identical images', async function () {
      const a = await filled(10, 10, red);
      const b = await filled(10, 10, red);
      assert.equal(a.compare(b), 0);
      assert.equal(a.compare(a), 0);
      a.destroy();
      b.destroy();
    });

    it('reports different pixels', async function () {
      const a = await filled(10, 10, red);
      const b = await filled(10, 10, red);
      b.setPixel(5, 5, blue);

      const result = a.compare(b);
      assert.ok(result & GD_CMP_IMAGE, 'GD_CMP_IMAGE');
      assert.ok(result & GD_CMP_COLOR, 'GD_CMP_COLOR');
      assert.notOk(result & GD_CMP_SIZE_X, 'not GD_CMP_SIZE_X');
      a.destroy();
      b.destroy();
    });

    it('reports a different width and height', async function () {
      const a = await filled(10, 10, red);
      const b = await filled(12, 10, red);
      const c = await filled(10, 12, red);

      assert.ok(a.compare(b) & GD_CMP_SIZE_X, 'GD_CMP_SIZE_X');
      assert.notOk(a.compare(b) & GD_CMP_SIZE_Y, 'not GD_CMP_SIZE_Y');
      assert.ok(a.compare(c) & GD_CMP_SIZE_Y, 'GD_CMP_SIZE_Y');
      a.destroy();
      b.destroy();
      c.destroy();
    });

    it('reports a different image type', async function () {
      const a = await gd.createTrueColor(10, 10);
      const b = await gd.create(10, 10);
      b.colorAllocate(0, 0, 0);

      assert.ok(a.compare(b) & GD_CMP_TRUECOLOR, 'GD_CMP_TRUECOLOR');
      a.destroy();
      b.destroy();
    });

    it('reports a different interlace setting', async function () {
      const a = await filled(10, 10, red);
      const b = await filled(10, 10, red);
      b.interlace = true;

      assert.equal(a.compare(b), GD_CMP_INTERLACE);
      a.destroy();
      b.destroy();
    });

    it('reports a different transparent color', async function () {
      const a = await filled(10, 10, red);
      const b = await filled(10, 10, red);
      b.colorTransparent(red);

      assert.ok(a.compare(b) & GD_CMP_TRANSPARENT, 'GD_CMP_TRANSPARENT');
      a.destroy();
      b.destroy();
    });

    it('reports a different number of palette colors', async function () {
      const a = await gd.create(10, 10);
      a.colorAllocate(0, 0, 0);
      const b = await gd.create(10, 10);
      b.colorAllocate(0, 0, 0);
      b.colorAllocate(255, 255, 255);

      assert.ok(a.compare(b) & GD_CMP_NUM_COLORS, 'GD_CMP_NUM_COLORS');
      a.destroy();
      b.destroy();
    });

    it('throws a TypeError when the argument is not an object', async function () {
      const a = await filled(10, 10, red);
      assert.throws(() => a.compare('image'), TypeError, /must be an Image object/);
      assert.throws(() => a.compare(), Error, /Expected 1 argument/);
      a.destroy();
    });
  });
  describe('Image arguments', function () {
    const methods = {
      copy: (img, arg) => img.copy(arg, 0, 0, 0, 0, 4, 4),
      copyResized: (img, arg) => img.copyResized(arg, 0, 0, 0, 0, 4, 4, 4, 4),
      copyResampled: (img, arg) => img.copyResampled(arg, 0, 0, 0, 0, 4, 4, 4, 4),
      copyRotated: (img, arg) => img.copyRotated(arg, 2, 2, 0, 0, 4, 4, 90),
      copyMerge: (img, arg) => img.copyMerge(arg, 0, 0, 0, 0, 4, 4, 50),
      copyMergeGray: (img, arg) => img.copyMergeGray(arg, 0, 0, 0, 0, 4, 4, 50),
      paletteCopy: (img, arg) => img.paletteCopy(arg),
      colorMatch: (img, arg) => img.colorMatch(arg),
      setBrush: (img, arg) => img.setBrush(arg),
      setTile: (img, arg) => img.setTile(arg),
      compare: (img, arg) => img.compare(arg),
      gifAnimAdd: (img, arg) => img.gifAnimAdd(0, 0, 0, 5, 1, arg),
    };

    for (const [name, call] of Object.entries(methods)) {
      it(`gd.Image#${name}() -- throws a TypeError when the image argument is a plain object`, async function () {
        const img = await filled(4, 4, red);
        assert.throws(() => call(img, {}), TypeError, /must be an Image object/);
        assert.throws(() => call(img, []), TypeError, /must be an Image object/);
        assert.throws(() => call(img, Object.create(gd.Image.prototype)), TypeError, /must be an Image object/);
        img.destroy();
      });

      it(`gd.Image#${name}() -- throws an Error when the image argument is destroyed`, async function () {
        const img = await filled(4, 4, red);
        const destroyed = await filled(4, 4, blue);
        destroyed.destroy();
        assert.throws(() => call(img, destroyed), Error, /already destroyed/);
        img.destroy();
      });
    }
  });
});
