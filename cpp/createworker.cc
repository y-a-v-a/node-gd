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
    CreateWorker(napi_env env, const char* resource_name, int width, int height, int trueColor)
    : AsyncWorker(env, resource_name), _deferred(Napi::Promise::Deferred::New(env)), _width(width), _height(height), _trueColor(trueColor) {
    }

    Napi::Promise::Deferred _deferred;

  protected:
    void Execute() override {
      if (_trueColor == 0) {
        image = gdImageCreate(_width, _height);
      } else {
        image = gdImageCreateTrueColor(_width, _height);
      }
      if (!image) {
        SetError("No image created!");
      }
    }

    virtual void OnOK() override {
      Napi::Value _argv = Napi::External<gdImagePtr>::New(Env(), &image);
      Napi::Object instance = Gd::Image::constructor.New({_argv});

      _deferred.Resolve(instance);
    }

    virtual void OnError(const Napi::Error& e) override {
      _deferred.Reject(Napi::String::New(Env(), e.Message()));
    }

  private:
    gdImagePtr image;

    int _width;

    int _height;

    int _trueColor;
};
