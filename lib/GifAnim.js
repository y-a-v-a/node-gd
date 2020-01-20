'use strict';

/**
 * Wrapper class around GD's Gif animation functions
 * Copyright (c) 2020 Vincent Bruijn <vebruijn@gmail.com>
 *
 * MIT Licensed
 */

const bindings = require('./bindings');
const fs = require('fs');

module.exports = class GifAnim {
  constructor(image, options = {}) {
    if (!image || image.constructor !== bindings.Image) {
      throw new Error('Constructor requires an instance of gd.Image as first parameter');
    }
    options = Object.assign({
      globalColorMap: 1,
      loops: -1,
      localColorMap: 0,
      leftOffset: 0,
      topOffset: 0,
      delay: 100,
      disposal: 1
    }, options);
    this.isEnded = false;
    this.frames = [];

    const animationMetaData = image.gifAnimBegin(options.globalColorMap, options.loops);
    if (!animationMetaData) {
      throw new Error('Unable to begin Gif animation');
    }
    this.frameBuffers = [animationMetaData];

    const firstFrame = image.gifAnimAdd(
      options.localColorMap,
      options.leftOffset,
      options.topOffset,
      options.delay,
      options.disposal,
      null);

    if (!firstFrame) {
      throw new Error('Unable to add frame to Gif animation');
    }
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
      throw new Error('Only instances of gd.Image can be added as additional frames.');
    }

    if (this.isEnded) {
      throw new Error('No more frames can be added to this animation, gd.GifAnim#end() has been called earlier for this instance.')
    }

    options = Object.assign({
      localColorMap: 0,
      leftOffset: 0,
      topOffset: 0,
      delay: 100,
      disposal: 1
    }, options);

    const newFrame = image.gifAnimAdd(
      options.localColorMap,
      options.leftOffset,
      options.topOffset,
      options.delay,
      options.disposal,
      this.frames[this.lastIndex]);

    if (!newFrame) {
      throw new Error('Unable to add frame to Gif animation');
    }

    this.frameBuffers.push(newFrame);
    this.frames.push(image);
  }

  get lastIndex() {
    return this.frames.length - 1;
  }

  end(outName) {
    return new Promise((resolve, reject) => {
      if (this.isEnded) {
        reject('gd.GifAnim#end() already called');
      }
      this.frames[this.lastIndex].gifAnimEnd();
      if (typeof outName === 'string' && outName.length) {
        fs.writeFile(outName, Buffer.concat(this.frameBuffers), error => {
          if (error) reject('Unable to save animation');

          this.isEnded = true;
          resolve(true);
        });
      } else {
        this.isEnded = true;
        resolve(Buffer.concat(this.frameBuffers));
      }
    });
  }
};
