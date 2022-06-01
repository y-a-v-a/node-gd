import fs from 'fs';

import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);

var source = currentDir + '/fixtures';

const s = source + '/input.jpg';

describe('gd.createFromJpegPtr - Creating image from Buffer', function () {
  it('should not accept a String', function (done) {
    const imageAsString = fs.readFile(s, function (error, data) {
      if (error) {
        throw error;
      }

      assert.throws(
        function () {
          gd.createFromJpegPtr(data.toString('utf8'));
        },
        TypeError,
        /Argument not a Buffer/
      );

      done();
    });
  });

  it('should accept a Buffer', function (done) {
    const imageData = fs.readFile(s, function (error, data) {
      if (error) {
        throw error;
      }

      const img = gd.createFromJpegPtr(data);

      assert.ok(img instanceof gd.Image);
      img.destroy();
      done();
    });
  });

  it('should not accept a Number', function (done) {
    assert.throws(
      function () {
        gd.createFromJpegPtr(1234567890);
      },
      TypeError,
      /Argument not a Buffer/
    );
    done();
  });
});
