import path from 'node:path';
import { pathToFileURL } from 'node:url';

const packageDir = path.resolve(process.argv[2] ?? '.');
const { default: gd } = await import(pathToFileURL(path.join(packageDir, 'index.js')).href);

const img = await gd.createTrueColor(16, 16);
img.filledRectangle(0, 0, 15, 15, 0x3366cc);
const avif = img.avifPtr(90, 10);
img.destroy();

const decoded = gd.createFromAvifPtr(avif);
const [r, g, b] = [decoded.red, decoded.green, decoded.blue].map((channel) => channel.call(decoded, decoded.getTrueColorPixel(8, 8)));
decoded.destroy();

const close = (value, expected) => Math.abs(value - expected) <= 8;
if (!close(r, 0x33) || !close(g, 0x66) || !close(b, 0xcc)) {
  console.error(`AVIF round trip changed the color to ${r},${g},${b}`);
  process.exit(1);
}
console.log(`AVIF round trip works, ${avif.length} bytes`);
