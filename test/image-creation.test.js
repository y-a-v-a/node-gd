'use strict';

var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

/**
 * gd.create
 * ╦┌┬┐┌─┐┌─┐┌─┐  ┌─┐┬─┐┌─┐┌─┐┌┬┐┬┌─┐┌┐┌
 * ║│││├─┤│ ┬├┤   │  ├┬┘├┤ ├─┤ │ ││ ││││
 * ╩┴ ┴┴ ┴└─┘└─┘  └─┘┴└─└─┘┴ ┴ ┴ ┴└─┘┘└┘
 */
describe('Creating a paletted image', function() {
  it('can be done synchronously', async () => {
    var img = await gd.create(100, 100);

    assert.ok(img instanceof gd.Image);
    img.destroy();
  });

  it('throws an Error when too few arguments are supplied', async () => {
    var img;
    try {
      img = await gd.create(100);
    } catch (e) {
      assert.ok(e instanceof Error);
    }
  });

  it('throws an Error when argument is not a Number - NaN', async () => {
    var img;
    try {
      img = await gd.create(NaN, 100);
    } catch (e) {
      assert.ok(e instanceof Error);
    }
  });

  it('throws an Error when argument is not a Number - Infinity', async () => {
    var img;
    try {
      img = await gd.create(Infinity, 100);
    } catch (e) {
      assert.ok(e instanceof Error);
    }
  });

  it('throws an TypeError when the first argument if of wrong type', async () => {
    var img;
    try {
      img = await gd.create('bogus', undefined);
    } catch (e) {
      assert.ok(e instanceof TypeError);
    }
  });

  it('throws an TypeError when the second argument if of wrong type', async () => {
    var img;
    try {
      img = await gd.create(100, 'bogus');
    } catch (e) {
      assert.ok(e instanceof TypeError);
    }
  });


  it('throws a RangeError when the width parameter is 0', async () => {
    var img;
    try {
      img = await gd.create(0, 100);
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('throws a RangeError when the height parameter is 0', async () => {
    var img;
    try {
      img = await gd.create(100, 0);
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('throws a RangeError when the height parameter is a negative value', async () => {
    var img;
    try {
      img = await gd.create(100, -10);
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('throws an Error when creating an image without width and height - gd.create()', async () => {
    try {
      gd.create();
    } catch (exception) {
      assert.ok(exception instanceof Error);
    }
  });

  it('throws a Error when the height parameter is 0 async', async () => {
    var img;
    try {
      gd.create(100, 0, function(error, img) { });
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('returns an object containing basic information about the created image', async () => {
    var img = await gd.create(100, 100);

    assert.equal(img.width, 100);
    assert.equal(img.height, 100);
    assert.equal(img.trueColor, 0);

    img.destroy();
  });

  it('returns an object containing basic information about the created image async', async () => {
    gd.create(100, 100, function(error, image) {
      if (error) {
        throw error;
      }

      assert.equal(image.width, 100);
      assert.equal(image.height, 100);
      assert.equal(image.trueColor, 0);

      image.destroy();
    });
  });
});

/**
 * gd.createTrueColor and await gd.createTrueColor
 */
describe('Create a true color image', function() {
  it('can be done synchronously.', async () => {
    var img = await gd.createTrueColor(100, 100);
    assert.ok(img instanceof gd.Image);
    img.destroy();
  });

  it('throws an Error when too few arguments are supplied', async () => {
    var img;
    try {
      img = await gd.createTrueColor(100);
    } catch (e) {
      assert.ok(e instanceof Error);
    }
  });

  it('throws an TypeError when the first argument if of wrong type', async () => {
    var img;
    try {
      img = await gd.createTrueColor('bogus', undefined);
    } catch (e) {
      assert.ok(e instanceof TypeError);
    }
  });

  it('throws an TypeError when the second argument if of wrong type', async () => {
    var img;
    try {
      img = await gd.createTrueColor(100, 'bogus');
    } catch (e) {
      assert.ok(e instanceof TypeError);
    }
  });

  it('can be done asynchronously.', async () => {
    var img = await gd.createTrueColor(10, 10);
    assert.ok(img instanceof gd.Image);
    img.destroy();

  });

  it('throws a RangeError when the width parameter is 0', async () => {
    var img;
    try {
      img = await gd.createTrueColor(0, 100);
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('throws a RangeError when the height parameter is 0', async () => {
    var img;
    try {
      img = await gd.createTrueColor(100, 0);
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('returns an object containing basic information about the created image', async () => {
    var img = await gd.createTrueColor(100, 100);
    assert.ok(img.width === 100 && img.height === 100 && img.trueColor === 1);
    img.destroy();
  });
});
