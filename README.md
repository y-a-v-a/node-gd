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

### Open file

    var gd = require("node-gd");

    gd.openPng("test.png", function(png, path){
        console.log("Width: ", png.width);
        console.log("Height: ", png.width);
    });
    
