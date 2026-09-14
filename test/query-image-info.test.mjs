import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);

var source = currentDir + '/fixtures/';

describe('Section querying image information', function () {
  it('gd.trueColorAlpha() -- can query true color alpha values of a color', async () => {
    var image = await gd.createTrueColor(100, 100);
    var someColor = gd.trueColorAlpha(63, 255, 191, 63);

    assert.equal(image.red(someColor), 63);
    assert.equal(image.green(someColor), 255);
    assert.equal(image.blue(someColor), 191);
    assert.equal(image.alpha(someColor), 63);
  });

  it('gd.trueColor() -- can query true color values of a color', async () => {
    var image = await gd.createTrueColor(100, 100);
    var someColor = gd.trueColor(63, 255, 191, 63);

    assert.equal(image.red(someColor), 63);
    assert.equal(image.green(someColor), 255);
    assert.equal(image.blue(someColor), 191);
  });

  it('gd.Image#colorAllocateAlpha() -- can query palette color values of a color with alpha', async () => {
    var image = await gd.create(100, 100);
    var someColor = image.colorAllocateAlpha(63, 255, 191, 63);

    assert.equal(image.red(someColor), 63);
    assert.equal(image.green(someColor), 255);
    assert.equal(image.blue(someColor), 191);
    assert.equal(image.alpha(someColor), 63);
  });

  it('gd.Image#colorAllocate() -- can query palette color values of a color', async () => {
    var image = await gd.create(100, 100);
    var someColor = image.colorAllocate(63, 255, 191);

    assert.equal(image.red(someColor), 63);
    assert.equal(image.green(someColor), 255);
    assert.equal(image.blue(someColor), 191);
  });

  it('gd.Image#getTrueColorPixel() -- can query the color of a pixel within image bounds', async function () {
    var s = source + 'input.png';
    const image = await gd.openPng(s);
    var color = image.getTrueColorPixel(0, 0);
    assert.isNumber(color, 'got Number for getTrueColorPixel');
  });

  it('gd.Image#getTrueColorPixel() -- will throw an error when quering the color of a pixel outside of image bounds', async function () {
    var s = source + 'input.png';
    const image = await gd.openPng(s);
    assert.throws(function () {
      var color = image.getTrueColorPixel(-1, -1);
    }, 'Value for x and y must be greater than 0');

    var color = image.getTrueColorPixel(image.width + 1, 1);
    assert.equal(
      0,
      color,
      '0 should be returned when querying above upper bounds'
    );
  });
  it('gd.Image#getPixel() and gd.Image#getTrueColorPixel() -- return the pixel on the last row and column', async function () {
    const image = await gd.createTrueColor(10, 10);
    const color = gd.trueColor(1, 2, 3);
    image.setPixel(9, 9, color);

    assert.equal(image.getPixel(9, 9), color);
    assert.equal(image.getTrueColorPixel(9, 9), color);
    image.destroy();
  });

  it('gd.Image#getPixel() and gd.Image#getTrueColorPixel() -- return 0 at and beyond the width and height of a true color image', async function () {
    const image = await gd.createTrueColor(10, 10);
    image.filledRectangle(0, 0, 9, 9, gd.trueColor(1, 2, 3));

    for (const [x, y] of [[10, 0], [0, 10], [10, 10], [50, 50]]) {
      assert.strictEqual(image.getPixel(x, y), 0, `getPixel(${x}, ${y})`);
      assert.strictEqual(image.getTrueColorPixel(x, y), 0, `getTrueColorPixel(${x}, ${y})`);
    }
    image.destroy();
  });

  it('gd.Image#getTrueColorPixel() -- returns 0 at and beyond the width and height of a palette image', async function () {
    const image = await gd.create(10, 10);
    // index 0 is white, so an out of bounds lookup of index 0 would not be 0
    image.colorAllocate(255, 255, 255);

    assert.equal(image.getTrueColorPixel(9, 9), gd.trueColor(255, 255, 255));
    for (const [x, y] of [[10, 0], [0, 10], [10, 10], [50, 50]]) {
      assert.strictEqual(image.getTrueColorPixel(x, y), 0, `getTrueColorPixel(${x}, ${y})`);
      assert.strictEqual(image.getPixel(x, y), 0, `getPixel(${x}, ${y})`);
    }
    image.destroy();
  });
});
