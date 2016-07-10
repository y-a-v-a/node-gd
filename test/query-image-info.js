var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

describe('Section Querying image information', function() {
  it('can query true color alpha values of a color', function(done) {
    gd.createTrueColor(100, 100, function(error, image) {
      var someColor = gd.trueColorAlpha(63, 255, 191, 63);
      if (error) {
        throw error;
      }
      assert.equal(image.red(someColor), 63);
      assert.equal(image.green(someColor), 255);
      assert.equal(image.blue(someColor), 191);
      assert.equal(image.alpha(someColor), 63);
      done();
    });
  });

  it('can query true color values of a color', function(done) {
    gd.createTrueColor(100, 100, function(error, image) {
      var someColor = gd.trueColor(63, 255, 191, 63);
      if (error) {
        throw error;
      }
      assert.equal(image.red(someColor), 63);
      assert.equal(image.green(someColor), 255);
      assert.equal(image.blue(someColor), 191);
      done();
    });
  });

  it('can query palette color values of a color with alpha', function(done) {
    gd.create(100, 100, function(error, image) {
      var someColor = image.colorAllocateAlpha(63, 255, 191, 63);
      if (error) {
        throw error;
      }
      assert.equal(image.red(someColor), 63);
      assert.equal(image.green(someColor), 255);
      assert.equal(image.blue(someColor), 191);
      assert.equal(image.alpha(someColor), 63);
      done();
    });
  });

  it('can query palette color values of a color', function(done) {
    gd.create(100, 100, function(error, image) {
      var someColor = image.colorAllocate(63, 255, 191);
      if (error) {
        throw error;
      }
      assert.equal(image.red(someColor), 63);
      assert.equal(image.green(someColor), 255);
      assert.equal(image.blue(someColor), 191);
      done();
    });
  });

});
