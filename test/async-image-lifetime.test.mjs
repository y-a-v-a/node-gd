import { spawnSync } from 'node:child_process';
import { assert } from 'chai';
import path from 'node:path';

import dirname from './dirname.mjs';

const currentDir = dirname(import.meta.url);
const root = path.resolve(currentDir, '..');
const target = currentDir + '/output';

/**
 * Runs a scenario in a child process so a possible crash (segfault) does not
 * take down the mocha process. Returns the child's spawnSync result.
 */
function runScenario(source) {
  return spawnSync(
    process.execPath,
    ['--input-type=module', '-e', source],
    {
      env: {
        ...process.env,
        UV_THREADPOOL_SIZE: '1',
        NODE_GD_ROOT: root,
        NODE_GD_TARGET: target,
      },
      encoding: 'utf8',
      timeout: 30000,
    },
  );
}

describe('async image lifetime', function () {
  it('save worker racing destroy() does not crash the process', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      const im = await gd.createTrueColor(64, 64);
      const p = im.png(process.env.NODE_GD_TARGET + '/race-1.png');
      im.destroy();
      await p;
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
  });

  it('multiple pending workers racing destroy() do not crash', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      const im = await gd.createTrueColor(128, 128);
      const t = process.env.NODE_GD_TARGET;
      const p1 = im.png(t + '/race-2a.png');
      const p2 = im.jpeg(t + '/race-2b.jpg', 80);
      const p3 = im.file(t + '/race-2c.gd');
      im.destroy();
      await Promise.all([p1, p2, p3]);
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
  });

  it('destroy() blocks new operations while workers are pending', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      const im = await gd.createTrueColor(64, 64);
      const p = im.png(process.env.NODE_GD_TARGET + '/race-3.png');
      im.destroy();
      try {
        im.width;
        console.log('NO_THROW');
      } catch (e) {
        console.log('THREW');
      }
      await p;
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
    assert.match(result.stdout, /THREW/);
    assert.notMatch(result.stdout, /NO_THROW/);
  });

  it('pending writes still complete after destroy()', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      import { statSync } from 'node:fs';
      const file = process.env.NODE_GD_TARGET + '/race-4.png';
      const im = await gd.createTrueColor(64, 64);
      const p = im.png(file);
      im.destroy();
      const ok = await p;
      console.log('RESOLVED=' + ok);
      console.log('SIZE=' + statSync(file).size);
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
    assert.match(result.stdout, /RESOLVED=true/);
    const size = Number(/SIZE=(\d+)/.exec(result.stdout)[1]);
    assert.isAbove(size, 0, 'expected a non-empty png file');
  });
});
