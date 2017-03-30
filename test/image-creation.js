var fs = require('fs');

var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';

/**
 * gd.create and gd.createSync
 * ╦┌┬┐┌─┐┌─┐┌─┐  ┌─┐┬─┐┌─┐┌─┐┌┬┐┬┌─┐┌┐┌
 * ║│││├─┤│ ┬├┤   │  ├┬┘├┤ ├─┤ │ ││ ││││
 * ╩┴ ┴┴ ┴└─┘└─┘  └─┘┴└─└─┘┴ ┴ ┴ ┴└─┘┘└┘
 */
describe('Creating a paletted image', function() {
  it('can be done synchronously', function(done) {
    var img = gd.createSync(100, 100);

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

  it('throws an Error when argument is not a Number - NaN', function(done) {
    var img;
    try {
      img = gd.createSync(NaN, 100);
    } catch (e) {
      assert.ok(e instanceof Error);
      return done();
    }
  });

  it('throws an Error when argument is not a Number - Infinity', function(done) {
    var img;
    try {
      img = gd.createSync(Infinity, 100);
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
    gd.create(10, 10, function(error, img) {
      assert.ok(img instanceof gd.Image);
      img.destroy();

      done();
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

  it('throws a RangeError when the height parameter is a negative value', function(done) {
    var img;
    try {
      img = gd.createSync(100, -10);
    } catch (e) {
      assert.ok(e instanceof RangeError);
      return done();
    }
  });

  it('throws an Error when creating an image without width and height - gd.create()', function(done) {
    try {
      gd.create();
    } catch (exception) {
      assert.ok(exception instanceof Error);
      return done();
    }
  });

  it('throws a Error when the height parameter is 0 async', function(done) {
    var img;
    try {
      gd.create(100, 0, function(error, img) { });
    } catch (e) {
      assert.ok(e instanceof RangeError);
      done();
    }
  });

  it('returns an object containing basic information about the created image', function(done) {
    var img = gd.createSync(100, 100);

    assert.equal(img.width, 100);
    assert.equal(img.height, 100);
    assert.equal(img.trueColor, 0);

    img.destroy();
    return done();
  });

  it('returns an object containing basic information about the created image async', function(done) {
    gd.create(100, 100, function(error, image) {
      if (error) {
        throw error;
      }

      assert.equal(image.width, 100);
      assert.equal(image.height, 100);
      assert.equal(image.trueColor, 0);

      image.destroy();
      return done();
    });
  });
});

/**
 * gd.createTrueColor and gd.createTrueColorSync
 */
describe('Create a true color image', function() {
  it('can be done synchronously.', function(done) {
    var img = gd.createTrueColorSync(100, 100);
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
