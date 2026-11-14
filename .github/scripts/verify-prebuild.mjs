import { createRequire } from 'node:module';
import path from 'node:path';
import { pathToFileURL } from 'node:url';

const packageDir = path.resolve(process.argv[2] ?? '.');
const libc = process.argv[3];

const require = createRequire(path.join(packageDir, 'package.json'));
const platformPackage = `@y-a-v-a/node-gd-${process.platform}-${process.arch}/package.json`;
let expectedDir = path.join(packageDir, 'prebuilds');
try {
  expectedDir = path.dirname(require.resolve(platformPackage));
} catch {}

const { default: gd } = await import(pathToFileURL(path.join(packageDir, 'index.js')).href);

const binaries = Object.keys(require.cache).filter((file) => file.endsWith('.node'));
console.log('Binary:', binaries.join(', '));
if (binaries.length !== 1 || !binaries[0].startsWith(expectedDir + path.sep)) {
  console.error(`The loaded binary is not the prebuilt binary in ${expectedDir}`);
  process.exit(1);
}
if (libc && !binaries[0].endsWith(`.${libc}.node`)) {
  console.error(`The loaded binary is not the ${libc} build`);
  process.exit(1);
}
console.log('libgd:', gd.getGDVersion());
console.log(
  'Features:',
  Object.getOwnPropertyNames(gd)
    .filter((key) => key.startsWith('GD_'))
    .map((key) => `${key}=${gd[key]}`)
    .join(' ')
);

const img = await gd.createTrueColor(10, 10);
img.setPixel(1, 1, 0xff0000);
const pixel = img.getPixel(1, 1);
const png = img.pngPtr();
img.destroy();

if (pixel !== 0xff0000 || png.length === 0) {
  console.error(`Drawing failed: pixel ${pixel.toString(16)}, PNG ${png.length} bytes`);
  process.exit(1);
}
console.log('Prebuild loaded and working');
