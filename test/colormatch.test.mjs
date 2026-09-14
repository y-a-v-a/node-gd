import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

describe('gd.Image#colormatch', function () {
  it('throws error when `this` image is not truecolor', async function () {
    const baseImage = await gd.create(100, 100);
    const paletteImage = await gd.create(100, 100);

    assert.throws(() => baseImage.colorMatch(paletteImage), Error, /should be truecolor/);
    baseImage.destroy();
    paletteImage.destroy();
  });

  it('throws an Error when argument image is not palette', async function () {
    const baseImage = await gd.createTrueColor(100, 100);
    const trueColorImg = await gd.createTrueColor(100, 100);

    assert.throws(() => baseImage.colorMatch(trueColorImg), Error, /must be palette/);
    baseImage.destroy();
    trueColorImg.destroy();
  });

  it('expects images to have same dimensions', async function () {
    const baseImage = await gd.createTrueColor(100, 100);
    const paletteImage = await gd.create(90, 90);
    paletteImage.colorAllocate(0, 0, 0);

    assert.throws(() => baseImage.colorMatch(paletteImage), Error, /same dimensions/);
    baseImage.destroy();
    paletteImage.destroy();
  });

  it('expects the palette iamge to have at least one color allocated', async function () {
    const baseImage = await gd.createTrueColor(100, 100);
    const paletteImage = await gd.create(100, 100);

    assert.throws(() => baseImage.colorMatch(paletteImage), Error, /At least 1 color/);
    baseImage.destroy();
    paletteImage.destroy();
  });

  it('can match palette colors to truecolor image', async function () {
    const currentDir = dirname(import.meta.url);
    const baseImage = await gd.openJpeg(`${currentDir}/fixtures/input.jpg`);
    const paletteImage = await gd.openGif(`${currentDir}/fixtures/node-gd.gif`);

    const result = baseImage.colorMatch(paletteImage);
    assert.equal(result, 0);

    await paletteImage.saveGif(`${currentDir}/output/colorMatch.gif`);
    baseImage.destroy();
    paletteImage.destroy();
  });
});
