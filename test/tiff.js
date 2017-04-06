var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

describe('Section Handling TIFF files', function() {
  it('can open a tiff and save it as a jpg', function(done) {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      done();
      return;
    }
    s = source + 'input.tif';
    t = target + 'output-from-tiff.jpg';

    return gd.openTiff(s, function(error, img) {
      if (error) {
        throw error;
      }
      return img.saveJpeg(t, 100, function(error) {
        if (error) {
          throw error;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        return done();
      })
    });
  });

  it('can open a jpg file and save it as a tiff', function(done) {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      done();
      return;
    }
    s = source + 'input.jpg';
    t = target + 'output-from-jpg.tiff';

    return gd.openJpeg(s, function(error, img) {
      if (error) {
        throw error;
      }
      return img.saveTiff(t, function(error) {
        if (error) {
          throw error;
        }
        assert.ok(fs.existsSync(t));
        img.destroy();
        return done();
      });
    });
  });

  it('can open a tiff and save it as a tiff', function(done) {
    var s;
    var t;
    if (gd.getGDVersion() < '2.2.4') {
      done();
      return;
    }
    s = source + 'input.tif';
    t = target + 'output-from-tiff.tif';

    var image = gd.createFromTiff(s);
    return image.saveTiff(t, function(error) {
      if (error) {
        throw error;
      }
      assert.ok(fs.existsSync(t));
      image.destroy();
      return done();
    })
  });

  it('can open a tif and store it in a pointer and save a tiff from the pointer', function(done) {
    if (gd.getGDVersion() < '2.2.4') {
      done();
      return;
    }
    var s = source + 'input.tif';
    var t = target + 'output-from-tiff-ptr.tif';

    var imageData = fs.readFileSync(s);
    var image = gd.createFromTiffPtr(imageData);
    return image.saveTiff(t, function(error) {
      if (error) {
        throw error;
      }
      assert.ok(fs.existsSync(t));
      image.destroy();
      return done();
    });

  });
});
