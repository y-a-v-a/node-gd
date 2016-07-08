var util = require('util');
var fs = require('fs');

var bindings;

function openFormatFn(fmt) {
  return function() {
    var args = [];
    for (var i = 0; i < arguments.length; i++) {
      args[i] = (arguments[i]);
    }

    var file = args.shift();
    var cb = args.pop();

    if (typeof cb !== "function") {
      return bindings["createFrom" + fmt].apply(this, arguments);
    }

    return fs.readFile(file, function (err, data) {
      var img;
      if (err) {
        return cb(err);
      } else {
        try {
          img = bindings["createFrom" + fmt + "Ptr"](data);
        } catch(e) {
          return cb(e);
        }
        return cb(null, img);
      }
    });
  };
}

function saveFormatFn(format) {
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
  var format, formats, valid, version = bindings.getGDVersion();

  formats = {
    Jpeg: [1, 1],
    Png: [1, 1],
    Gif: [1, 1],
    Gd: [1, 1],
    Gd2: [1, 1],
    Gd2Part: [1, 0],
    WBMP: [1, 1]
  };

  if (version >= '2.1.0') {
    formats.Bmp = [1, 1];
  }

  if (bindings.GD_TIFF && version >= '2.1.0') {
    formats.Tiff = [0, 1];
  }

  if (bindings.GD_WEBP && version >= '2.1.0') {
    formats.Webp = [1, 1];
  }

  for (format in formats) {
    valid = formats[format];
    if (!!valid[0]) {
      bindings["open" + format] = openFormatFn(format);
    }
    if (!!valid[1]) {
      bindings.Image.prototype["save" + format] = saveFormatFn(format);
    }
  }
}

var libPaths = [
  __dirname + '/../build/Release/node_gd.node',
  __dirname + '/../build/default/node_gd.node'
];

try {
  bindings = require(libPaths.shift());
} catch (e) {
  try {
    bindings = require(libPaths.shift());
  } catch (e) {
    console.log('Unable to find addon node_gd.node in build directory.');
    process.exit(1);
  }
}

exportFormats();

if (bindings.getGDVersion() >= '2.1.1') {
  bindings.Image.prototype.saveFile = function() {
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

  bindings.openFile = function() {
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

module.exports = bindings;
