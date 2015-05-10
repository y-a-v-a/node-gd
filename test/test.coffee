assert = require('chai').assert
gd = require '../js/node-gd.js'
fs = require 'fs'
path = require 'path'
util = require 'util'

describe 'Node.js GD Graphics Library', ->
  s = [
    __dirname+'/fixtures/input.png',
    __dirname + '/fixtures/input-rotate.png'
  ]
  t = [
    __dirname+'/fixtures/output.png',
    __dirname + '/fixtures/output-rotate.png'
  ]

  before ->
    # delete previous output image from disk
    t.forEach (img, idx) ->
      if fs.existsSync img
        fs.unlinkSync img

  it 'can scale-down (resize) an image', (done) ->
    gd.openPng s[0], (err, img) ->
      throw err if err
      scale = 2 # factor
      w = Math.floor(img.width / scale)
      h = Math.floor(img.height / scale)

      # create new blank image canvas
      canvas = gd.createTrueColor w, h

      # copy existing image to canvas, scaled
      img.copyResampled canvas, 0, 0, 0, 0, w, h, img.width, img.height

      # save canvas to disk
      canvas.savePng t[0], 1, (err) ->
        throw err if err
        assert.ok fs.existsSync t[0]
        done()

  it 'can rotate an image', (done) ->
    gd.openPng s[1], (err, img) ->
      throw err if err
      w = 100
      h = 100
      canvas = gd.createTrueColor w, h

      img.copyRotated canvas, 50, 50, 0, 0, img.width, img.height, 45

      canvas.savePng t[1], 1, (err) ->
        throw err if err
        assert.ok fs.existsSync t[1]
        done()
