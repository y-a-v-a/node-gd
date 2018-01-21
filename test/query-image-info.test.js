'use strict';

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

  it('can query the color of a pixel within image bounds', function(done) {
    var s = source + 'input.png';
    gd.openPng(s, function(error, image) {
      var color = image.getTrueColorPixel(0,0);
      assert.isNumber(color, 'got Number for getTrueColorPixel');
      done();
    });
  });

  it('will throw an error when quering the color of a pixel outside of image bounds', function(done) {
    var s = source + 'input.png';
    gd.openPng(s, function(error, image) {
      assert.throws(function() {
        var color = image.getTrueColorPixel(-1,-1);
      }, 'Value for x and y must be greater than 0');

      var color = image.getTrueColorPixel(image.width + 1,1);
      assert.equal(0, color, '0 should be returned when querying above upper bounds');

      done();
    });
  });
});
