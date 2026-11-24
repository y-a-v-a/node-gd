import { spawnSync } from 'node:child_process';
import { join } from 'node:path';
import { pathToFileURL } from 'node:url';
import { assert } from 'chai';

import gd from '../index.js';
import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);
const target = join(currentDir, 'output');
const indexUrl = pathToFileURL(join(currentDir, '..', 'index.js')).href;

describe('gd.Image save methods save a copy of the image', function () {
  it('drawing after png() does not change the saved file', function () {
    const result = spawnSync(
      process.execPath,
      [
        '--input-type=module',
        '-e',
        `
        const { default: gd } = await import(process.env.NODE_GD_URL);
        const { join } = await import('node:path');
        const out = process.env.OUTPUT_DIR;
        const blocker = await gd.createTrueColor(2000, 2000);
        const im = await gd.createTrueColor(64, 64);
        const pending = [
          blocker.png(join(out, 'save-copy-blocker.png'), 9),
          im.png(join(out, 'save-copy-draw.png')),
        ];
        im.filledRectangle(0, 0, 63, 63, 0xff0000);
        await Promise.all(pending);
        blocker.destroy();
        im.destroy();
        const saved = await gd.createFromPng(join(out, 'save-copy-draw.png'));
        console.log('PIXEL=' + saved.getTrueColorPixel(10, 10));
        saved.destroy();
        `,
      ],
      {
        env: {
          ...process.env,
          UV_THREADPOOL_SIZE: '1',
          NODE_GD_URL: indexUrl,
          OUTPUT_DIR: target,
        },
        encoding: 'utf8',
        timeout: 30000,
      },
    );

    assert.strictEqual(result.status, 0, `child failed: ${result.stderr}`);
    assert.match(result.stdout, /PIXEL=0\b/);
  });

  it('keeps alpha, resolution and interlace in the saved file', async function () {
    const file = join(target, 'save-copy-settings.png');
    const im = await gd.createTrueColor(16, 16);
    im.alphaBlending(0);
    im.saveAlpha(1);
    im.filledRectangle(0, 0, 15, 15, gd.trueColorAlpha(255, 0, 0, 64));
    im.setResolution(300, 150);
    im.interlace = true;
    await im.png(file);
    im.destroy();

    const saved = await gd.createFromPng(file);
    assert.strictEqual((saved.getTrueColorPixel(8, 8) >>> 24) & 0x7f, 64);
    assert.strictEqual(saved.resX, 300);
    assert.strictEqual(saved.resY, 150);
    assert.isTrue(saved.interlace);
    saved.destroy();
  });

  it('keeps the palette and transparent color of a palette image', async function () {
    const file = join(target, 'save-copy-palette.gif');
    const im = await gd.create(16, 16);
    im.colorAllocate(255, 255, 255);
    const red = im.colorAllocate(255, 0, 0);
    im.filledRectangle(4, 4, 11, 11, red);
    im.colorTransparent(red);
    await im.gif(file);
    im.destroy();

    const saved = await gd.createFromGif(file);
    const pixel = saved.getPixel(8, 8);
    assert.strictEqual(saved.getTransparent(), pixel);
    assert.strictEqual(saved.red(pixel), 255);
    assert.strictEqual(saved.green(pixel), 0);
    saved.destroy();
  });

  it('saves an image that has drawn a filled polygon', async function () {
    const file = join(target, 'save-copy-polygon.png');
    const im = await gd.createTrueColor(32, 32);
    im.filledPolygon(
      [
        { x: 2, y: 2 },
        { x: 30, y: 4 },
        { x: 16, y: 30 },
      ],
      0x00ff00,
    );
    assert.isTrue(await im.png(file));
    im.destroy();
  });
});
