#
# Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>
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
    return gd_bindings["createFrom" + format].apply(arguments)  unless typeof callback is "function"
    args.pop()
    fs.readFile filename, "binary", (err, data) ->
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
if fs.existsSync(lib = __dirname+'/../build/Release/node-gd.node')
  gd_bindings = require lib
else if fs.existsSync(lib = __dirname+'/../build/default/node-gd.node')
  gd_bindings = require lib
else
  throw 'unable to locate node-gd.node within build directory'

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

# create wrapper functions
for format of formats
  v = formats[format]
  format = format.replace /^[a-z]/, (m0) ->
    m0.toUpperCase()
  exports["open" + format] = open_func(format, v[0]) if v[0] >= 0
  gd_bindings.Image::["save" + format] = save_func(format, v[1]) if v[1] >= 0
