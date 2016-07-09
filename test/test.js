var fs = require('fs');
var path = require('path');
var util = require('util');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

before(function() {

  console.log('Built on top of GD version: ' + gd.getGDVersion() + '\n\n');

  return fs.readdir(target, function(err, files) {
    return files.forEach(function(file, idx) {
      if (file.substr(0, 6) === 'output') {
        return fs.unlink(target + file, function(err) {
          if (err) {
            throw err;
          }
        });
      }
    });
  });
});

describe('Node.js GD Graphics Library', function() {
  /**
   * gd.create and gd.createSync
   * ╦┌┬┐┌─┐┌─┐┌─┐  ┌─┐┬─┐┌─┐┌─┐┌┬┐┬┌─┐┌┐┌
   * ║│││├─┤│ ┬├┤   │  ├┬┘├┤ ├─┤ │ ││ ││││
   * ╩┴ ┴┴ ┴└─┘└─┘  └─┘┴└─└─┘┴ ┴ ┴ ┴└─┘┘└┘
   */
  describe('Creating a paletted image', function() {
    it('can be done synchronously.', function(done) {
      var img = gd.createSync(100,100);
      assert.ok(img instanceof gd.Image);
      img.destroy();
      return done();
    });

    it('throws an Error when too few arguments are supplied', function(done) {
      var img;
      try {
        img = gd.createSync(100);
      } catch (e) {
        assert.ok(e instanceof Error);
        return done();
      }
    });

    it('throws an TypeError when the first argument if of wrong type', function(done) {
      var img;
      try {
        img = gd.createSync('bogus', undefined);
      } catch (e) {
        assert.ok(e instanceof TypeError);
        return done();
      }
    });

    it('throws an TypeError when the second argument if of wrong type', function(done) {
      var img;
      try {
        img = gd.createSync(100, 'bogus');
      } catch (e) {
        assert.ok(e instanceof TypeError);
        return done();
      }
    });

    it('can be done asynchronously.', function(done) {
      return gd.create(10, 10, function(error, img) {
        assert.ok(img instanceof gd.Image);
        img.destroy();

        return done();
      });
    });

    it('throws a RangeError when the width parameter is 0', function(done) {
      var img;
      try {
        img = gd.createSync(0, 100);
      } catch (e) {
        assert.ok(e instanceof RangeError);
        return done();
      }
    });

    it('throws a RangeError when the height parameter is 0', function(done) {
      var img;
      try {
        img = gd.createSync(100, 0);
      } catch (e) {
        assert.ok(e instanceof RangeError);
        return done();
      }
    });

    it('returns an object containing basic information about the created image', function(done) {
      var img = gd.createSync(100, 100);
      assert.ok(img.width === 100 && img.height === 100 && img.trueColor === 0);
      img.destroy();
      return done();
    });
  });

  /**
   * gd.createTrueColor and gd.createTrueColorSync
   */
  describe('Create a true color image', function() {
    it('can be done synchronously.', function(done) {
      var img = gd.createTrueColorSync(100,100);
      assert.ok(img instanceof gd.Image);
      img.destroy();
      return done();
    });

    it('throws an Error when too few arguments are supplied', function(done) {
      var img;
      try {
        img = gd.createTrueColorSync(100);
      } catch (e) {
        assert.ok(e instanceof Error);
        return done();
      }
    });

    it('throws an TypeError when the first argument if of wrong type', function(done) {
      var img;
      try {
        img = gd.createTrueColorSync('bogus', undefined);
      } catch (e) {
        assert.ok(e instanceof TypeError);
        return done();
      }
    });

    it('throws an TypeError when the second argument if of wrong type', function(done) {
      var img;
      try {
        img = gd.createTrueColorSync(100, 'bogus');
      } catch (e) {
        assert.ok(e instanceof TypeError);
        return done();
      }
    });

    it('can be done asynchronously.', function(done) {
      return gd.createTrueColor(10, 10, function(error, img) {
        assert.ok(img instanceof gd.Image);
        img.destroy();

        return done();
      });
    });

    it('throws a RangeError when the width parameter is 0', function(done) {
      var img;
      try {
        img = gd.createTrueColorSync(0, 100);
      } catch (e) {
        assert.ok(e instanceof RangeError);
        return done();
      }
    });

    it('throws a RangeError when the height parameter is 0', function(done) {
      var img;
      try {
        img = gd.createTrueColorSync(100, 0);
      } catch (e) {
        assert.ok(e instanceof RangeError);
        return done();
      }
    });

    it('returns an object containing basic information about the created image', function(done) {
      var img = gd.createTrueColorSync(100, 100);
      assert.ok(img.width === 100 && img.height === 100 && img.trueColor === 1);
      img.destroy();
      return done();
    });
  });


  describe('GD color functions', function() {
    // it('', function(done) {});

    it('can return an integer representation of rgb color values', function(done) {
      var red = gd.trueColor(255, 0, 0);
      assert.ok(16711680 === red);
      return done();
    });

    it('can return an integer representation of rgba color values', function(done) {
      var transparentRed = gd.trueColorAlpha(255, 0, 0, 63);
      assert.ok(1073676288 === transparentRed);
      return done();
    });
  });

  describe('From the gd.Image query functions,', function() {
    it('getBoundsSafe should return 0 if the coordinate [-10, 1000] is checked against the image bounds.', function(done) {
      var s = source + 'input.png';
      var coord = [-10, 1000];
      return gd.openPng(s, function(error, image) {
        if (error) {
          throw error;
        }

        assert.ok(image.getBoundsSafe(coord[0], coord[1]) === 0);
        image.destroy();
        return done();
      });
    });

    it('getBoundsSafe should return 1 if the coordinate [10, 10] is checked against the image bounds.', function(done) {
      var s = source + 'input.png';
      var coord = [10, 10];
      return gd.openPng(s, function(error, image) {
        if (error) {
          throw error;
        }

        assert.ok(image.getBoundsSafe(coord[0], coord[1]) === 1);
        image.destroy();
        return done();
      });
    });

    it('getTrueColorPixel should return "e6e6e6" when queried for coordinate [10, 10].', function(done) {
      var s = source + 'input.png';
      var coord = [10, 10];
      return gd.openPng(s, function(error, image) {
        var color;
        if (error) {
          throw error;
        }
        color = image.getTrueColorPixel(coord[0], coord[1]);

        assert.ok(color.toString(16) === 'e6e6e6');
        return done();
      });
    });

    it('getTrueColorPixel should return 0 when queried for coordinate [101, 101].', function(done) {
      var s = source + 'input.png';
      var coord = [101, 101];
      return gd.openPng(s, function(error, image) {
        var color;
        if (error) {
          throw error;
        }
        color = image.getTrueColorPixel(coord[0], coord[1]);

        assert.ok(color === 0);
        return done();
      });
    });

    it('imageColorAt should return "be392e" when queried for coordinate [50, 50].', function(done) {
      var s = source + 'input.png';
      var coord = [50, 50];
      return gd.openPng(s, function(error, image) {
        var color;
        if (error) {
          throw error;
        }
        color = image.imageColorAt(coord[0], coord[1]);

        assert.ok(color.toString(16) === 'be392e');
        return done();
      });
    });

    it('imageColorAt should throw an error when queried for coordinate [101, 101].', function(done) {
      var s = source + 'input.png';
      var coord = [101, 101];
      return gd.openPng(s, function(error, image) {
        var color;
        if (error) {
          throw error;
        }
        try {
          color = image.imageColorAt(coord[0], coord[1]);
        } catch (exception) {
          assert.ok(exception instanceof Error);

          return done();
        }
      });
    });
  });

  it('can scale-down (resize) an image', function(done) {
    var s, t;
    s = source + 'input.png';
    t = target + 'output-scale.png';
    return gd.openPng(s, function(err, img) {
      var canvas, h, scale, w;
      if (err) {
        throw err;
      }
      scale = 2;
      w = Math.floor(img.width / scale);
      h = Math.floor(img.height / scale);
      canvas = gd.createTrueColorSync(w, h);
      img.copyResampled(canvas, 0, 0, 0, 0, w, h, img.width, img.height);
      return canvas.savePng(t, 1, function(err) {
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
  it('can rotate an image', function(done) {
    var s, t;
    s = source + 'input.png';
    t = target + 'output-rotate.png';
    return gd.openPng(s, function(err, img) {
      var canvas, h, w;
      if (err) {
        throw err;
      }
      w = 100;
      h = 100;
      canvas = gd.createTrueColorSync(w, h);
      img.copyRotated(canvas, 50, 50, 0, 0, img.width, img.height, 45);
      return canvas.savePng(t, 1, function(err) {
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
  it('can convert to grayscale', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-grayscale.png';
    return gd.openPng(s, function(err, img) {
      if (err) {
        throw err;
      }
      img.grayscale();
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

  it('can add gaussian blur to an image', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-gaussianblur.png';
    return gd.openPng(s, function(err, img) {
      var i, j;
      if (err) {
        throw err;
      }
      for (i = j = 0; j < 10; i = ++j) {
        img.gaussianBlur();
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
  it('can negate an image', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-negate.png';
    return gd.openPng(s, function(err, img) {
      if (err) {
        throw err;
      }
      img.negate();
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
  it('can change brightness of an image', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-brightness.png';
    return gd.openPng(s, function(err, img) {
      var brightness;
      if (err) {
        throw err;
      }
      brightness = Math.floor(Math.random() * 100);
      img.brightness(brightness);
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
  it('can change contrast of an image', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-contrast.png';
    return gd.openPng(s, function(err, img) {
      var contrast;
      if (err) {
        throw err;
      }
      contrast = Math.floor(Math.random() * 2000) - 900;
      img.contrast(contrast);
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
  it('can emboss an image', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-emboss.png';
    return gd.openPng(s, function(err, img) {
      if (err) {
        throw err;
      }
      img.emboss();
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
  it('can apply selective blur to an image', function(done) {
    var s, t;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    s = source + 'input.png';
    t = target + 'output-selectiveBlur.png';
    return gd.openPng(s, function(err, img) {
      if (err) {
        throw err;
      }
      img.selectiveBlur();
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
  it('can replace a color to another color', function(done) {
    var img, s, t;
    s = source + 'input.png';
    t = target + 'output-replaced.png';
    return gd.openPng(s, function(error, image) {
      if (error) throw error;

      var colors = [
        image.getTrueColorPixel(10,10),
        image.getTrueColorPixel(10,11),
        image.getTrueColorPixel(10,12),
        image.getTrueColorPixel(10,13),
        image.getTrueColorPixel(10,14),
        image.getTrueColorPixel(10,15)
      ];
      var colorTo = gd.trueColor(0,255,255);

      for (var i = 0; i < colors.length; i++) {
        image.colorReplace(colors[i], colorTo);
      }

      image.savePng(t, 0, function(error) {
        if (error) throw error;

        assert.ok(fs.existsSync(t));
        image.destroy();
        return done();
      });
    });
  });
  it('can create a truecolor BMP image with text', function(done) {
    var f, img, t, txtColor;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    f = source + 'FreeSans.ttf';
    t = target + 'output-truecolor-string.bmp';
    img = gd.createTrueColorSync(120, 20);
    txtColor = img.colorAllocate(255, 255, 0);
    img.stringFT(txtColor, f, 16, 0, 8, 18, "Hello world!");
    return img.saveBmp(t, 0, function(err) {
      if (err) {
        throw err;
      }
      assert.ok(fs.existsSync(t));
      return done();
    });
  });
  it('can create a truecolor Tiff image with text', function(done) {
    var f, img, t, txtColor;
    if (gd.getGDVersion() < '2.1.1') {
      done();
      return;
    }
    f = source + 'FreeSans.ttf';
    t = target + 'output-truecolor-string.tif';
    img = gd.createTrueColorSync(120, 20);
    txtColor = img.colorAllocate(255, 255, 0);
    img.stringFT(txtColor, f, 16, 0, 8, 18, "Hello world!");
    return img.saveTiff(t, function(err) {
      if (err) {
        throw err;
      }
      assert.ok(fs.existsSync(t));
      return done();
    });
  });


  describe('section Handling file types', function() {
    it('can copy a png into a jpeg', function(done) {
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
    it('can copy a png into gif', function(done) {
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
    it('can copy a png into gd', function(done) {
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
    it('can copy a png into WBMP', function(done) {
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
    it('can open a jpeg file and save it as png', function(done) {
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
    it('throws an Error when creating an image without width and height', function(done) {
      try {
        gd.create();
      } catch (exception) {
        assert.ok(exception instanceof Error);
        return done();
      }
    });
    return it('can open a bmp and save it as png', function(done) {
      var s, t;
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


  /*
   * ┌┬┐┌─┐┌┬┐┌─┐┬─┐┬ ┬  ┬  ┌─┐┌─┐┬┌─┌─┐
   * │││├┤ ││││ │├┬┘└┬┘  │  ├┤ ├─┤├┴┐└─┐
   * ┴ ┴└─┘┴ ┴└─┘┴└─ ┴   ┴─┘└─┘┴ ┴┴ ┴└─┘
   */
  describe('section Memory leaks', function() {
    it('sync openJpeg and then destroy doesn\'t leak', function(done) {
      var s;
      var oldMem;
      var newMem;
      var iterations = 150;
      s = source + 'input.jpg';
      if(global.gc) {
        // iterate image loading to memory and destroying it, GC and new stats
        for (var i = 0; i < iterations; i++) {
          var img = gd.openJpeg(s);
          img.destroy();
          global.gc(1); // GC scavenge mode
        };
        for (var i = 0; i < 10; i++) {
          global.gc(1); // GC scavenge mode
        }
        for (var i = 0; i < 10; i++) {
          global.gc();
        }
        oldMem = process.memoryUsage();
        // iterate image loading to memory and destroying it, GC and new stats
        for (var i = 0; i < iterations; i++) {
          var img = gd.openJpeg(s);
          img.destroy();
          global.gc(1); // GC scavenge mode
        };
        for (var i = 0; i < 10; i++) {
          global.gc(1); // GC scavenge mode
        }
        for (var i = 0; i < 10; i++) {
          global.gc();
        }
        newMem = process.memoryUsage();
        if((newMem.heapUsed > (oldMem.heapUsed + 100*1024)) || (newMem.rss > (oldMem.rss + 100*1024)) || (newMem.heapTotal > (oldMem.heapTotal + 100*1024))){ // consider quadruple the image in the memory size be ok
            var error = new Error("Memory leaks.\nrss delta: " + (newMem.rss - oldMem.rss) + "\nheapTotal delta: " + (newMem.heapTotal - oldMem.heapTotal) + "\nheapUsed delta: " + (newMem.heapUsed - oldMem.heapUsed));
            done(error);
        }else{
          done();
        }
      }else{
        done();
      }
    });
  });

});
