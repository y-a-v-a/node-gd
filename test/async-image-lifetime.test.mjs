import { spawnSync } from 'node:child_process';
import { assert } from 'chai';
import { fileURLToPath } from 'node:url';
import { dirname, resolve } from 'node:path';

const __dirname = dirname(fileURLToPath(import.meta.url));
const root = resolve(__dirname, '..');

/**
 * Runs a scenario in a child process so a possible crash (segfault) does not
 * take down the mocha process. Returns the child's exit code.
 */
function runScenario(source, env = {}) {
  const result = spawnSync(
    process.execPath,
    ['--input-type=module', '-e', source],
    {
      env: {
        ...process.env,
        UV_THREADPOOL_SIZE: '1',
        NODE_GD_ROOT: root,
        ...env,
      },
      encoding: 'utf8',
      timeout: 30000,
    },
  );
  return result;
}

describe('async image lifetime', function () {
  it('save worker racing destroy() does not crash the process', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      const im = await gd.createTrueColor(64, 64);
      const p = im.png('/tmp/node-gd-race-1.png');
      im.destroy();
      await p;
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
  });

  it('multiple pending workers racing destroy() do not crash', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      const im = await gd.createTrueColor(128, 128);
      const p1 = im.png('/tmp/node-gd-race-2a.png');
      const p2 = im.jpeg('/tmp/node-gd-race-2b.jpg', 80);
      const p3 = im.file('/tmp/node-gd-race-2c.gd');
      im.destroy();
      await Promise.all([p1, p2, p3]);
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
  });

  it('destroy() blocks new operations while workers are pending', function () {
    const result = runScenario(`
      const { default: gd } = await import(process.env.NODE_GD_ROOT + '/index.js');
      const im = await gd.createTrueColor(64, 64);
      const p = im.png('/tmp/node-gd-race-3.png');
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
      const im = await gd.createTrueColor(64, 64);
      const p = im.png('/tmp/node-gd-race-4.png');
      im.destroy();
      const ok = await p;
      console.log('RESOLVED=' + ok);
      console.log('SIZE=' + statSync('/tmp/node-gd-race-4.png').size);
    `);
    assert.strictEqual(result.status, 0, `child crashed: ${result.stderr}`);
    assert.match(result.stdout, /RESOLVED=true/);
    const size = Number(/SIZE=(\d+)/.exec(result.stdout)[1]);
    assert.isAbove(size, 0, 'expected a non-empty png file');
  });
});
