/**
 * Copyright (c) 2020, Vincent Bruijn <vebruijn@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <gd.h>
#include <napi.h>
#include "node_gd.h"

using namespace Napi;

class CreateFromFileWorker : public AsyncWorker {
public:
  static Value DoWork(const CallbackInfo& info) {
    if (info.Length() <= 0 || !info[0].IsString()) {
      Napi::TypeError::New(info.Env(), "Argument must be a string").ThrowAsJavaScriptException();
      return info.Env().Null();
    }
    std::string path = info[0].As<Napi::String>().Utf8Value();

    CreateFromFileWorker* worker = new CreateFromFileWorker(info.Env(), "CreateFromFileWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override {
    // execute the async task
    image = gdImageCreateFromFile(path.c_str());
  }

  virtual void OnOK() override {
    // create new instance of Gd::Image with resulting image
    Napi::Value argv = Napi::External<gdImagePtr>::New(Env(), &image);
    Napi::Object instance = Gd::Image::constructor.New({argv});

    // resolve Promise with instance of Gd::Image
    _deferred.Resolve(instance);
  }

  virtual void OnError(const Napi::Error& e) override {
    _deferred.Reject(Napi::String::New(Env(), e.Message()));
  }

private:
  CreateFromFileWorker(napi_env env, const char* resource_name)
      : AsyncWorker(env, resource_name), _deferred(Napi::Promise::Deferred::New(env)) {
      }

  Promise::Deferred _deferred;

  gdImagePtr image;

  std::string path;
};
