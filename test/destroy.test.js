const gd = require('../index');
const assert = require('chai').assert;

describe("Image destroy", function() {
  it("gd.Image#destroy() -- accessing 'width' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.ok(img.width === 200);
    img.destroy();

    try {
      img.width;
    } catch(e) {
      assert.ok(e instanceof Error);
    }
  });

  it("gd.Image#destroy() -- accessing 'height' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.ok(img.height === 200);
    img.destroy();

    try {
      img.height;
    } catch(e) {
      assert.ok(e instanceof Error);
    }
  });

  it("gd.Image#destroy() -- accessing 'trueColor' property after destroy throws an Error", async function() {
    const img = await gd.create(200,200);
    assert.ok(img.trueColor === 0);
    img.destroy();

    try {
      img.trueColor;
    } catch(e) {
      assert.ok(e instanceof Error);
    }
  });

  // it("gd.Image#destroy() -- ", async function() {

  // });
});
