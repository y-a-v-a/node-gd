'use strict';

var fs = require('fs');

const gd = require('../index');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

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
    img.copyResampled(
      canvas,
      0,
      0,
      0,
      0,
      img.width,
      img.height,
      img.width,
      img.height
    );
    await canvas.saveGif(t);
    assert.ok(fs.existsSync(t));
    img.destroy();
    canvas.destroy();
  });

  it('gd.Image#saveGd() -- can copy a png into gd', async () => {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.gd';
    const img = await gd.openPng(s);

    var canvas;
    canvas = await gd.createTrueColor(img.width, img.height);
    img.copyResampled(
      canvas,
      0,
      0,
      0,
      0,
      img.width,
      img.height,
      img.width,
      img.height
    );
    await canvas.saveGd(t);
    assert.ok(fs.existsSync(t));
    img.destroy();
    canvas.destroy();
  });

  it('gd.Image#saveWBMP() -- can copy a png into WBMP', async function () {
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
    img.copyResampled(
      canvas,
      0,
      0,
      0,
      0,
      img.width,
      img.height,
      img.width,
      img.height
    );
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
    var s, t;
    s = source + 'input.jpg';
    t = target + 'output-from-jpeg.avif';
    const img = await gd.openJpeg(s);

    await img.saveAvif(t, -1);
    assert.ok(fs.existsSync(t));
    img.destroy();
  });

  it('gd.Image#savePng() -- can open a bmp and save it as png', async function () {
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
