import fs from 'fs';

import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);

var source = currentDir + '/fixtures/';
var target = currentDir + '/output/';

describe('Section Handling TIFF files', function () {
  it('gd.openTiff() -- can open a tiff and save it as a jpg', async function () {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      return this.skip();
    }
    s = source + 'input.tif';
    t = target + 'output-from-tiff.jpg';

    const img = await gd.openTiff(s);
    await img.saveJpeg(t, 100);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('gd.Image#saveTiff() -- can open a jpg file and save it as a tiff', async function () {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      return this.skip();
    }
    s = source + 'input.jpg';
    t = target + 'output-from-jpg.tiff';

    const img = await gd.openJpeg(s);
    await img.saveTiff(t);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('gd.createFromTiff() -- can open a tiff and save it as a tiff', async function () {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      return this.skip();
    }
    s = source + 'input.tif';
    t = target + 'output-from-tiff.tif';

    var image = await gd.createFromTiff(s);
    await image.saveTiff(t);
    assert.ok(fs.existsSync(t));
    image.destroy();
  });

  it('gd.createFromTiffPtr() -- can open a tif and store it in a pointer and save a tiff from the pointer', async function () {
    if (gd.getGDVersion() < '2.2.4') {
      return this.skip();
    }
    var s = source + 'input.tif';
    var t = target + 'output-from-tiff-ptr.tif';

    var imageData = fs.readFileSync(s);
    var image = gd.createFromTiffPtr(imageData);
    await image.saveTiff(t);
    assert.ok(fs.existsSync(t));
    image.destroy();
  });

  it('gd.Image#saveTiff() -- can create a truecolor Tiff image with text', async function () {
    var f, img, t, txtColor;
    if (gd.getGDVersion() < '2.2.4' || !gd.GD_TIFF) {
      return this.skip();
    }
    f = source + 'FreeSans.ttf';
    t = target + 'output-truecolor-string.tif';
    img = await gd.createTrueColor(120, 20);
    txtColor = img.colorAllocate(255, 255, 0);
    img.stringFT(txtColor, f, 16, 0, 8, 18, 'Hello world!');
    await img.saveTiff(t);
    assert.ok(fs.existsSync(t));
  });
});
