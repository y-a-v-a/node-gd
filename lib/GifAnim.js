/**
 * Wrapper class around GD's Gif animation functions
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

import bindings from './bindings.js';
import fs from 'fs';

export default class GifAnim {
  constructor(image, options = {}) {
    if (!image || image.constructor !== bindings.Image) {
      throw new Error(
        'Constructor requires an instance of gd.Image as first parameter'
      );
    }
    options = Object.assign(
      {
        globalColorMap: 1,
        loops: -1,
        localColorMap: image.trueColor ? 1 : 0,
        leftOffset: 0,
        topOffset: 0,
        delay: 100,
        disposal: 1,
      },
      options
    );
    this.isEnded = false;
    this.frames = [];

    const animationMetaData = image.gifAnimBegin(
      options.globalColorMap,
      options.loops
    );
    this.frameBuffers = [animationMetaData];

    const firstFrame = image.gifAnimAdd(
      options.localColorMap,
      options.leftOffset,
      options.topOffset,
      options.delay,
      options.disposal,
      null
    );

    // keep reference to images because
    // they are used by gd.Image.prototype.gifAnimAdd()
    this.frameBuffers.push(firstFrame);
    this.frames.push(image);
  }

  /**
   * Add a new frame to the animation
   * @param {gd.Image} image Image to add as next frame
   * @param {object} options Object containing meta data for the frame
   */
  add(image, options = {}) {
    if (!image || image.constructor !== bindings.Image) {
      throw new Error(
        'Only instances of gd.Image can be added as additional frames.'
      );
    }

    if (this.isEnded) {
      throw new Error(
        'No more frames can be added to this animation, gd.GifAnim#end() has been called earlier for this instance.'
      );
    }

    options = Object.assign(
      {
        localColorMap: image.trueColor ? 1 : 0,
        leftOffset: 0,
        topOffset: 0,
        delay: 100,
        disposal: 1,
      },
      options
    );

    const newFrame = image.gifAnimAdd(
      options.localColorMap,
      options.leftOffset,
      options.topOffset,
      options.delay,
      options.disposal,
      this.frames[this.lastIndex]
    );


    this.frameBuffers.push(newFrame);
    this.frames.push(image);
  }

  get lastIndex() {
    return this.frames.length - 1;
  }

  end(outName) {
    return new Promise((resolve, reject) => {
      if (this.isEnded) {
        return reject(new Error('gd.GifAnim#end() already called'));
      }

      let trailer;
      try {
        trailer = this.frames[this.lastIndex].gifAnimEnd();
      } catch (error) {
        return reject(error);
      }
      this.frameBuffers.push(trailer);
      this.isEnded = true;

      if (typeof outName === 'string' && outName.length) {
        fs.writeFile(outName, Buffer.concat(this.frameBuffers), (error) => {
          if (error) {
            return reject(new Error('Unable to save animation', { cause: error }));
          }

          resolve(true);
        });
      } else {
        resolve(Buffer.concat(this.frameBuffers));
      }
    });
  }
}
