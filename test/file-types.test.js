'use strict';

var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

describe('Section Handling file types', function() {
  it('can copy a png into a jpeg - gd.Image#saveJpeg()', async () => {
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

  it('can copy a png into gif - gd.Image#saveGif()', async () => {
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

  it('can copy a png into gd - gd.Image#saveGd()', async () => {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.gd';
    const img = await gd.openPng(s);

    var canvas;
    canvas = await gd.createTrueColor(img.width, img.height);
    img.copyResampled(canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height);
    await canvas.saveGd(t);
    assert.ok(fs.existsSync(t));
    img.destroy();
    canvas.destroy();
  });

  it('can copy a png into WBMP - gd.Image#saveWBMP()', async function() {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      this.skip();
      return;
    }
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

  it('can open a jpeg file and save it as png - gd.Image#savePng()', async function() {
    var s, t;
    s = source + 'input.jpg';
    t = target + 'output-from-jpeg.png';
    const img = await gd.openJpeg(s);

    await img.savePng(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('can open a bmp and save it as png - gd.Image#savePng()', async function() {
    var s;
    var t;
    if (gd.getGDVersion() < '2.1.1') {
      this.skip();
      return;
    }
    s = source + 'input.bmp';
    t = target + 'output-from-bmp.png';
    const img = await gd.openBmp(s);
    await img.savePng(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });
});
