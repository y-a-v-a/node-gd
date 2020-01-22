const gd = require('../index');
const assert = require('chai').assert;

describe("gd.Image#colormatch", function() {
  it("throws error when `this` image is not truecolor", async function() {
    const baseImage = await gd.create(100,100);
    const paletteImage = await gd.create(100,100);

    try {
      baseImage.colorMatch(paletteImage);
    } catch(e) {
      assert.instanceOf(e, Error);
    }
  });

  it('throws an Error when argument image is not palette', async function() {
    const baseImage = await gd.create(100,100);
    const trueColorImg = await gd.createTrueColor(100,100);

    try {
      baseImage.colorMatch(trueColorImg);
    } catch(e) {
      assert.instanceOf(e, Error);
    }
  });

  it('expects images to have same dimensions', async function() {
    const baseImage = await gd.createTrueColor(100,100);
    const paletteImage = await gd.create(90,90);

    try {
      baseImage.colorMatch(paletteImage);
    } catch(e) {
      baseImage.destroy();
      paletteImage.destroy();
      assert.instanceOf(e, Error);
    }
  });

  it ('expects the palette iamge to have at least one color allocated',  async function() {
    const baseImage = await gd.createTrueColor(100,100);
    const paletteImage = await gd.create(100,100);

    try {
      baseImage.colorMatch(paletteImage);
    } catch(e) {
      baseImage.destroy();
      paletteImage.destroy();
      assert.instanceOf(e, Error);
    }
  });

  it ('can match palette colors to truecolor image',  async function() {
    const baseImage = await gd.openJpeg(`${__dirname}/fixtures/input.jpg`);
    const paletteImage = await gd.openGif(`${__dirname}/fixtures/node-gd.gif`);

    const result = baseImage.colorMatch(paletteImage);
    assert.equal(result, 0);

    await paletteImage.saveGif(`${__dirname}/output/colorMatch.gif`);
    baseImage.destroy();
    paletteImage.destroy();
  });
});
