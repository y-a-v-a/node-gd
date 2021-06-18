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
});
