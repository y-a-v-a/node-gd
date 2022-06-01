import gd from '../index.js';
import { assert } from 'chai';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);

var source = currentDir + '/fixtures';
var target = currentDir + '/output';

describe('gd.openFile', () => {
  it('returns a Promise', () => {
    const imagePromise = gd.openFile(`${source}/input.jpg`);

    assert.ok(imagePromise.constructor === Promise);
    imagePromise.then((image) => image.destroy());
  });

  it('opens a file', async () => {
    const img = await gd.openFile(`${source}/input.jpg`);

    assert.ok(img.width === 100);

    img.destroy();
  });

  it('throws an exception when file does not exist', async () => {
    try {
      await gd.openFile(`${source}/abcxyz.jpg`);
    } catch (exception) {
      assert.ok(exception instanceof Error);
    }
  });
});

describe('gd.file', () => {
  it('returns a Promise', async () => {
    const img = await gd.openFile(`${source}/input.jpg`);

    var a = img.file(`${target}/test.jpg`);
    assert.isTrue(a.constructor === Promise);
  });

  it('returns a Promise which resolves to boolean', async () => {
    const img = await gd.openFile(`${source}/input.jpg`);

    var a = await img.file(`${target}/test1.jpg`);

    assert.isTrue(a === true);
  });

  it('saves files of different extensions', async function () {
    const image = await gd.openFile(`${source}/input.jpg`);

    const success1 = await image.file(`${target}/test.bmp`);
    const success2 = await image.file(`${target}/test.png`);

    assert.isTrue(success1 === true);
    assert.isTrue(success2 === true);
  });
});
