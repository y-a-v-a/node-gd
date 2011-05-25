/*
Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

var sys  = require('sys');
var fs   = require('fs');
var bind = require('./gd_bindings');

for(var p in bind) {
	if (bind[p] !== undefined) exports[p] = bind[p];
}

var formats = {
	jpeg    : [1,2],  // format : arguments' length [open, save]
	png     : [1,2], 
	gif     : [1,1], 
	gd      : [1,1], 
	gd2     : [1,1], 
	gd2Part : [5,-1],
	WBMP    : [1,1]
};

function arg2arr(args) {
	var arr = [];
	for(var i=0,l=args.length; i<l; i++) {
		arr[i] = args[i];
	}
	return arr;
}

function open_func(format, len) {
	return function() {
		var args     = arg2arr(arguments);
		var filename = args.shift();
		var callback = args[len-1];

		if (typeof callback != 'function') {
			return bind['createFrom'+format].apply(arguments);
		}

		args.pop();

		fs.readFile(filename, 'binary', function(err, data) {
			if(err) throw err;
			callback(bind['createFrom'+format+'Ptr'](data));
		});
	}
}

function save_func(format, len) {
	format = format.toLowerCase();

	return function() {
		var args     = arg2arr(arguments);
		var filename = args.shift();
		var callback = args[len-1];

		if (typeof callback != 'function') {
			return this[format].apply(this, arguments);
		}

		var data = this[format+'Ptr'].apply(this, args);

		fs.writeFile(filename, data, "binary", function(err) {
			if (err) throw err;
			callback();
		});
	}
}

// create wrapper functions
for(var format in formats) {
	var v = formats[format];

	format = format.replace(/^[a-z]/, function(m0){ return m0.toUpperCase() });

	if (v[0] >= 0) {
		exports['open'+format] = open_func(format, v[0]);
	}

	if (v[1] >= 0) {
		bind.Image.prototype['save'+format] = save_func(format, v[1]);
	}
}

exports.noop = function(){};
