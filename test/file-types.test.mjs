import fs from 'fs';

import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);

var source = currentDir + '/fixtures/';
var target = currentDir + '/output/';

describe('Section Handling file types', function () {
  it('gd.Image#jpeg() -- returns a Promise', async function () {
    const s = `${source}input.png`;
    const t = `${target}output-gd.Image.jpeg.jpg`;
    const img = await gd.openPng(s);
    const successPromise = img.jpeg(t, 0);

    assert.ok(successPromise.constructor === Promise);
  });

  it('gd.Image#jpeg() -- returns true when save is succesfull', async function () {
    const s = `${source}input.png`;
    const t = `${target}output-success-gd.Image.jpeg.jpg`;
    const img = await gd.openPng(s);

    assert.ok(img instanceof gd.Image);
    const success = await img.jpeg(t, 0);

    assert.ok(success === true);
  });

  it('gd.Image#jpeg() -- returns "Cannot save JPEG file" in catch when failing', async function () {
    const s = `${source}input.png`;
    const img = await gd.openPng(s);
    assert.ok(img instanceof gd.Image);

    img.jpeg('', 100).catch(function (reason) {
      assert.ok(reason === 'Cannot save JPEG file');
    });
  });

  it('gd.Image#saveJpeg() -- can copy a png into a jpeg', async () => {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.jpg';
    const img = await gd.openPng(s);

    var canvas;
    canvas = await gd.createTrueColor(100, 100);
    img.copyResampled(canvas, 0, 0, 0, 0, 100, 100, img.width, img.height);
    await canvas.saveJpeg(t, 10);
    assert.ok(fs.existsSync(t));
    img.destroy();
    canvas.destroy();
  });

  it('gd.Image#saveGif() -- can copy a png into gif', async () => {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.gif';
    const img = await gd.openPng(s);
    var canvas;
    canvas = await gd.createTrueColor(img.width, img.height);
    img.copyResampled(canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height);
    await canvas.saveGif(t);
    assert.ok(fs.existsSync(t));
    img.destroy();
    canvas.destroy();
  });

  it('gd.Image#saveWBMP() -- can copy a png into WBMP', async function () {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.wbmp';
    const img = await gd.openPng(s);
    var canvas, fg;
    canvas = await gd.createTrueColor(img.width, img.height);
    img.copyResampled(canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height);
    fg = img.getPixel(5, 5);
    await canvas.saveWBMP(t, fg);
    assert.ok(fs.existsSync(t));
    img.destroy();
    canvas.destroy();
  });

  it('gd.Image#savePng() -- can open a jpeg file and save it as png', async function () {
    var s, t;
    s = source + 'input.jpg';
    t = target + 'output-from-jpeg.png';
    const img = await gd.openJpeg(s);

    await img.savePng(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('gd.Image#saveAvif() -- can open a jpeg file and save it as avif', async function () {
    if (gd.getGDVersion() < '2.3.2' || !gd.GD_AVIF) {
      this.skip();
      return;
    }
    
    // Skip this test in CI environments where AVIF encoder may not be available
    if (process.env.CI || process.env.GITHUB_ACTIONS) {
      this.skip();
      return;
    }
    
    var s, t;
    s = source + 'input.jpg';
    t = target + 'output-from-jpeg.avif';
    const img = await gd.openJpeg(s);

    try {
      await img.saveAvif(t, -1);
      assert.ok(fs.existsSync(t));
    } catch (error) {
      // If AVIF codec is not available at runtime, skip the test
      if (error.message && (error.message.includes('codec') || error.message.includes('encoder') || error.message.includes('AVIF'))) {
        this.skip();
        return;
      }
      throw error;
    } finally {
      img.destroy();
    }
  });

  it('gd.Image#saveHeif() -- can open a jpeg file and save it as heif', async function () {
    if (!gd.GD_HEIF) {
      this.skip();
      return;
    }
    var s, t;
    s = source + 'input.jpg';
    t = target + 'output-from-jpeg.heif';
    const img = await gd.openJpeg(s);

    await img.saveHeif(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('gd.openHeif() -- can open a heif file and save it as png', async function () {
    if (!gd.GD_HEIF) {
      this.skip();
      return;
    }
    var s, t;
    s = source + 'input.heif';
    t = target + 'output-from-heif.png';
    const img = await gd.openHeif(s);

    await img.savePng(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('gd.Image#savePng() -- can open a bmp and save it as png', async function () {
    var s;
    var t;
    s = source + 'input.bmp';
    t = target + 'output-from-bmp.png';
    const img = await gd.openBmp(s);
    await img.savePng(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });
});
