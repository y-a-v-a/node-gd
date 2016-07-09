var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';

/*
 * ┌┬┐┌─┐┌┬┐┌─┐┬─┐┬ ┬  ┬  ┌─┐┌─┐┬┌─┌─┐
 * │││├┤ ││││ │├┬┘└┬┘  │  ├┤ ├─┤├┴┐└─┐
 * ┴ ┴└─┘┴ ┴└─┘┴└─ ┴   ┴─┘└─┘┴ ┴┴ ┴└─┘
 */
describe('Section Memory leaks', function() {
  it('sync gd.openJpeg() and then img.destroy() doesn\'t leak', function(done) {
    var oldMem;
    var newMem;
    var iterations = 150;
    var s = source + 'input.jpg';

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
      if ((newMem.heapUsed > (oldMem.heapUsed + 100 * 1024)) || (newMem.rss > (oldMem.rss + 100 * 1024)) || (newMem.heapTotal > (oldMem.heapTotal + 100 * 1024))){ // consider quadruple the image in the memory size be ok
          var error = new Error("Memory leaks.\nrss delta: " + (newMem.rss - oldMem.rss) + "\nheapTotal delta: " + (newMem.heapTotal - oldMem.heapTotal) + "\nheapUsed delta: " + (newMem.heapUsed - oldMem.heapUsed));
          done(error);
      } else {
        done();
      }
    } else {
      done();
    }
  });
});
