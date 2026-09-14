import gd from '../index.js';
import { assert } from 'chai';

/**
 * Drawing primitives
 * Values of libgd constants which are not exported by node-gd
 */
const gdStyled = -2;
const gdBrushed = -3;
const gdTiled = -5;
const gdAntiAliased = -7;
const gdPie = 0;
const gdChord = 1;
const gdNoFill = 2;
const gdEdged = 4;

async function canvas(width = 100, height = 100) {
  const img = await gd.createTrueColor(width, height);
  const white = gd.trueColor(255, 255, 255);
  img.filledRectangle(0, 0, width - 1, height - 1, white);
  return img;
}

describe('Drawing primitives', function () {
  const red = gd.trueColor(255, 0, 0);
  const green = gd.trueColor(0, 255, 0);
  const blue = gd.trueColor(0, 0, 255);
  const white = gd.trueColor(255, 255, 255);
  const black = gd.trueColor(0, 0, 0);

  describe('gd.Image#setPixel()', function () {
    it('sets the color of a single pixel', async function () {
      const img = await canvas();
      img.setPixel(10, 20, red);
      assert.equal(img.getTrueColorPixel(10, 20), red);
      assert.equal(img.getTrueColorPixel(11, 20), white);
      img.destroy();
    });

    it('returns the image instance to allow chaining', async function () {
      const img = await canvas();
      const result = img.setPixel(1, 1, red).setPixel(2, 2, blue);
      assert.strictEqual(result, img);
      assert.equal(img.getTrueColorPixel(2, 2), blue);
      img.destroy();
    });

    it('silently ignores coordinates outside of the image', async function () {
      const img = await canvas(10, 10);
      assert.doesNotThrow(() => img.setPixel(50, 50, red));
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await canvas();
      assert.throws(() => img.setPixel(1, 1), Error, /Expected 3 argument/);
      img.destroy();
    });

    it('throws a TypeError when the color is not a Number', async function () {
      const img = await canvas();
      assert.throws(() => img.setPixel(1, 1, 'red'), TypeError, /Argument 2 must be a Number/);
      img.destroy();
    });
  });

  describe('gd.Image#line()', function () {
    it('draws a horizontal line between two points', async function () {
      const img = await canvas();
      img.line(10, 50, 90, 50, red);
      for (let x = 10; x <= 90; x += 10) {
        assert.equal(img.getTrueColorPixel(x, 50), red, `pixel ${x},50`);
      }
      assert.equal(img.getTrueColorPixel(9, 50), white);
      assert.equal(img.getTrueColorPixel(91, 50), white);
      assert.equal(img.getTrueColorPixel(50, 49), white);
      img.destroy();
    });

    it('draws a diagonal line', async function () {
      const img = await canvas();
      img.line(0, 0, 99, 99, blue);
      assert.equal(img.getTrueColorPixel(0, 0), blue);
      assert.equal(img.getTrueColorPixel(42, 42), blue);
      assert.equal(img.getTrueColorPixel(99, 99), blue);
      assert.equal(img.getTrueColorPixel(99, 0), white);
      img.destroy();
    });

    it('throws a TypeError when a coordinate is not a Number', async function () {
      const img = await canvas();
      assert.throws(() => img.line(0, '0', 10, 10, red), TypeError, /Argument 1 must be a Number/);
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await canvas();
      assert.throws(() => img.line(0, 0, 10, 10), Error, /Expected 5 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#dashedLine()', function () {
    it('draws a line with gaps in it', async function () {
      const img = await canvas();
      img.dashedLine(0, 50, 99, 50, red);
      let colored = 0;
      let blank = 0;
      for (let x = 0; x < 100; x++) {
        const pixel = img.getTrueColorPixel(x, 50);
        if (pixel === red) colored++;
        if (pixel === white) blank++;
      }
      assert.isAbove(colored, 0, 'dashed line has colored pixels');
      assert.isAbove(blank, 0, 'dashed line has gaps');
      assert.equal(colored + blank, 100);
      img.destroy();
    });
  });

  describe('gd.Image#rectangle() and gd.Image#filledRectangle()', function () {
    it('draws the outline of a rectangle', async function () {
      const img = await canvas();
      img.rectangle(10, 10, 50, 40, red);
      assert.equal(img.getTrueColorPixel(10, 10), red);
      assert.equal(img.getTrueColorPixel(50, 40), red);
      assert.equal(img.getTrueColorPixel(30, 10), red);
      assert.equal(img.getTrueColorPixel(10, 25), red);
      assert.equal(img.getTrueColorPixel(30, 25), white, 'inside is not filled');
      img.destroy();
    });

    it('draws a filled rectangle', async function () {
      const img = await canvas();
      img.filledRectangle(10, 10, 50, 40, green);
      assert.equal(img.getTrueColorPixel(10, 10), green);
      assert.equal(img.getTrueColorPixel(30, 25), green);
      assert.equal(img.getTrueColorPixel(50, 40), green);
      assert.equal(img.getTrueColorPixel(51, 41), white);
      img.destroy();
    });

    it('returns the image instance to allow chaining', async function () {
      const img = await canvas();
      assert.strictEqual(img.rectangle(1, 1, 5, 5, red), img);
      assert.strictEqual(img.filledRectangle(1, 1, 5, 5, red), img);
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await canvas();
      assert.throws(() => img.rectangle(1, 1, 5), Error, /Expected 5 argument/);
      assert.throws(() => img.filledRectangle(1, 1, 5), Error, /Expected 5 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#polygon(), #openPolygon() and #filledPolygon()', function () {
    const triangle = [
      { x: 10, y: 90 },
      { x: 50, y: 10 },
      { x: 90, y: 90 },
    ];

    it('draws a closed polygon', async function () {
      const img = await canvas();
      img.polygon(triangle, red);
      // closing edge from last to first point
      assert.equal(img.getTrueColorPixel(50, 90), red);
      assert.equal(img.getTrueColorPixel(50, 10), red);
      assert.equal(img.getTrueColorPixel(50, 60), white, 'inside is not filled');
      img.destroy();
    });

    it('draws an open polygon without the closing edge', async function () {
      const img = await canvas();
      img.openPolygon(triangle, red);
      assert.equal(img.getTrueColorPixel(10, 90), red);
      assert.equal(img.getTrueColorPixel(90, 90), red);
      assert.equal(img.getTrueColorPixel(50, 90), white, 'closing edge not drawn');
      img.destroy();
    });

    it('draws a filled polygon', async function () {
      const img = await canvas();
      img.filledPolygon(triangle, blue);
      assert.equal(img.getTrueColorPixel(50, 60), blue);
      assert.equal(img.getTrueColorPixel(50, 89), blue);
      assert.equal(img.getTrueColorPixel(5, 5), white);
      assert.equal(img.getTrueColorPixel(95, 5), white);
      img.destroy();
    });

    it('returns the image instance to allow chaining', async function () {
      const img = await canvas();
      assert.strictEqual(img.polygon(triangle, red), img);
      assert.strictEqual(img.openPolygon(triangle, red), img);
      assert.strictEqual(img.filledPolygon(triangle, red), img);
      img.destroy();
    });

    it('skips entries that are not points', async function () {
      const withInvalid = [{ x: 40, y: 40 }, 'skip', { y: 5 }, null, { x: 10, y: 40 }];

      for (const method of ['polygon', 'openPolygon', 'filledPolygon']) {
        const img = await canvas(50, 50);
        img[method](withInvalid, red);
        assert.equal(img.getTrueColorPixel(25, 40), red, `${method}: line between valid points`);
        assert.equal(img.getTrueColorPixel(0, 0), white, `${method}: no stray point at origin`);
        assert.equal(img.getTrueColorPixel(25, 20), white, `${method}: nothing drawn elsewhere`);
        img.destroy();
      }
    });

    it('throws a TypeError when a point has non numeric coordinates', async function () {
      const img = await canvas();
      for (const method of ['polygon', 'openPolygon', 'filledPolygon']) {
        assert.throws(() => img[method]([{ x: 1, y: 1 }, { x: '2', y: 2 }], red), TypeError, /numeric x and y/, method);
      }
      img.destroy();
    });

    it('draws nothing for an empty array of points', async function () {
      const img = await canvas();
      for (const method of ['polygon', 'openPolygon', 'filledPolygon']) {
        assert.strictEqual(img[method]([], red), img, method);
      }
      assert.equal(img.getTrueColorPixel(0, 0), white);
      img.destroy();
    });

    it('throws a TypeError when the points are not an array', async function () {
      const img = await canvas();
      assert.throws(() => img.polygon({ x: 1, y: 1 }, red), TypeError, /must be an array/);
      assert.throws(() => img.openPolygon('points', red), TypeError, /must be an array/);
      assert.throws(() => img.filledPolygon(42, red), TypeError, /must be an array/);
      img.destroy();
    });

    it('throws an Error when the color is missing', async function () {
      const img = await canvas();
      assert.throws(() => img.polygon(triangle), Error, /Expected 2 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#arc() and gd.Image#filledArc()', function () {
    it('draws a partial arc', async function () {
      const img = await canvas();
      // 0 degrees is at 3 o'clock, angles increase clockwise
      img.arc(50, 50, 80, 80, 0, 90, red);
      assert.equal(img.getTrueColorPixel(90, 50), red, 'start of arc');
      assert.equal(img.getTrueColorPixel(50, 90), red, 'end of arc');
      assert.equal(img.getTrueColorPixel(10, 50), white, 'left side not drawn');
      assert.equal(img.getTrueColorPixel(50, 10), white, 'top side not drawn');
      img.destroy();
    });

    it('draws a filled pie slice', async function () {
      const img = await canvas();
      img.filledArc(50, 50, 80, 80, 0, 90, green, gdPie);
      assert.equal(img.getTrueColorPixel(60, 60), green);
      assert.equal(img.getTrueColorPixel(40, 40), white);
      img.destroy();
    });

    it('draws a chord outline with gdChord | gdNoFill', async function () {
      const img = await canvas();
      img.filledArc(50, 50, 80, 80, 0, 180, blue, gdChord | gdNoFill);
      // straight line between end points at y = 50
      assert.equal(img.getTrueColorPixel(50, 50), blue);
      assert.equal(img.getTrueColorPixel(50, 70), white, 'chord is not filled');
      img.destroy();
    });

    it('draws edges to the center with gdEdged | gdNoFill', async function () {
      const img = await canvas();
      img.filledArc(50, 50, 80, 80, 0, 90, red, gdEdged | gdNoFill);
      assert.equal(img.getTrueColorPixel(70, 50), red, 'edge to center');
      assert.equal(img.getTrueColorPixel(50, 70), red, 'edge to center');
      assert.equal(img.getTrueColorPixel(62, 62), white, 'not filled');
      img.destroy();
    });

    it('throws an Error when the style argument is missing', async function () {
      const img = await canvas();
      assert.throws(() => img.filledArc(50, 50, 80, 80, 0, 90, red), Error, /Expected 8 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#ellipse() and gd.Image#filledEllipse()', function () {
    it('draws the outline of an ellipse', async function () {
      const img = await canvas();
      img.ellipse(50, 50, 80, 40, red);
      assert.equal(img.getTrueColorPixel(10, 50), red);
      assert.equal(img.getTrueColorPixel(90, 50), red);
      assert.equal(img.getTrueColorPixel(50, 30), red);
      assert.equal(img.getTrueColorPixel(50, 70), red);
      assert.equal(img.getTrueColorPixel(50, 50), white, 'center is not filled');
      img.destroy();
    });

    it('draws a filled ellipse', async function () {
      const img = await canvas();
      img.filledEllipse(50, 50, 80, 40, green);
      assert.equal(img.getTrueColorPixel(50, 50), green);
      assert.equal(img.getTrueColorPixel(20, 50), green);
      assert.equal(img.getTrueColorPixel(50, 15), white);
      assert.equal(img.getTrueColorPixel(12, 32), white, 'corner of bounding box');
      img.destroy();
    });

    it('throws a TypeError when width is not a Number', async function () {
      const img = await canvas();
      assert.throws(() => img.ellipse(50, 50, null, 40, red), TypeError, /Argument 2 must be a Number/);
      img.destroy();
    });
  });

  describe('gd.Image#fill() and gd.Image#fillToBorder()', function () {
    it('flood fills an area of the same color', async function () {
      const img = await canvas();
      img.rectangle(10, 10, 60, 60, red);
      img.fill(30, 30, blue);
      assert.equal(img.getTrueColorPixel(30, 30), blue);
      assert.equal(img.getTrueColorPixel(59, 59), blue);
      assert.equal(img.getTrueColorPixel(10, 10), red, 'border untouched');
      assert.equal(img.getTrueColorPixel(80, 80), white, 'outside untouched');
      img.destroy();
    });

    it('flood fills up to a border of a specific color', async function () {
      const img = await canvas();
      img.rectangle(10, 10, 60, 60, red);
      // a line of a different color inside the area does not stop the fill
      img.line(20, 35, 50, 35, green);
      img.fillToBorder(30, 30, red, blue);
      assert.equal(img.getTrueColorPixel(30, 30), blue);
      assert.equal(img.getTrueColorPixel(30, 50), blue);
      assert.equal(img.getTrueColorPixel(30, 35), blue, 'green line got overwritten');
      assert.equal(img.getTrueColorPixel(10, 10), red, 'border untouched');
      assert.equal(img.getTrueColorPixel(80, 80), white, 'outside untouched');
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await canvas();
      assert.throws(() => img.fill(1, 1), Error, /Expected 3 argument/);
      assert.throws(() => img.fillToBorder(1, 1, red), Error, /Expected 4 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#setThickness()', function () {
    it('draws thicker lines', async function () {
      const img = await canvas();
      img.setThickness(5).line(10, 50, 90, 50, red);
      assert.equal(img.getTrueColorPixel(50, 48), red);
      assert.equal(img.getTrueColorPixel(50, 52), red);
      assert.equal(img.getTrueColorPixel(50, 56), white);
      img.destroy();
    });

    it('throws a TypeError when thickness is not a Number', async function () {
      const img = await canvas();
      assert.throws(() => img.setThickness('5'), TypeError);
      img.destroy();
    });
  });

  describe('gd.Image#setStyle()', function () {
    it('draws lines using a style pattern with the gdStyled color', async function () {
      const img = await canvas();
      img.setStyle([red, red, blue, blue]);
      img.line(0, 10, 99, 10, gdStyled);
      assert.equal(img.getTrueColorPixel(0, 10), red);
      assert.equal(img.getTrueColorPixel(1, 10), red);
      assert.equal(img.getTrueColorPixel(2, 10), blue);
      assert.equal(img.getTrueColorPixel(3, 10), blue);
      assert.equal(img.getTrueColorPixel(4, 10), red);
      img.destroy();
    });

    it('skips style entries that are not numbers', async function () {
      const img = await canvas();
      img.setStyle([red, 'skip', blue]);
      img.line(0, 10, 99, 10, gdStyled);
      assert.equal(img.getTrueColorPixel(0, 10), red);
      assert.equal(img.getTrueColorPixel(1, 10), blue);
      assert.equal(img.getTrueColorPixel(2, 10), red);
      assert.equal(img.getTrueColorPixel(3, 10), blue);
      img.destroy();
    });

    it('throws a TypeError when the style is not an array', async function () {
      const img = await canvas();
      assert.throws(() => img.setStyle(red), TypeError, /must be an array/);
      assert.throws(() => img.setStyle(), TypeError, /must be an array/);
      img.destroy();
    });
  });

  describe('gd.Image#setBrush()', function () {
    it('draws lines using an image as brush with the gdBrushed color', async function () {
      const img = await canvas();
      const brush = await gd.createTrueColor(5, 5);
      brush.filledRectangle(0, 0, 4, 4, green);

      assert.strictEqual(img.setBrush(brush), img);
      img.line(10, 50, 90, 50, gdBrushed);
      assert.equal(img.getTrueColorPixel(50, 50), green);
      assert.equal(img.getTrueColorPixel(50, 52), green, 'brush is 5 pixels high');
      assert.equal(img.getTrueColorPixel(50, 55), white);
      brush.destroy();
      img.destroy();
    });

    it('throws a TypeError when brush is not an image', async function () {
      const img = await canvas();
      assert.throws(() => img.setBrush(), TypeError, /must be an Image object/);
      img.destroy();
    });
  });

  describe('gd.Image#setTile()', function () {
    it('fills shapes using an image as tile with the gdTiled color', async function () {
      const img = await canvas();
      const tile = await gd.createTrueColor(2, 2);
      tile.setPixel(0, 0, red).setPixel(1, 0, blue).setPixel(0, 1, blue).setPixel(1, 1, red);

      assert.strictEqual(img.setTile(tile), img);
      img.filledRectangle(0, 0, 9, 9, gdTiled);
      assert.equal(img.getTrueColorPixel(0, 0), red);
      assert.equal(img.getTrueColorPixel(1, 0), blue);
      assert.equal(img.getTrueColorPixel(0, 1), blue);
      assert.equal(img.getTrueColorPixel(1, 1), red);
      assert.equal(img.getTrueColorPixel(2, 2), red);
      assert.equal(img.getTrueColorPixel(20, 20), white);
      tile.destroy();
      img.destroy();
    });

    it('throws a TypeError when tile is not an image', async function () {
      const img = await canvas();
      assert.throws(() => img.setTile(1), TypeError, /must be an Image object/);
      img.destroy();
    });
  });

  describe('gd.Image#setAntiAliased() and gd.Image#setAntiAliasedDontBlend()', function () {
    it('draws anti-aliased lines with the gdAntiAliased color', async function () {
      const img = await canvas();
      assert.strictEqual(img.setAntiAliased(black), img);
      img.line(0, 0, 99, 30, gdAntiAliased);

      const colors = new Set();
      for (let x = 0; x < 100; x++) {
        for (let y = 0; y < 32; y++) {
          colors.add(img.getTrueColorPixel(x, y));
        }
      }
      // white, black and intermediate grays
      assert.isAbove(colors.size, 2, 'anti-aliasing introduces blended colors');
      img.destroy();
    });

    it('accepts a color that should not be blended', async function () {
      const img = await canvas();
      assert.strictEqual(img.setAntiAliasedDontBlend(black, white), img);
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await canvas();
      assert.throws(() => img.setAntiAliasedDontBlend(black), Error, /Expected 2 argument/);
      img.destroy();
    });
  });

  describe('gd.Image#setClip() and gd.Image#getClip()', function () {
    it('returns the full image as the default clipping rectangle', async function () {
      const img = await canvas(100, 50);
      assert.deepEqual(img.getClip(), { x1: 0, y1: 0, x2: 99, y2: 49 });
      img.destroy();
    });

    it('can set and read back a clipping rectangle', async function () {
      const img = await canvas();
      assert.strictEqual(img.setClip(10, 20, 30, 40), img);
      assert.deepEqual(img.getClip(), { x1: 10, y1: 20, x2: 30, y2: 40 });
      img.destroy();
    });

    it('restricts drawing to the clipping rectangle', async function () {
      const img = await canvas();
      img.setClip(25, 25, 74, 74);
      img.filledRectangle(0, 0, 99, 99, red);
      // reset clip, pixel queries are subject to clipping as well
      img.setClip(0, 0, 99, 99);
      assert.equal(img.getTrueColorPixel(50, 50), red);
      assert.equal(img.getTrueColorPixel(25, 25), red);
      assert.equal(img.getTrueColorPixel(24, 24), white);
      assert.equal(img.getTrueColorPixel(75, 75), white);
      img.destroy();
    });

    it('makes getBoundsSafe() and pixel queries respect the clipping rectangle', async function () {
      const img = await canvas();
      img.setClip(25, 25, 74, 74);
      assert.equal(img.getBoundsSafe(50, 50), 1);
      assert.equal(img.getBoundsSafe(10, 10), 0);
      assert.equal(img.getTrueColorPixel(10, 10), 0);
      img.destroy();
    });

    it('throws an Error when too few arguments are supplied', async function () {
      const img = await canvas();
      assert.throws(() => img.setClip(0, 0, 10), Error, /Expected 4 argument/);
      img.destroy();
    });
  });

  describe('Drawing on a palette image', function () {
    it('uses palette indexes as colors', async function () {
      const img = await gd.create(50, 50);
      const background = img.colorAllocate(255, 255, 255);
      const paletteRed = img.colorAllocate(255, 0, 0);

      img.filledRectangle(5, 5, 20, 20, paletteRed);
      assert.equal(img.getPixel(10, 10), paletteRed);
      assert.equal(img.getPixel(30, 30), background);
      assert.equal(img.getTrueColorPixel(10, 10), red);
      img.destroy();
    });
  });

  describe('Drawing on a destroyed image', function () {
    it('throws an Error', async function () {
      const img = await canvas();
      img.destroy();
      assert.throws(() => img.line(0, 0, 10, 10, red), Error, /already destroyed/);
      assert.throws(() => img.setPixel(0, 0, red), Error, /already destroyed/);
      assert.throws(() => img.getClip(), Error, /already destroyed/);
    });
  });
});
