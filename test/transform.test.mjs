import gd from '../index.js';
import { assert } from 'chai';

/**
 * Values of libgd constants which are not exported by node-gd
 */
const GD_CROP_DEFAULT = 0;
const GD_CROP_TRANSPARENT = 1;
const GD_CROP_BLACK = 2;
const GD_CROP_WHITE = 3;
const GD_CROP_SIDES = 4;
const GD_BILINEAR_FIXED = 3;
const GD_NEAREST_NEIGHBOUR = 16;
const GD_PIXELATE_UPPERLEFT = 0;
const GD_PIXELATE_AVERAGE = 1;

const red = gd.trueColor(255, 0, 0);
const green = gd.trueColor(0, 255, 0);
const blue = gd.trueColor(0, 0, 255);
const white = gd.trueColor(255, 255, 255);
const black = gd.trueColor(0, 0, 0);

/**
 * Create a 20x10 true color image with a distinct color in each corner
 */
async function cornerImage() {
  const img = await gd.createTrueColor(20, 10);
  img.filledRectangle(0, 0, 19, 9, white);
  img.setPixel(0, 0, red);
  img.setPixel(19, 0, green);
  img.setPixel(0, 9, blue);
  img.setPixel(19, 9, black);
  return img;
}

describe('Image transformations', function () {
  describe('gd.Image#flipHorizontal(), #flipVertical() and #flipBoth()', function () {
    it('flips an image horizontally', async function () {
      const img = await cornerImage();
      assert.strictEqual(img.flipHorizontal(), img);
      assert.equal(img.getTrueColorPixel(0, 0), green);
      assert.equal(img.getTrueColorPixel(19, 0), red);
      assert.equal(img.getTrueColorPixel(0, 9), black);
      assert.equal(img.getTrueColorPixel(19, 9), blue);
      img.destroy();
    });

    it('flips an image vertically', async function () {
      const img = await cornerImage();
      assert.strictEqual(img.flipVertical(), img);
      assert.equal(img.getTrueColorPixel(0, 0), blue);
      assert.equal(img.getTrueColorPixel(19, 0), black);
      assert.equal(img.getTrueColorPixel(0, 9), red);
      assert.equal(img.getTrueColorPixel(19, 9), green);
      img.destroy();
    });

    it('flips an image both horizontally and vertically', async function () {
      const img = await cornerImage();
      assert.strictEqual(img.flipBoth(), img);
      assert.equal(img.getTrueColorPixel(0, 0), black);
      assert.equal(img.getTrueColorPixel(19, 0), blue);
      assert.equal(img.getTrueColorPixel(0, 9), green);
      assert.equal(img.getTrueColorPixel(19, 9), red);
      img.destroy();
    });

    it('restores the original image when flipped twice', async function () {
      const img = await cornerImage();
      const original = await cornerImage();
      img.flipHorizontal().flipHorizontal();
      assert.equal(img.compare(original), 0);
      img.destroy();
      original.destroy();
    });
  });

  describe('gd.Image#crop()', function () {
    it('returns a new image of the cropped area', async function () {
      const img = await cornerImage();
      const cropped = img.crop(15, 5, 5, 5);

      assert.instanceOf(cropped, gd.Image);
      assert.notStrictEqual(cropped, img);
      assert.equal(cropped.width, 5);
      assert.equal(cropped.height, 5);
      assert.equal(cropped.getTrueColorPixel(4, 4), black);
      assert.equal(img.width, 20, 'original is untouched');
      cropped.destroy();
      img.destroy();
    });

    it('only copies pixels within the image bounds when the crop area is larger', async function () {
      const img = await cornerImage();
      const cropped = img.crop(10, 5, 50, 50);

      assert.equal(cropped.width, 50);
      assert.equal(cropped.height, 50);
      assert.equal(cropped.getTrueColorPixel(5, 2), white, 'copied from source');
      assert.equal(cropped.getTrueColorPixel(20, 20), 0, 'area outside source stays empty');
      cropped.destroy();
      img.destroy();
    });

    it('keeps the image type of the source', async function () {
      const img = await gd.create(20, 20);
      img.colorAllocate(0, 0, 0);
      const cropped = img.crop(0, 0, 10, 10);
      assert.equal(cropped.trueColor, 0);
      cropped.destroy();
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await cornerImage();
      assert.throws(() => img.crop(0, 0, 10), Error, /Expected 4 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#cropAuto()', function () {
    async function framed(background) {
      const img = await gd.createTrueColor(50, 40);
      img.filledRectangle(0, 0, 49, 39, background);
      img.filledRectangle(10, 5, 29, 24, red);
      return img;
    }

    it('crops a white border with GD_CROP_WHITE', async function () {
      const img = await framed(white);
      const cropped = img.cropAuto(GD_CROP_WHITE);
      assert.equal(cropped.width, 20);
      assert.equal(cropped.height, 20);
      assert.equal(cropped.getTrueColorPixel(0, 0), red);
      cropped.destroy();
      img.destroy();
    });

    it('crops a black border with GD_CROP_BLACK', async function () {
      const img = await framed(black);
      const cropped = img.cropAuto(GD_CROP_BLACK);
      assert.equal(cropped.width, 20);
      assert.equal(cropped.height, 20);
      cropped.destroy();
      img.destroy();
    });

    it('crops a border of the corner color with GD_CROP_SIDES', async function () {
      const img = await framed(blue);
      const cropped = img.cropAuto(GD_CROP_SIDES);
      assert.equal(cropped.width, 20);
      assert.equal(cropped.height, 20);
      cropped.destroy();
      img.destroy();
    });

    it('crops a border of the transparent color with GD_CROP_TRANSPARENT', async function () {
      const transparent = gd.trueColorAlpha(0, 0, 0, 127);
      const img = await gd.createTrueColor(50, 40);
      img.alphaBlending(0);
      img.filledRectangle(0, 0, 49, 39, transparent);
      img.filledRectangle(10, 5, 29, 24, red);

      const uncropped = img.cropAuto(GD_CROP_TRANSPARENT);
      assert.equal(uncropped.width, 50, 'nothing cropped without a transparent color');

      img.colorTransparent(transparent);
      const cropped = img.cropAuto(GD_CROP_TRANSPARENT);
      assert.equal(cropped.width, 20);
      assert.equal(cropped.height, 20);
      uncropped.destroy();
      cropped.destroy();
      img.destroy();
    });

    it('returns null when the whole image is the border color', async function () {
      const img = await gd.createTrueColor(20, 20);
      img.filledRectangle(0, 0, 19, 19, white);
      assert.isNull(img.cropAuto(GD_CROP_WHITE));
      assert.isNull(img.cropAuto(GD_CROP_SIDES));
      img.destroy();
    });

    it('returns an uncropped copy with GD_CROP_DEFAULT when no transparent color is set', async function () {
      const img = await gd.createTrueColor(20, 20);
      img.filledRectangle(0, 0, 19, 19, white);
      const cropped = img.cropAuto(GD_CROP_DEFAULT);
      assert.notStrictEqual(cropped, img);
      assert.equal(cropped.width, 20);
      assert.equal(cropped.height, 20);
      cropped.destroy();
      img.destroy();
    });

    it('throws a RangeError for an unsupported mode', async function () {
      const img = await framed(white);
      assert.throws(() => img.cropAuto(5), RangeError, /Crop mode/);
      img.destroy();
    });

    it('throws a TypeError when mode is not a Number', async function () {
      const img = await framed(white);
      assert.throws(() => img.cropAuto('white'), TypeError);
      img.destroy();
    });
  });

  describe('gd.Image#cropThreshold()', function () {
    it('crops a border of a color within a threshold', async function () {
      const img = await gd.createTrueColor(50, 40);
      img.filledRectangle(0, 0, 49, 39, gd.trueColor(250, 250, 250));
      img.filledRectangle(0, 0, 49, 2, white);
      img.filledRectangle(10, 5, 29, 24, red);

      const cropped = img.cropThreshold(white, 0.5);
      assert.equal(cropped.width, 20);
      assert.equal(cropped.height, 20);
      cropped.destroy();
      img.destroy();
    });

    it('throws a TypeError when threshold is missing', async function () {
      const img = await cornerImage();
      assert.throws(() => img.cropThreshold(white), TypeError, /Argument 1 must be a Number/);
      img.destroy();
    });
  });

  describe('gd.Image#scale()', function () {
    it('returns a new image with the requested dimensions', async function () {
      const img = await cornerImage();
      const scaled = img.scale(40, 30);

      assert.instanceOf(scaled, gd.Image);
      assert.equal(scaled.width, 40);
      assert.equal(scaled.height, 30);
      assert.equal(img.width, 20, 'original is untouched');
      scaled.destroy();
      img.destroy();
    });

    it('uses the interpolation method of the image', async function () {
      const img = await gd.createTrueColor(2, 1);
      img.setPixel(0, 0, red).setPixel(1, 0, blue);
      img.interpolationId = GD_NEAREST_NEIGHBOUR;

      const scaled = img.scale(8, 4);
      const colors = new Set();
      for (let x = 0; x < 8; x++) {
        colors.add(scaled.getTrueColorPixel(x, 2));
      }
      assert.deepEqual([...colors].sort(), [blue, red].sort(), 'no blended colors');
      scaled.destroy();
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await cornerImage();
      assert.throws(() => img.scale(10), Error, /Expected 2 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#interpolationId', function () {
    it('defaults to GD_BILINEAR_FIXED', async function () {
      const img = await gd.createTrueColor(10, 10);
      assert.equal(img.interpolationId, GD_BILINEAR_FIXED);
      img.destroy();
    });

    it('can be set to another interpolation method', async function () {
      const img = await gd.createTrueColor(10, 10);
      img.interpolationId = GD_NEAREST_NEIGHBOUR;
      assert.equal(img.interpolationId, GD_NEAREST_NEIGHBOUR);
      img.destroy();
    });

    it('throws an Error when set to a value higher than 30', async function () {
      const img = await gd.createTrueColor(10, 10);
      assert.throws(() => {
        img.interpolationId = 31;
      }, Error, /cannot be higher than 30/);
      img.destroy();
    });
  });

  describe('gd.Image#rotateInterpolated()', function () {
    it('returns a new rotated image', async function () {
      const img = await cornerImage();
      const rotated = img.rotateInterpolated(90, white);

      assert.instanceOf(rotated, gd.Image);
      assert.equal(rotated.width, 10);
      assert.equal(rotated.height, 20);
      assert.equal(img.width, 20, 'original is untouched');
      rotated.destroy();
      img.destroy();
    });

    it('grows the canvas for non right angles', async function () {
      const img = await cornerImage();
      const rotated = img.rotateInterpolated(45, white);

      assert.isAbove(rotated.width, 20);
      assert.isAbove(rotated.height, 10);
      rotated.destroy();
      img.destroy();
    });

    it('throws an Error when the background color is missing', async function () {
      const img = await cornerImage();
      assert.throws(() => img.rotateInterpolated(45), Error, /Expected 2 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#pixelate()', function () {
    it('fills each block with its upper left color using GD_PIXELATE_UPPERLEFT', async function () {
      const img = await gd.createTrueColor(8, 8);
      img.filledRectangle(0, 0, 7, 7, white);
      img.setPixel(0, 0, red);
      img.setPixel(4, 4, blue);

      assert.strictEqual(img.pixelate(4, GD_PIXELATE_UPPERLEFT), img);
      assert.equal(img.getTrueColorPixel(3, 3), red);
      assert.equal(img.getTrueColorPixel(7, 7), blue);
      assert.equal(img.getTrueColorPixel(4, 0), white);
      img.destroy();
    });

    it('fills each block with the average color using GD_PIXELATE_AVERAGE', async function () {
      const img = await gd.createTrueColor(2, 2);
      img.setPixel(0, 0, white).setPixel(1, 0, white);
      img.setPixel(0, 1, black).setPixel(1, 1, black);

      img.pixelate(2, GD_PIXELATE_AVERAGE);
      const pixel = img.getTrueColorPixel(0, 0);
      assert.equal(pixel, img.getTrueColorPixel(1, 1));
      assert.closeTo(img.red(pixel), 127, 1);
      img.destroy();
    });

    it('throws a TypeError when mode is missing', async function () {
      const img = await cornerImage();
      assert.throws(() => img.pixelate(4), TypeError, /Argument 1 must be a Number/);
      img.destroy();
    });
  });

  describe('Filters', function () {
    it('gd.Image#negate() -- inverts colors', async function () {
      const img = await cornerImage();
      assert.strictEqual(img.negate(), img);
      assert.equal(img.getTrueColorPixel(5, 5), black);
      assert.equal(img.getTrueColorPixel(0, 0), gd.trueColor(0, 255, 255));
      img.destroy();
    });

    it('gd.Image#grayscale() -- removes color information', async function () {
      const img = await cornerImage();
      assert.strictEqual(img.grayscale(), img);
      const pixel = img.getTrueColorPixel(0, 0);
      assert.equal(img.red(pixel), img.green(pixel));
      assert.equal(img.green(pixel), img.blue(pixel));
      img.destroy();
    });

    it('gd.Image#brightness() -- brightens and darkens an image', async function () {
      const img = await gd.createTrueColor(2, 2);
      img.filledRectangle(0, 0, 1, 1, gd.trueColor(100, 100, 100));

      img.brightness(50);
      assert.equal(img.red(img.getTrueColorPixel(0, 0)), 150);
      img.brightness(-100);
      assert.equal(img.red(img.getTrueColorPixel(0, 0)), 50);
      img.destroy();
    });

    it('gd.Image#brightness() -- throws a TypeError when the value is not a Number', async function () {
      const img = await gd.createTrueColor(2, 2);
      assert.throws(() => img.brightness('50'), TypeError);
      img.destroy();
    });

    it('gd.Image#contrast() -- a negative value increases contrast', async function () {
      const img = await gd.createTrueColor(2, 1);
      img.setPixel(0, 0, gd.trueColor(100, 100, 100));
      img.setPixel(1, 0, gd.trueColor(160, 160, 160));

      img.contrast(-50);
      assert.isBelow(img.red(img.getTrueColorPixel(0, 0)), 100);
      assert.isAbove(img.red(img.getTrueColorPixel(1, 0)), 160);
      img.destroy();
    });

    it('gd.Image#sharpen() -- increases the difference at edges', async function () {
      const img = await gd.createTrueColor(9, 1);
      img.filledRectangle(0, 0, 4, 0, gd.trueColor(100, 100, 100));
      img.filledRectangle(5, 0, 8, 0, gd.trueColor(150, 150, 150));

      assert.strictEqual(img.sharpen(100), img);
      assert.isBelow(img.red(img.getTrueColorPixel(4, 0)), 100);
      assert.isAbove(img.red(img.getTrueColorPixel(5, 0)), 150);
      img.destroy();
    });

    it('gd.Image#sharpen() -- throws a TypeError when percentage is missing', async function () {
      const img = await gd.createTrueColor(2, 2);
      assert.throws(() => img.sharpen(), TypeError);
      img.destroy();
    });

    it('gd.Image#gaussianBlur() -- smooths hard edges', async function () {
      const img = await gd.createTrueColor(9, 9);
      img.filledRectangle(0, 0, 8, 8, black);
      img.setPixel(4, 4, white);

      assert.strictEqual(img.gaussianBlur(), img);
      assert.isBelow(img.red(img.getTrueColorPixel(4, 4)), 255);
      assert.isAbove(img.red(img.getTrueColorPixel(4, 3)), 0);
      img.destroy();
    });

    it('gd.Image#emboss() and gd.Image#selectiveBlur() -- return the image instance', async function () {
      const img = await cornerImage();
      assert.strictEqual(img.emboss(), img);
      assert.strictEqual(img.selectiveBlur(), img);
      img.destroy();
    });
  });

  describe('Palette and true color conversion', function () {
    it('gd.Image#createPaletteFromTrueColor() -- returns a new palette image', async function () {
      const img = await cornerImage();
      const palette = img.createPaletteFromTrueColor(0, 16);

      assert.instanceOf(palette, gd.Image);
      assert.equal(palette.trueColor, 0);
      assert.isAtMost(palette.colorsTotal, 16);
      assert.equal(palette.width, 20);
      assert.equal(img.trueColor, 1, 'original is untouched');
      palette.destroy();
      img.destroy();
    });

    it('gd.Image#createPaletteFromTrueColor() -- defaults to 256 colors without dithering', async function () {
      const img = await cornerImage();
      const palette = img.createPaletteFromTrueColor();
      assert.equal(palette.trueColor, 0);
      assert.isAtMost(palette.colorsTotal, 256);
      // quantization may shift colors slightly. Decompose the true color
      // value manually: palette.red() expects a palette index, not a color.
      const pixel = palette.getTrueColorPixel(0, 0);
      assert.closeTo((pixel >> 16) & 0xff, 255, 8);
      assert.closeTo((pixel >> 8) & 0xff, 0, 8);
      assert.closeTo(pixel & 0xff, 0, 8);
      palette.destroy();
      img.destroy();
    });

    it('gd.Image#trueColorToPalette() -- converts the image in place', async function () {
      const img = await cornerImage();
      const result = img.trueColorToPalette(1, 8);

      assert.equal(result, 1);
      assert.equal(img.trueColor, 0);
      assert.isAtMost(img.colorsTotal, 8);
      img.destroy();
    });

    it('gd.Image#paletteToTrueColor() -- converts the image in place', async function () {
      const img = await gd.create(10, 10);
      img.colorAllocate(255, 255, 255);
      const paletteRed = img.colorAllocate(255, 0, 0);
      img.setPixel(3, 3, paletteRed);

      assert.equal(img.paletteToTrueColor(), 1);
      assert.equal(img.trueColor, 1);
      assert.equal(img.getPixel(3, 3), red);
      assert.equal(img.getPixel(0, 0), white);
      img.destroy();
    });

    it('gd.Image#paletteToTrueColor() -- is a no-op for true color images', async function () {
      const img = await cornerImage();
      assert.equal(img.paletteToTrueColor(), 1);
      assert.equal(img.trueColor, 1);
      img.destroy();
    });

    it('gd.Image#createPaletteFromTrueColor() -- throws a TypeError for invalid arguments', async function () {
      const img = await cornerImage();
      assert.throws(() => img.createPaletteFromTrueColor('yes'), TypeError);
      img.destroy();
    });
  });

  describe('gd.Image#squareToCircle()', function () {
    it('returns a new square image of twice the radius', async function () {
      const img = await gd.createTrueColor(50, 50);
      img.filledRectangle(0, 0, 49, 49, red);

      const circle = img.squareToCircle(20);
      assert.instanceOf(circle, gd.Image);
      assert.equal(circle.width, 40);
      assert.equal(circle.height, 40);
      circle.destroy();
      img.destroy();
    });

    it('returns null for non square images', async function () {
      const img = await cornerImage();
      assert.isNull(img.squareToCircle(10));
      img.destroy();
    });

    it('throws a TypeError when radius is missing', async function () {
      const img = await gd.createTrueColor(50, 50);
      assert.throws(() => img.squareToCircle(), TypeError);
      img.destroy();
    });
  });
});
