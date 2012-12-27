assert = require('chai').assert
gd = require '../js/node-gd.js'
fs = require 'fs'
path = require 'path'
util = require 'util'

describe 'Node.js GD Graphics Library', ->
  source = __dirname+'/fixtures/input.png'
  target = __dirname+'/fixtures/output.png'

  beforeEach ->
    # delete previous output image from disk
    if fs.existsSync target
      fs.unlinkSync target

  it 'can scale-down (resize) an image', (done) ->
    gd.openPng source, (err, img) ->
      throw err if err
      scale = 2 # factor
      w = Math.floor(img.width / scale)
      h = Math.floor(img.height / scale)

      # create new blank image canvas
      canvas = gd.createTrueColor w, h

      # copy existing image to canvas, scaled
      img.copyResampled canvas, 0, 0, 0, 0, w, h, img.width, img.height

      # save canvas to disk
      canvas.savePng target, 1, (err) ->
        throw err if err
        assert.ok fs.existsSync target
        done()
