#!/usr/bin/env node

var util  = require('util');
var fs   = require('fs');
var path = require('path');
var gd   = require('./node-gd');

var source = './circle.png';
var target = './circle.thumb.png';

if (fs.existsSync(target)) fs.unlinkSync(target);

gd.openPng(
	source,
	function(err, png) {

		var w = Math.floor(png.width/2), h = Math.floor(png.height/2);
		var target_png = gd.createTrueColor(w, h);

		png.copyResampled(target_png,0,0,0,0,w,h,png.width,png.height);
		target_png.savePng(target, 1, gd.noop);
	}
);
