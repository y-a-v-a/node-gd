assert = require('chai').assert
gd = require '../js/node-gd.js'
fs = require 'fs'
path = require 'path'
util = require 'util'

describe 'Node.js GD Graphics Library', ->
  source = __dirname + '/fixtures/'
  target = __dirname + '/output/'

  before ->
    # delete previous output image from disk
    fs.readdir target, (err, files) ->
      files.forEach (file, idx) ->
        if file.substr(0, 6) == 'output'
          fs.unlink target + file, (err) ->
            throw err if err

  it 'can scale-down (resize) an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-scale.png'
    gd.openPng s, (err, img) ->
      throw err if err

      scale = 2 # factor
      w = Math.floor(img.width / scale)
      h = Math.floor(img.height / scale)

      # create new blank image canvas
      canvas = gd.createTrueColor w, h

      # copy existing image to canvas, scaled
      img.copyResampled canvas, 0, 0, 0, 0, w, h, img.width, img.height

      # save canvas to disk
      canvas.savePng t, 1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()

  it 'can rotate an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-rotate.png'
    gd.openPng s, (err, img) ->
      throw err if err
      w = 100
      h = 100
      canvas = gd.createTrueColor w, h

      img.copyRotated canvas, 50, 50, 0, 0, img.width, img.height, 45

      canvas.savePng t, 1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
  it 'can save a png to jpeg', (done) ->
    s = source + 'input.png'
    t = target + 'output.jpg'
    gd.openPng s, (err, img) ->
      throw err if err

      canvas = gd.createTrueColor 100, 100

      img.copyResampled canvas, 0, 0, 0, 0, 100, 100, img.width, img.height

      canvas.saveJpeg t, 10, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
  # it 'can open jpeg', (done) ->
  #   gd.openJpeg t[2], (err, img) ->
  #     throw err if err
  #     done()
  it 'can save a png to gif', (done) ->
    s = source + 'input.png'
    t = target + 'output.gif'
    gd.openPng s, (err, img) ->
      throw err if err

      canvas = gd.createTrueColor img.width, img.height

      img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

      canvas.saveGif t, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
  it 'can save a png to gd', (done) ->
    s = source + 'input.png'
    t = target + 'output.gd'
    gd.openPng s, (err, img) ->
      throw err if err

      canvas = gd.createTrueColor img.width, img.height

      img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

      canvas.saveGd t, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
  it 'can save a png to gd2', (done) ->
    s = source + 'input.png'
    t = target + 'output.gd2'
    gd.openPng s, (err, img) ->
      throw err if err

      canvas = gd.createTrueColor img.width, img.height

      img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

      canvas.saveGd2 t, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
    done()
  it 'can save a png to WBMP', (done) ->
    s = source + 'input.png'
    t = target + 'output.wbmp'
    gd.openPng s, (err, img) ->
      throw err if err

      canvas = gd.createTrueColor img.width, img.height

      img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

      fg = img.getPixel 5, 5

      canvas.saveWBMP t, fg, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
    done()
  it 'can convert to grayscale', (done) ->
    s = source + 'input.png'
    t = target + 'output-grayscale.png'
    gd.openPng s, (err, img) ->
      throw err if err

      img.toGrayscale()

      img.savePng t, 1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        done()
  it 'can create an image with text', (done) ->
    f = source + 'FreeSans.ttf'
    t = target + 'output-string.png'

    img = gd.create 200, 80

    img.colorAllocate 0, 255, 0
    txtColor = img.colorAllocate 255, 0, 255

    img.stringFT txtColor, f, 24, 0, 10, 60, "Hello world"

    img.savePng t, 1, (err) ->
      throw err if err
      done()
  it 'can create a truecolor iamge with text', (done) ->
    f = source + 'FreeSans.ttf'
    t = target + 'output-truecolor-string.png'

    img = gd.createTrueColor 120, 20

    txtColor = img.colorAllocate 255, 255, 0

    img.stringFT txtColor, f, 16, 0, 8, 18, "Hello world!"

    img.savePng t, 1, (err) ->
      throw err if err
      done()
