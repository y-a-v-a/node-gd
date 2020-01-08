'use strict';

var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

describe('Section Handling TIFF files', function() {
  it('can open a tiff and save it as a jpg', async function() {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      this.skip();
    }
    s = source + 'input.tif';
    t = target + 'output-from-tiff.jpg';

    const img = await gd.openTiff(s);
    await img.saveJpeg(t, 100);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('can open a jpg file and save it as a tiff', async function() {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      this.skip();
      return;
    }
    s = source + 'input.jpg';
    t = target + 'output-from-jpg.tiff';

    const img = await gd.openJpeg(s);
    await img.saveTiff(t);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('can open a tiff and save it as a tiff', async function() {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      this.skip();
      return;
    }
    s = source + 'input.tif';
    t = target + 'output-from-tiff.tif';

    var image = await gd.createFromTiff(s);
    await image.saveTiff(t);
    assert.ok(fs.existsSync(t));
    image.destroy();
  });

  it('can open a tif and store it in a pointer and save a tiff from the pointer', async function() {
    if (gd.getGDVersion() < '2.2.4') {
      this.skip();
      return;
    }
    var s = source + 'input.tif';
    var t = target + 'output-from-tiff-ptr.tif';

    var imageData = fs.readFileSync(s);
    var image = gd.createFromTiffPtr(imageData);
    await image.saveTiff(t);
    assert.ok(fs.existsSync(t));
    image.destroy();
  });
});
