var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

describe('Section Handling file types', function() {
  it('can copy a png into a jpeg - gd.Image#saveJpeg()', function(done) {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.jpg';
    return gd.openPng(s, function(err, img) {
      var canvas;
      if (err) {
        throw err;
      }
      canvas = gd.createTrueColorSync(100, 100);
      img.copyResampled(canvas, 0, 0, 0, 0, 100, 100, img.width, img.height);
      return canvas.saveJpeg(t, 10, function(err) {
        if (err) {
          throw err;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        canvas.destroy();
        return done();
      });
    });
  });

  it('can copy a png into gif - gd.Image#saveGif()', function(done) {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.gif';
    return gd.openPng(s, function(err, img) {
      var canvas;
      if (err) {
        throw err;
      }
      canvas = gd.createTrueColorSync(img.width, img.height);
      img.copyResampled(canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height);
      return canvas.saveGif(t, function(err) {
        if (err) {
          throw err;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        canvas.destroy();
        return done();
      });
    });
  });

  it('can copy a png into gd - gd.Image#saveGd()', function(done) {
    var s, t;
    s = source + 'input.png';
    t = target + 'output.gd';
    return gd.openPng(s, function(err, img) {
      var canvas;
      if (err) {
        throw err;
      }
      canvas = gd.createTrueColorSync(img.width, img.height);
      img.copyResampled(canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height);
      return canvas.saveGd(t, function(err) {
        if (err) {
          throw err;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        canvas.destroy();
        return done();
      });
    });
  });

  it('can copy a png into WBMP - gd.Image#saveWBMP()', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output.wbmp';
    gd.openPng(s, function(err, img) {
      var canvas, fg;
      if (err) {
        throw err;
      }
      canvas = gd.createTrueColorSync(img.width, img.height);
      img.copyResampled(canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height);
      fg = img.getPixel(5, 5);
      return canvas.saveWBMP(t, fg, function(err) {
        if (err) {
          throw err;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        canvas.destroy();
        return done();
      });
    });
  });

  it('can open a jpeg file and save it as png - gd.Image#savePng()', function(done) {
    var s, t;
    s = source + 'input.jpg';
    t = target + 'output-from-jpeg.png';
    return gd.openJpeg(s, function(err, img) {
      if (err) {
        throw err;
      }
      return img.savePng(t, -1, function(err) {
        if (err) {
          throw err;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        return done();
      });
    });
  });

  it('can open a bmp and save it as png - gd.Image#savePng()', function(done) {
    var s;
    var t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.bmp';
    t = target + 'output-from-bmp.png';
    return gd.openBmp(s, function(err, img) {
      if (err) {
        throw err;
      }
      return img.savePng(t, -1, function(err) {
        if (err) {
          throw err;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        return done();
      });
    });
  });
});
