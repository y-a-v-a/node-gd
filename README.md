GD bindings for Node.js
==================================

GD graphic library bindings for Node.js supporting asynchronous I/O.

Original source https://github.com/taggon/node-gd

Tested with Node v0.4

## Install

    npm install node-gd

## Dependencies

  * **libgd** - in debian install with
  
        apt-get install libgd2-xpm-dev

## Usage

### Require library

    var gd = require("node-gd");

### Open file

    gd.openPng("test.png", function(png, path){
        console.log("Width: ", png.width);
        console.log("Height: ", png.width);
    });
    
### Save file

    png.savePng("out.png", 0, function(){
        console.log("image saved!");
    });
    
### Create empty file

    var img = gd.create(width, height);
    
### Copy image

    source.copyResampled(dest,dstX,dstY,srcX,srcY,dstW,dstH,srcW,srcH);