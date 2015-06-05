assert = require('chai').assert
gd = require '../js/node-gd.js'
fs = require 'fs'
path = require 'path'
util = require 'util'

describe 'Node.js GD Graphics Library', ->
  source = __dirname + '/fixtures/'
  target = __dirname + '/output/'

  before ->
    console.log('Built on top of GD version: ' + gd.getGDVersion())
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
        img.destroy()
        canvas.destroy()
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
        img.destroy()
        canvas.destroy()
        done()

  it 'can convert to grayscale', (done) ->
    s = source + 'input.png'
    t = target + 'output-grayscale.png'
    gd.openPng s, (err, img) ->
      throw err if err

      img.toGrayscale()

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
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
      assert.ok fs.existsSync t
      img.destroy()
      done()

  it 'can create a truecolor image with text', (done) ->
    f = source + 'FreeSans.ttf'
    t = target + 'output-truecolor-string.png'

    img = gd.createTrueColor 120, 20

    txtColor = img.colorAllocate 255, 255, 0

    img.stringFT txtColor, f, 16, 0, 8, 18, "Hello world!"

    img.savePng t, 1, (err) ->
      throw err if err
      assert.ok fs.existsSync t
      img.destroy()
      done()

  it 'can add gaussian blur to an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-gaussianblur.png'

    gd.openPng s, (err, img) ->
      throw err if err

      # call gaussianBlur 10 times on image
      # creates a very blurry image
      img.gaussianBlur() for i in [0...10]

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
        done()

  it 'can negate an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-negate.png'

    gd.openPng s, (err, img) ->
      throw err if err

      img.negate()

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
        done()

  it 'can change brightness of an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-brightness.png'

    gd.openPng s, (err, img) ->
      throw err if err

      brightness = Math.floor(Math.random() * 100)

      img.brightness(brightness)

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
        done()

  it 'can change contrast of an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-contrast.png'

    gd.openPng s, (err, img) ->
      throw err if err

      # a value of 100 gives no contrast i.e. rgb 127, 127, 127 gray
      # I found out a value somewhere between -900 and 1100
      # fits best.
      contrast = Math.floor(Math.random() * 2000) - 900

      img.contrast(contrast)

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
        done()

  it 'can emboss an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-emboss.png'

    gd.openPng s, (err, img) ->
      throw err if err

      img.emboss()

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
        done()

  it 'can apply selective blur to an image', (done) ->
    s = source + 'input.png'
    t = target + 'output-selectiveBlur.png'

    gd.openPng s, (err, img) ->
      throw err if err

      img.selectiveBlur()

      img.savePng t, -1, (err) ->
        throw err if err
        assert.ok fs.existsSync t
        img.destroy()
        done()

  it 'can create a truecolor BMP image with text', (done) ->
    if gd.getGDVersion() < '2.1.1'
      done()
      return

    f = source + 'FreeSans.ttf'
    t = target + 'output-truecolor-string.bmp'

    img = gd.createTrueColor 120, 20

    txtColor = img.colorAllocate 255, 255, 0

    img.stringFT txtColor, f, 16, 0, 8, 18, "Hello world!"

    img.saveBmp t, 0, (err) ->
      throw err if err
      assert.ok fs.existsSync t
      done()

  it 'can create a truecolor Tiff image with text', (done) ->
    if gd.getGDVersion() < '2.1.1'
      done()
      return
    f = source + 'FreeSans.ttf'
    t = target + 'output-truecolor-string.tif'

    img = gd.createTrueColor 120, 20

    txtColor = img.colorAllocate 255, 255, 0

    img.stringFT txtColor, f, 16, 0, 8, 18, "Hello world!"

    img.saveTiff t, (err) ->
      throw err if err
      assert.ok fs.existsSync t
      done()

  # from-to different file types
  describe  'section Handling file types', ->
    it 'can copy a png into a jpeg', (done) ->
      s = source + 'input.png'
      t = target + 'output.jpg'
      gd.openPng s, (err, img) ->
        throw err if err

        canvas = gd.createTrueColor 100, 100

        img.copyResampled canvas, 0, 0, 0, 0, 100, 100, img.width, img.height

        canvas.saveJpeg t, 10, (err) ->
          throw err if err
          assert.ok fs.existsSync t
          img.destroy()
          canvas.destroy()
          done()
    it 'can copy a png into gif', (done) ->
      s = source + 'input.png'
      t = target + 'output.gif'
      gd.openPng s, (err, img) ->
        throw err if err

        canvas = gd.createTrueColor img.width, img.height

        img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

        canvas.saveGif t, (err) ->
          throw err if err
          assert.ok fs.existsSync t
          img.destroy()
          canvas.destroy()
          done()
    it 'can copy a png into gd', (done) ->
      s = source + 'input.png'
      t = target + 'output.gd'
      gd.openPng s, (err, img) ->
        throw err if err

        canvas = gd.createTrueColor img.width, img.height

        img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

        canvas.saveGd t, (err) ->
          throw err if err
          assert.ok fs.existsSync t
          img.destroy()
          canvas.destroy()
          done()
  # it 'can save a png to gd2', (done) ->
  #   s = source + 'input.png'
  #   t = target + 'output.gd2'
  #   gd.openPng s, (err, img) ->
  #     throw err if err

  #     canvas = gd.createTrueColor img.width, img.height

  #     img.copyResampled canvas, 0, 0, 0, 0, img.width, img.height, img.width, img.height

  #     canvas.saveGd2 t, 1, null, (err) ->
  #       throw err if err
  #       assert.ok fs.existsSync t
  #       done()
  #   done()
    it 'can copy a png into WBMP', (done) ->
      if gd.getGDVersion() < '2.1.1'
        done()
        return

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
          img.destroy()
          canvas.destroy()
          done()
      done()
    it 'can open a jpeg file and save it as png', (done) ->
      s = source + 'input.jpg'
      t = target + 'output-from-jpeg.png'

      gd.openJpeg s, (err, img) ->
        throw err if err

        img.savePng t, -1, (err) ->
          throw err if err
          assert.ok fs.existsSync t
          img.destroy()
          done()
    it 'can open a bmp and save it as png', (done) ->
      s = source + 'input.bmp'
      t = target + 'output-from-bmp.png'

      gd.openBmp s, (err, img) ->
        throw err if err

        img.savePng t, -1, (err) ->
          throw err if err
          assert.ok fs.existsSync t
          img.destroy()
          done()

