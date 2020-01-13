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
describe('gd.create - Creating a paletted image', function() {
  it('returns a Promise', () => {
    const imagePromise = gd.create(100,100);
    assert.ok(imagePromise.constructor === Promise);

    imagePromise.then(image => image.destroy());
  });

  it('can be done', async () => {
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

  it('throws a RangeError when the height parameter is a fraction value', async () => {
    var img;
    try {
      img = await gd.create(100.5, 101.6);
    } catch (e) {
      assert.ok(e instanceof RangeError);
    }
  });

  it('throws an Error when creating an image without width and height', async () => {
    try {
      await gd.create();
    } catch (exception) {
      assert.ok(exception instanceof Error);
    }
  });

  it('throws a Error when the height parameter is 0 async', async () => {
    var img;
    try {
      await gd.create(100, 0);
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
});

/**
 * gd.createTrueColor and await gd.createTrueColor
 */
describe('gd.createTrueColor - Create a true color image', function() {
  it('returns a Promise', () => {
    const imagePromise = gd.createTrueColor(101,101);

    assert.ok(imagePromise.constructor === Promise);

    imagePromise.then(image => image.destroy());
  });

  it('returns a Promise that resolves to an Image', async function() {
    const imagePromise = gd.createTrueColor(101,101);

    imagePromise.then(image => {
      assert.ok(image.constructor === gd.Image);
    });
  });

  it('can be done', async () => {
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
