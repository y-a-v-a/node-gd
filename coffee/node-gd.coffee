#
# Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>
# Copyright (c) 2014-2015, Vincent Bruijn <vebruijn@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

# format : arguments' length [open, save]
open_func = (format, len) ->
  return ->
    args = Array::slice.call(arguments)
    filename = args.shift()
    callback = args[len - 1]
    return gd_bindings["createFrom" + format].apply(this, arguments) unless typeof callback is "function"
    args.pop()
    fs.readFile filename, (err, data) ->
      if err
        callback err
      else
        callback null, gd_bindings["createFrom" + format + "Ptr"](data)

save_func = (format, len) ->
  format = format.toLowerCase()
  return ->
    args = Array::slice.call(arguments)
    filename = args.shift()
    callback = args[len - 1]
    return this[format].apply(this, arguments)  unless typeof callback is "function"
    data = this[format + "Ptr"].apply(this, args)
    fs.writeFile filename, data, "binary", callback

util = require 'util'
fs = require 'fs'

# not sure what dif. is between Release and default
# but its not consistent across node installations
if fs.existsSync(lib = __dirname+'/../build/Release/node_gd.node')
  gd_bindings = require lib
else if fs.existsSync(lib = __dirname+'/../build/default/node_gd.node')
  gd_bindings = require lib
else
  throw 'unable to locate node_gd.node within build directory'

for p of gd_bindings
  if typeof gd_bindings[p] isnt 'undefined'
    exports[p] = gd_bindings[p]

formats =
  jpeg: [1, 2]
  png: [1, 2]
  gif: [1, 1]
  gd: [1, 1]
  gd2: [1, 1]
  gd2Part: [5, -1]
  WBMP: [1, 1]

if gd_bindings.getGDVersion() >= '2.1.0'
  formats.bmp = [1, 2]

if gd_bindings.GD_TIFF && gd_bindings.getGDVersion() >= '2.1.0'
  formats.tiff = [-1, 1]

if gd_bindings.GD_WEBP && gd_bindings.getGDVersion() >= '2.1.0'
  formats.webp = [1, 2]

# create wrapper functions
for format of formats
  v = formats[format]
  format = format.replace /^[a-z]/, (m0) ->
    m0.toUpperCase()
  exports["open" + format] = open_func(format, v[0]) if v[0] >= 0
  gd_bindings.Image::["save" + format] = save_func(format, v[1]) if v[1] >= 0

if gd_bindings.getGDVersion() >= '2.1.1'
  gd_bindings.Image::["saveFile"] = () ->
    args = Array::slice.call(arguments)
    callback = args[args.length - 1]
    return this["file"].apply(this, args) unless typeof callback is "function"
    this["fileCallback"].apply(this, args)

  exports["openFile"] = () ->
    args = Array::slice.call(arguments)
    callback = args[args.length - 1]
    return this["createFromFile"].apply(this, args) unless typeof callback is "function"
    callback null, this["createFromFile"].apply(this, args)
