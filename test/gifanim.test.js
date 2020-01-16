var gd = require('../index');

describe('Gif animation creation', function() {
  it('gd.Image#gifAnimBegin -- returns a Promise', async function() {
    this.skip();
    var anim = './test/output/anim.gif';

    // create first frame
    var firstFrame = await gd.create(200,200);

    // allocate some colors
    var whiteBackground = firstFrame.colorAllocate(255, 255, 255);
    var pink = firstFrame.colorAllocate(255, 0, 255);
    var trans = firstFrame.colorAllocate(1,1,1);

    // // create first frame and draw an ellipse
    firstFrame.ellipse(100, -50, 100, 100, pink);
    // // await firstFrame.gif('./test.gif');

    // // start animation
    firstFrame.gifAnimBegin(anim, 1, -1);
    firstFrame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);

    var totalFrames = [];
    for(var i = 0; i < 30; i++) {
      totalFrames.push(gd.create(200,200));
    }

    Promise.all(totalFrames).then(frames => {
      return frames.map(function(frame, idx, arr) {
        frame.colorAllocate(255, 255, 255);
        let pink = frame.colorAllocate(255, 0, 255);
        firstFrame.paletteCopy(frame);
        frame.colorTransparent(trans);
        // const frame = await gd.create(200, 200);
        // arr[idx] = frame;
        frame.ellipse(100, (idx * 10 - 40), 100, 100, pink);
        var lastFrame = i === 0 ? firstFrame : arr[i - 1];

        // await frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);
        frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, lastFrame);
        // frame.destroy();

        // frame.ellipse(100, (1 * 10 - 40), 100, 100, pink);
        // await frame.file(`./test-1.jpg`);
        // frame.gifAnimAdd(anim, 0, 0, 0, 5, 1, null);
        return frame;
      });

    }).then(async function(frames) {
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


  it("has a new implementation", async function() {
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

    const anim = new gd.GifAnim(image, {delay: 10});

    anim.add(image2, {delay: 10});
    anim.add(image3, {delay: 10});
    anim.add(image4, {delay: 10});
    anim.add(image5, {delay: 10});

    await anim.end('./test/output/output-animation.gif');

  });
});
