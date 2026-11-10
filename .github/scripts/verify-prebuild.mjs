import { createRequire } from 'node:module';
import path from 'node:path';
import { pathToFileURL } from 'node:url';

const packageDir = path.resolve(process.argv[2] ?? '.');
const libc = process.argv[3];

const require = createRequire(path.join(packageDir, 'package.json'));
const binary = require('node-gyp-build').resolve(packageDir);
console.log('Binary:', path.relative(packageDir, binary));

if (!binary.startsWith(path.join(packageDir, 'prebuilds') + path.sep)) {
  console.error('The loaded binary is not a prebuilt binary');
  process.exit(1);
}
if (libc && !binary.endsWith(`.${libc}.node`)) {
  console.error(`The loaded binary is not the ${libc} build`);
  process.exit(1);
}

const { default: gd } = await import(pathToFileURL(path.join(packageDir, 'index.js')).href);
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
