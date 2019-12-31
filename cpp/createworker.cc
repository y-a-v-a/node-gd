/**
 * Async worker to create a palette or true color image asynchronously
 * Uses node-addon-api, N-API from C++
 * https://github.com/nodejs/node-addon-api
 *
 * Copyright (c) 2015-2020, Vincent Bruijn <vebruijn@gmail.com>
 */
#include <napi.h>
#include <gd.h>
#include "node_gd.h"

class CreateWorker : public Napi::AsyncWorker {
  public:
    CreateWorker(Napi::Function callback, int width, int height, int trueColor)
      : Napi::AsyncWorker(callback), _width(width), _height(height), _trueColor(trueColor) {}
    ~CreateWorker() {
      image = NULL;
      _width = 0;
      _height = 0;
      _trueColor = 0;
    }

    void Execute() {
      if (_trueColor == 0) {
        image = gdImageCreate(_width, _height);
      } else {
        image = gdImageCreateTrueColor(_width, _height);
      }
      if (!image) {
        SetError("No image created!");
      }
    }

    void OnOK() {
      Napi::HandleScope scope(Env());

      Napi::Value _argv = Napi::External<gdImagePtr>::New(Env(), &image);
      Napi::Object _image = Gd::Image::constructor.New({_argv});

      Callback().Call({
        Env().Null(),
        _image
      });
    }

  private:
    gdImagePtr image;
    int _width;
    int _height;
    int _trueColor;
};
