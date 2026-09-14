import gd from '../index.js';
import { assert } from 'chai';

/**
 * gd.create
 * ╦┌┬┐┌─┐┌─┐┌─┐  ┌─┐┬─┐┌─┐┌─┐┌┬┐┬┌─┐┌┐┌
 * ║│││├─┤│ ┬├┤   │  ├┬┘├┤ ├─┤ │ ││ ││││
 * ╩┴ ┴┴ ┴└─┘└─┘  └─┘┴└─└─┘┴ ┴ ┴ ┴└─┘┘└┘
 */
describe('gd.create - Creating a paletted image', function () {
  it('returns a Promise', async () => {
    const imagePromise = gd.create(100, 100);
    assert.strictEqual(imagePromise.constructor, Promise);

    const image = await imagePromise;
    image.destroy();
  });

  it('can be done', async () => {
    var img = await gd.create(100, 100);

    assert.ok(img instanceof gd.Image);
    img.destroy();
  });

  it('can be done sync', async () => {
    var img = gd.createSync(100, 100);

    assert.ok(img instanceof gd.Image);
    img.destroy();
  });

  it('throws Error when accessing instance getter via __proto__', async () => {
    var img = gd.createSync(100, 100);

    assert.throws(() => img.__proto__.width, Error);
    img.destroy();
  });

  it('throws TypeError when accessing prototype function via __proto__', async () => {
    var img = gd.createSync(100, 100);

    assert.throws(() => img.__proto__.getPixel(1, 1), TypeError);
    img.destroy();
  });

  it('throws an Error when too few arguments are supplied', async () => {
    assert.throws(() => gd.create(100), Error, /Expected 2 argument/);
  });

  it('throws a RangeError when argument is not a Number - NaN', async () => {
    assert.throws(() => gd.create(NaN, 100), RangeError, /width/);
  });

  it('throws a RangeError when argument is not a Number - Infinity', async () => {
    assert.throws(() => gd.create(Infinity, 100), RangeError, /width/);
  });

  it('throws an TypeError when the first argument if of wrong type', async () => {
    assert.throws(() => gd.create('bogus', undefined), TypeError, /Argument 0 must be a Number/);
  });

  it('throws an TypeError when the second argument if of wrong type', async () => {
    assert.throws(() => gd.create(100, 'bogus'), TypeError, /Argument 1 must be a Number/);
  });

  it('throws a RangeError when the width parameter is 0', async () => {
    assert.throws(() => gd.create(0, 100), RangeError, /width/);
  });

  it('throws a RangeError when the height parameter is 0', async () => {
    assert.throws(() => gd.create(100, 0), RangeError, /height/);
  });

  it('throws a RangeError when the height parameter is a negative value', async () => {
    assert.throws(() => gd.create(100, -10), RangeError, /height/);
  });

  it('truncates fractional width and height values', async () => {
    const img = await gd.create(100.5, 101.6);

    assert.equal(img.width, 100);
    assert.equal(img.height, 101);
    img.destroy();
  });

  it('throws an Error when creating an image without width and height', async () => {
    assert.throws(() => gd.create(), Error, /Expected 2 argument/);
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
describe('gd.createTrueColor - Create a true color image', function () {
  it('returns a Promise', async () => {
    const imagePromise = gd.createTrueColor(101, 101);

    assert.ok(imagePromise.constructor === Promise);

    const image = await imagePromise;
    image.destroy();
  });

  it('returns a Promise that resolves to an Image', async function () {
    const image = await gd.createTrueColor(101, 101);

    assert.ok(image.constructor === gd.Image);
    image.destroy();
  });

  it('can be done', async () => {
    var img = await gd.createTrueColor(100, 100);
    assert.ok(img instanceof gd.Image);
    img.destroy();
  });

  it('throws an Error when too few arguments are supplied', async () => {
    assert.throws(() => gd.createTrueColor(100), Error, /Expected 2 argument/);
  });

  it('throws an TypeError when the first argument if of wrong type', async () => {
    assert.throws(() => gd.createTrueColor('bogus', undefined), TypeError, /Argument 0 must be a Number/);
  });

  it('throws an TypeError when the second argument if of wrong type', async () => {
    assert.throws(() => gd.createTrueColor(100, 'bogus'), TypeError, /Argument 1 must be a Number/);
  });

  it('throws a RangeError when the width parameter is 0', async () => {
    assert.throws(() => gd.createTrueColor(0, 100), RangeError, /width/);
  });

  it('throws a RangeError when the height parameter is 0', async () => {
    assert.throws(() => gd.createTrueColor(100, 0), RangeError, /height/);
  });

  it('truncates fractional width and height values', async () => {
    const img = await gd.createTrueColor(100.5, 101.6);

    assert.equal(img.width, 100);
    assert.equal(img.height, 101);
    img.destroy();
  });

  it('returns an object containing basic information about the created image', async () => {
    var img = await gd.createTrueColor(100, 100);
    assert.ok(img.width === 100 && img.height === 100 && img.trueColor === 1);
    img.destroy();
  });

  it('has 9 enumerable properties', async function () {
    const img = await gd.createTrueColor(100, 100);
    const props = [
      'trueColor',
      'width',
      'height',
      'interlace',
      'colorsTotal',
      'toString',
      'interpolationId',
      'resX',
      'resY',
    ];

    let i = 0;
    for (let prop in img) {
      assert.isTrue(props.includes(prop));
      i++;
    }

    assert.equal(i, 9);

    img.destroy();
  });
});
