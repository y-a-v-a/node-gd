/**
 * Async worker to create a palette or true color image asynchronously
 * Uses Nan v2
 * https://github.com/nodejs/nan
 *
 * Copyright (c) 2015, Vincent Bruijn <vebruijn@gmail.com>
 */
#include <nan.h>
#include "node_gd.h"
#include <gd.h>

class CreateWorker : public Nan::AsyncWorker {
  public:
    CreateWorker(Nan::Callback *callback, int width, int height, int trueColor)
      : Nan::AsyncWorker(callback), _width(width), _height(height), _trueColor(trueColor) {}
    ~CreateWorker() {}

    void Execute() {
      if (_trueColor == 0) {
        image = gdImageCreate(_width, _height);
      } else {
        image = gdImageCreateTrueColor(_width, _height);
      }
      if (!image) {
        SetErrorMessage("No image created!");
      }
    }

    void HandleOKCallback() {
      Nan::HandleScope scope;

      v8::Local<v8::Value> _arg_ = Nan::New<v8::External>(image);
      v8::Local<v8::FunctionTemplate> func = Nan::New(Gd::Image::constructor);
      v8::Local<v8::Object> _image = func->GetFunction()->NewInstance(1, &_arg_);

      v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        _image
      };
      callback->Call(2, argv);
    }

  private:
    gdImagePtr image;
    int _width;
    int _height;
    int _trueColor;
};
