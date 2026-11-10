import fs from 'fs';
import gd from '../../index.js';

console.log('GD_XPM', gd.GD_XPM, 'GD_TIFF', gd.GD_TIFF);
const img = await gd.createTrueColor(16, 16);
img.filledRectangle(0, 0, 15, 15, 0xff0000);
const data = img.tiffPtr();
console.log('tiffPtr', data.length, 'bytes, header', data.subarray(0, 8).toString('hex'));
fs.writeFileSync('/tmp/roundtrip.tif', data);

const attempts = [
  ['createFromTiffPtr(tiffPtr())', () => gd.createFromTiffPtr(data)],
  ['openTiff(file of tiffPtr())', () => gd.openTiff('/tmp/roundtrip.tif')],
  ['createFromTiffPtr(fixture)', () => gd.createFromTiffPtr(fs.readFileSync('test/fixtures/input.tif'))],
  ['openTiff(fixture)', () => gd.openTiff('test/fixtures/input.tif')],
];
for (const [label, fn] of attempts) {
  try {
    const result = await fn();
    console.log(label, '->', result.width, 'x', result.height);
  } catch (error) {
    console.log(label, 'fails:', error.message);
  }
}
await img.saveTiff('/tmp/saved.tif');
console.log('saveTiff', fs.statSync('/tmp/saved.tif').size, 'bytes, header', fs.readFileSync('/tmp/saved.tif').subarray(0, 8).toString('hex'));
