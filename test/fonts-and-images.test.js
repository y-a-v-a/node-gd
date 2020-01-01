'use strict';

var fs = require('fs');
var path = require('path');
var util = require('util');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

var fontFile = source + 'FreeSans.ttf';

/**
 * Text / fonts in images
 * ╔═╗┌─┐┌┐┌┌┬┐┌─┐  ┬┌┐┌  ┬┌┬┐┌─┐┌─┐┌─┐┌─┐
 * ╠╣ │ ││││ │ └─┐  ││││  ││││├─┤│ ┬├┤ └─┐
 * ╚  └─┘┘└┘ ┴ └─┘  ┴┘└┘  ┴┴ ┴┴ ┴└─┘└─┘└─┘
 */
describe('Creating images containing text', function() {
  it('can create an image with text', async () => {
    var img, t, txtColor;
    t = target + 'output-string.png';

    img = await gd.create(200, 80);
    img.colorAllocate(0, 255, 0);
    txtColor = img.colorAllocate(255, 0, 255);

    img.stringFT(txtColor, fontFile, 24, 0, 10, 60, "Hello world");

    return img.savePng(t, 1, function(err) {
      if (err) {
        throw err;
      }

      assert.ok(fs.existsSync(t));
      img.destroy();
    });
  });

  it('can create a truecolor image with text', async () => {
    var img, t, txtColor;
    t = target + 'output-truecolor-string.png';
    img = await gd.createTrueColor(120, 20);
    txtColor = img.colorAllocate(255, 255, 0);
    img.stringFT(txtColor, fontFile, 16, 0, 8, 18, "Hello world!");
    return img.savePng(t, 1, function(err) {
      if (err) {
        throw err;
      }
      assert.ok(fs.existsSync(t));
      img.destroy();
    });
  });

  it('can return the coordinates of the bounding box of a string', async () => {
    var t = target + 'output-truecolor-string-2.png';

    var img = await gd.createTrueColor(300, 300);
    var txtColor = img.colorAllocate(127, 90, 90);
    var boundingBox = img.stringFT(txtColor, fontFile, 16, 0, 8, 18, "Hello World2!", true);

    assert.equal(boundingBox.length, 8, 'BoundingBox not eight coordinates?');
    assert.deepEqual(boundingBox, [ 8, 20, 136, 20, 136, 0, 8, 0 ], 'BoundingBox size changed?');

    img.destroy();
  });

  it('can return the coordinates of the bounding box of a string using a specific function', async () => {
    var t = target + 'output-truecolor-string-2.png';

    var img = await gd.createTrueColor(300, 300);
    var txtColor = img.colorAllocate(127, 90, 90);
    var boundingBox = img.stringFTBBox(txtColor, fontFile, 16, -45, 20, 20, "Hello World2!", true);
    assert.equal(boundingBox.length, 8, 'BoundingBox not eight coordinates?');
    assert.deepEqual(boundingBox, [ 18, 21, 85, 130, 101, 120, 34, 11 ], 'BoundingBox size changed?');

    img.destroy();
  });

  it('throws an error when gd.Image#stringFTEx() does not receive an object', async () => {
    var t = target + 'noob.png';
    var image = await gd.createTrueColor(100, 100);
    var txtColor = image.colorAllocate(255, 255, 0);
    var extras = '';

    try {
      image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Lorem ipsum", extras);
    } catch(e) {
      assert.ok(e instanceof Error);
      image.destroy();
    }
  });

  it('can consume an object with font extras', async () => {
    var t = target + 'output-truecolor-string-3.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      linespacing: 1.5,
      hdpi: 300,
      vdpi: 300,
      charmap: 'unicode',
      disable_kerning: false,
      xshow: true,
      return_fontpathname: true,
      use_fontconfig: false,
      fontpath: ''
    };
    var txtColor = image.colorAllocate(255, 255, 0);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Hello world\nYes we're here", extras);

    assert.equal(extras.fontpath, process.cwd() + '/test/fixtures/FreeSans.ttf');
    assert.equal(extras.xshow, '72 53 21 21 53 25 72 53 33 21 -424 68 53 49 25 72 53 20 33 53 25 54 53 33 53');

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('can set the dpi of an image using a font extras object', async () => {
    var t = target + 'output-truecolor-string-300dpi.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      hdpi: 300,
      vdpi: 150
    };
    var txtColor = image.colorAllocate(255, 0, 255);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Font extras\ndpi test", extras);

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('can set the linespacing of text in an image using a font extras object', async () => {
    var t = target + 'output-truecolor-string-linespacing.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      linespacing: 2.1
    };

    var txtColor = image.colorAllocate(0, 255, 255);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Font extras\nlinespacing", extras);

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('can request the kerning table of text in an image using a font extras object', async () => {
    var t = target + 'output-truecolor-string-xshow.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      xshow: true
    };

    var txtColor = image.colorAllocate(0, 255, 255);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Font extras\nxshow", extras);

    assert.equal(extras.xshow, '19.2 16.96 17.28 8.96 8 16.96 15.36 8.96 10.56 17.28 -139.52 15.36 15.68 17.28 16.96 23.04')

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('can disable the use of kerning of text in an image using a font extras object', async () => {
    var t = target + 'output-truecolor-string-disable-kerning.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      disable_kerning: true
    };

    var txtColor = image.colorAllocate(255, 255, 0);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Font extras\nKerning disabled", extras);

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('can return the font path using font extras', async () => {
    var t = target + 'output-truecolor-string-3.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      return_fontpathname: true
    };

    var txtColor = image.colorAllocate(127, 255, 0);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Font extras\nreturn font path", extras);

    assert.equal(extras.fontpath, process.cwd() + '/test/fixtures/FreeSans.ttf');

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('can use a specified charmap to render a font with font extras', async () => {
    var t = target + 'output-truecolor-string-charmap.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      charmap: 'unicode'
    };

    var txtColor = image.colorAllocate(255, 255, 0);
    image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Hello world\nUse unicode!", extras);

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });

  it('throws an error when an unknown charmap is given with font extras', async () => {
    var t = target + 'bogus.png';

    var image = await gd.createTrueColor(300, 300);
    var extras = {
      charmap: 'bogus'
    };

    var txtColor = image.colorAllocate(255, 255, 0);
    try {
      image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Hello world\nUse unicode!", extras);
    } catch(e) {
      assert.ok(e instanceof Error);
      image.destroy();
    }
  });

  it('returns an array of coordinates of the bounding box when an 8th boolean parameter is given to gd.Image#stringFTEx', async () => {
    var t = target + 'bogus.png';
    var image = await gd.createTrueColor(300, 300);
    var extras = {
      hdpi: 120,
      vdpi: 120
    };

    var txtColor = image.colorAllocate(255, 255, 0);

    var boundingBox = image.stringFTEx(txtColor, fontFile, 24, 0, 10, 60, "Hello world\nxshow string!", extras, true);

    assert.equal(boundingBox.length, 8);
    assert.deepEqual(boundingBox, [ 8, 112, 241, 112, 241, 28, 8, 28 ], 'BoundingBox size changed?');
    image.destroy();
  });

  it('can put text on a circle', async () => {
    var t = target + 'output-truecolor-string-circle.png';

    var image = await gd.createTrueColor(300, 300);

    var txtColor = image.colorAllocate(255, 255, 0);
    image.stringFTCircle(150, 150, 100, 32, 1, fontFile, 24, 'Hello', 'world!', txtColor);

    image.savePng(t, 0, function(error) {
      if (error) {
        throw error;
      }
      image.destroy();
    });
  });
});
