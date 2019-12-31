'use strict';

var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures';
var target = __dirname + '/output/';

const s = source + '/input.jpg';

describe('Creating image from Buffer', function() {
  it('should not accept a String', function(done) {
    const imageAsString = fs.readFile(s, function(error, data) {
      if (error) {
        throw error;
      }

      assert.throws(function() {
        gd.createFromJpegPtr(data.toString('utf8'))
      }, TypeError, /Argument not a Buffer/);

      done();
    });

  });

  it('should accept a Buffer', function(done) {
    const imageData = fs.readFile(s, function(error, data) {
      if (error) {
        throw error;
      }

      const img = gd.createFromJpegPtr(data);

      assert.ok(img instanceof gd.Image);
      img.destroy();
      done();
    });
  });

  it('should not accept a Number', function(done) {
    assert.throws(function() {
      gd.createFromJpegPtr(1234567890);
    }, TypeError, /Argument not a Buffer/);
    done();
  });
});
