GD bindings for Node.js
==================================

GD graphic library bindings for Node.js supporting asynchronous I/O.

Original source https://github.com/taggon/node-gd

Tested with Node v0.4

WARNING
-------
This library is ABANDONWARE; its original author, [Taegon Kim](https://github.com/taggon/node-gd) of North Korea,
stopped loving it over 2 years ago at v0.0.2. This is the community-maintained version currently at v0.1.8, which
you will also find linked as the the [official NodeJS.org node-gd repo](https://npmjs.org/package/node-gd). However,
a word of caution: it's [unusually painful to install](https://github.com/taggon/node-gd/issues/13), and there are better alternatives by now--including:
[node-canvas](https://github.com/LearnBoost/node-canvas), [node-o3-canvas](https://github.com/ajaxorg/node-o3-canvas), 
and [node-image](https://github.com/pkrumins/node-image).

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
