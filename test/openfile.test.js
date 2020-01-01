var gd = require('../js/node-gd.js');
var assert = require('chai').assert;

var source = __dirname + '/fixtures/';
var target = __dirname + '/output/';


describe("gd#openFile", () => {
  it('opens a file', async () => {
    const img = await gd.openFile(`${source}input.jpg`);

    assert.ok(img.width === 100);

    img.destroy();
  });

  it('throws an exception when file does not exist', async () => {
    try {
      await gd.openFile(`${source}abcxyz.jpg`);
    } catch(exception) {
      assert.ok(exception instanceof Error);
    }
  });
})
