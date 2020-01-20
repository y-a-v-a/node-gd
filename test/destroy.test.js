const gd = require('../index');
const assert = require('chai').assert;

describe("Image destroy", function() {
  it("gd.Image#destroy() -- accessing 'width' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.strictEqual(img.width, 200);
    assert.strictEqual(img.height, 200);
    assert.instanceOf(img, gd.Image, 'Object not instance of gd.Image');
    img.destroy();

    try {
      img.width;
    } catch(e) {
      assert.instanceOf(e, Error);
    }
  });

  it("gd.Image#destroy() -- accessing 'height' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.strictEqual(img.height, 200);
    img.destroy();

    try {
      img.height;
    } catch(e) {
      assert.ok(e instanceof Error);
    }
  });

  it("gd.Image#destroy() -- accessing 'trueColor' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.strictEqual(img.trueColor, 0);
    img.destroy();

    try {
      img.trueColor;
    } catch(e) {
      assert.ok(e instanceof Error);
    }
  });

  it("gd.Image#destroy() -- accessing 'trueColor' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.strictEqual(img.trueColor, 0);
    img.destroy();

    try {
      img.getPixel(1, 1);
    } catch(e) {
      assert.ok(e instanceof Error);
    }
  });

  // it("gd.Image#destroy() -- ", async function() {

  // });
});
