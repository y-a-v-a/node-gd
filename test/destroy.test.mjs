import gd from '../index.js';
import { assert } from 'chai';

describe('Image destroy', function () {
  it("gd.Image#destroy() -- accessing 'width' property after destroy throws an Error", async function () {
    const img = await gd.create(200, 200);
    assert.strictEqual(img.width, 200);
    assert.strictEqual(img.height, 200);
    assert.instanceOf(img, gd.Image, 'Object not instance of gd.Image');
    img.destroy();

    assert.throws(() => img.width, Error, /already destroyed/);
  });

  it("gd.Image#destroy() -- accessing 'height' property after destroy throws an Error", async function () {
    const img = await gd.create(200, 200);
    assert.strictEqual(img.height, 200);
    img.destroy();

    assert.throws(() => img.height, Error, /already destroyed/);
  });

  it("gd.Image#destroy() -- accessing 'trueColor' property after destroy throws an Error", async function () {
    const img = await gd.create(200, 200);
    assert.strictEqual(img.trueColor, 0);
    img.destroy();

    assert.throws(() => img.trueColor, Error, /already destroyed/);
  });

  it("gd.Image#destroy() -- calling 'getPixel' after destroy throws an Error", async function () {
    const img = await gd.create(200, 200);
    assert.strictEqual(img.trueColor, 0);
    img.destroy();

    assert.throws(() => img.getPixel(1, 1), Error, /already destroyed/);
  });

  it("gd.Image#destroy() -- setting 'interlace' after destroy throws an Error", async function () {
    const img = await gd.create(200, 200);
    img.destroy();

    assert.throws(() => {
      img.interlace = true;
    }, Error, /already destroyed/);
  });

  it("gd.Image#destroy() -- setting 'interpolationId' after destroy throws an Error", async function () {
    const img = await gd.createTrueColor(200, 200);
    img.destroy();

    assert.throws(() => {
      img.interpolationId = 16;
    }, Error, /already destroyed/);
  });

  // it("gd.Image#destroy() -- ", async function() {

  // });
});
