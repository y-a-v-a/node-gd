var util = require('util');
var fs = require('fs');

var gdBindings;

function openFormatFunction(format) {
  return function () {
    var args = [];
    for (var i = 0; i < arguments.length; i++) {
      args[i] = (arguments[i]);
    }

    var filename = args.shift();
    var callback = args.pop();

    if (typeof callback !== "function") {
      return gdBindings["createFrom" + format].apply(this, arguments);
    }

    return fs.readFile(filename, function (err, data) {
      var image;
      if (err) {
        return callback(err);
      } else {
        try {
          image = gdBindings["createFrom" + format + "Ptr"](data);
        } catch(e) {
          return callback(e);
        }
        return callback(null, image);
      }
    });
  };
}

function saveFormatFunction(format) {
  format = format.toLowerCase();

  return function () {
    var data;
    var args = [];
    for (var i = 0; i < arguments.length; i++) {
      args[i] = (arguments[i]);
    }

    var filename = args.shift();
    var callback = args.pop();

    if (typeof callback !== "function") {
      return this[format].apply(this, arguments);
    }

    data = this[format + "Ptr"].apply(this, args);
    return fs.writeFile(filename, data, "binary", callback);
  };
}

function exportFormats() {
  var format;
  var formats;
  var valid;

  formats = {
    Jpeg: [1, 1],
    Png: [1, 1],
    Gif: [1, 1],
    Gd: [1, 1],
    Gd2: [1, 1],
    Gd2Part: [1, 0],
    WBMP: [1, 1]
  };

  if (gdBindings.getGDVersion() >= '2.1.0') {
    formats.Bmp = [1, 1];
  }

  if (gdBindings.GD_TIFF && gdBindings.getGDVersion() >= '2.1.0') {
    formats.Tiff = [0, 1];
  }

  if (gdBindings.GD_WEBP && gdBindings.getGDVersion() >= '2.1.0') {
    formats.Webp = [1, 1];
  }

  for (format in formats) {
    valid = formats[format];
    if (!!valid[0]) {
      gdBindings["open" + format] = openFormatFunction(format);
    }
    if (!!valid[1]) {
      gdBindings.Image.prototype["save" + format] = saveFormatFunction(format);
    }
  }

  if (gdBindings.getGDVersion() >= '2.1.1') {
    gdBindings.Image.prototype["saveFile"] = function() {
      var args = [];
      for (var i = 0; i < arguments.length; i++) {
        args[i] = (arguments[i]);
      }

      var callback = args[args.length - 1];
      if (typeof callback !== "function") {
        return this.file.apply(this, args);
      }
      return this.fileCallback.apply(this, args);
    };

    gdBindings.openFile = function() {
      var args = [];
      var image;
      for (var i = 0; i < arguments.length; i++) {
        args[i] = (arguments[i]);
      }

      var callback = args[args.length - 1];
      if (typeof callback !== "function") {
        return this.createFromFile.apply(this, args);
      }

      try {
        image = this.createFromFile.apply(this, args);
      } catch(e) {
        return callback(e);
      }
      return callback(null, image);
    };
  }
}

var libPaths = [
  __dirname + '/../build/Release/node_gd.node',
  __dirname + '/../build/default/node_gd.node'
];

try {
  gdBindings = require(libPaths.shift());
} catch (e) {
  try {
    gdBindings = require(libPaths.shift());
  } catch (e) {
    console.log('Unable to find addon node_gd.node in build directory.');
    process.exit(1);
  }
}

exportFormats();

module.exports = gdBindings;

