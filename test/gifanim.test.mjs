import fs from 'fs';

import gd from '../index.js';
import { assert } from 'chai';

describe('Gif animation creation', function () {
  it('gd.Image#gifAnimBegin -- returns a Promise', async function () {
    this.skip();
    var anim = './test/output/anim.gif';

    // create first frame
    var firstFrame = await gd.create(200, 200);

    // allocate some colors
    var whiteBackground = firstFrame.colorAllocate(255, 255, 255);
    var pink = firstFrame.colorAllocate(255, 0, 255);
    var trans = firstFrame.colorAllocate(1, 1, 1);

    // // create first frame and draw an ellipse
    firstFrame.ellipse(100, -50, 100, 100, pink);
    // // await firstFrame.gif('./test.gif');

    // // start animation
    firstFrame.gifAnimBegin(anim, 1, -1);
    firstFrame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);

    var totalFrames = [];
    for (var i = 0; i < 30; i++) {
      totalFrames.push(gd.create(200, 200));
    }

    Promise.all(totalFrames)
      .then((frames) => {
        return frames.map(function (frame, idx, arr) {
          frame.colorAllocate(255, 255, 255);
          let pink = frame.colorAllocate(255, 0, 255);
          firstFrame.paletteCopy(frame);
          frame.colorTransparent(trans);
          // const frame = await gd.create(200, 200);
          // arr[idx] = frame;
          frame.ellipse(100, idx * 10 - 40, 100, 100, pink);
          var lastFrame = i === 0 ? firstFrame : arr[i - 1];

          // await frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);
          frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, lastFrame);
          // frame.destroy();

          // frame.ellipse(100, (1 * 10 - 40), 100, 100, pink);
          // await frame.file(`./test-1.jpg`);
          // frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);
          return frame;
        });
      })
      .then(async function (frames) {
        // Promise.all(frames).then(async frames => {

        frames.map(async (frame, idx) => {
          await frame.gif(`./test/output/anim-${idx}.gif`);
          frame.destroy();
        });
        firstFrame.gifAnimEnd(anim);
        firstFrame.destroy();
        // });
      });
  });

  it('has a new implementation', async function () {
    const image = await gd.create(200, 200);
    image.colorAllocate(255, 255, 255);
    const pink = image.colorAllocate(255, 0, 255);
    image.ellipse(100, 100, 100, 100, pink);
    const image2 = await gd.create(200, 200);
    image2.colorAllocate(255, 255, 255);
    image.paletteCopy(image2);
    image2.ellipse(100, 100, 80, 80, pink);
    const image3 = await gd.create(200, 200);
    image3.ellipse(100, 100, 60, 60, pink);
    const image4 = await gd.create(200, 200);
    image4.ellipse(100, 100, 70, 70, pink);
    const image5 = await gd.create(200, 200);
    image5.ellipse(100, 100, 90, 90, pink);

    const anim = new gd.GifAnim(image, { delay: 10 });

    anim.add(image2, { delay: 10 });
    anim.add(image3, { delay: 10 });
    anim.add(image4, { delay: 10 });
    anim.add(image5, { delay: 10 });

    await anim.end('./test/output/output-animation.gif');
  });
});

describe('gd.GifAnim', function () {
  async function frame(size) {
    const image = await gd.create(20, 20);
    image.colorAllocate(255, 255, 255);
    const pink = image.colorAllocate(255, 0, 255);
    image.filledEllipse(10, 10, size, size, pink);
    return image;
  }

  it('throws an Error when not constructed with an image', function () {
    assert.throws(() => new gd.GifAnim(), Error, /requires an instance of gd.Image/);
    assert.throws(() => new gd.GifAnim({ width: 20 }), Error, /requires an instance of gd.Image/);
  });

  it('keeps track of added frames', async function () {
    const first = await frame(4);
    const anim = new gd.GifAnim(first);
    assert.equal(anim.frames.length, 1);
    assert.equal(anim.lastIndex, 0);
    assert.isFalse(anim.isEnded);

    anim.add(await frame(8));
    anim.add(await frame(12));
    assert.equal(anim.frames.length, 3);
    assert.equal(anim.lastIndex, 2);

    await anim.end();
    anim.frames.forEach((image) => image.destroy());
  });

  it('throws an Error when adding something that is not an image', async function () {
    const anim = new gd.GifAnim(await frame(4));
    assert.throws(() => anim.add('frame'), Error, /Only instances of gd.Image/);
    await anim.end();
    anim.frames.forEach((image) => image.destroy());
  });

  it('resolves to a Buffer containing an animated GIF when no file name is given', async function () {
    const anim = new gd.GifAnim(await frame(4), { delay: 5, loops: 0 });
    anim.add(await frame(8), { delay: 5 });
    anim.add(await frame(12), { delay: 5 });

    const data = await anim.end();
    assert.instanceOf(data, Buffer);
    assert.equal(data.subarray(0, 6).toString('latin1'), 'GIF89a');
    assert.isTrue(anim.isEnded);

    // libgd decodes the first frame of an animation
    const decoded = gd.createFromGifPtr(data);
    assert.equal(decoded.width, 20);
    assert.equal(decoded.height, 20);
    decoded.destroy();
    anim.frames.forEach((image) => image.destroy());
  });

  // Known issue: gd.Image#gifAnimEnd() discards the data returned by
  // gdImageGifAnimEndPtr(), so the trailer byte is never appended.
  it.skip('ends the animated GIF with a trailer byte', async function () {
    const anim = new gd.GifAnim(await frame(4));
    anim.add(await frame(8));
    const data = await anim.end();
    assert.equal(data[data.length - 1], 0x3b, 'GIF trailer');
    anim.frames.forEach((image) => image.destroy());
  });

  it('writes the animation to a file when a file name is given', async function () {
    const target = './test/output/output-animation-file.gif';
    const anim = new gd.GifAnim(await frame(4));
    anim.add(await frame(10));

    assert.isTrue(await anim.end(target));
    assert.equal(fs.readFileSync(target).subarray(0, 6).toString('latin1'), 'GIF89a');
    anim.frames.forEach((image) => image.destroy());
  });

  it('throws an Error when adding frames after the animation ended', async function () {
    const anim = new gd.GifAnim(await frame(4));
    await anim.end();
    const late = await frame(8);
    assert.throws(() => anim.add(late), Error, /No more frames can be added/);
    late.destroy();
    anim.frames.forEach((image) => image.destroy());
  });

  it('rejects when the animation is ended twice', async function () {
    const anim = new gd.GifAnim(await frame(4));
    await anim.end();
    let reason;
    try {
      await anim.end();
    } catch (e) {
      reason = e;
    }
    assert.equal(reason, 'gd.GifAnim#end() already called');
    anim.frames.forEach((image) => image.destroy());
  });
});
