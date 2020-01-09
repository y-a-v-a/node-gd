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

/**
 * CreateFromWorker only to be inherited from
 *
 * This worker class contains recurring code. The CreateFromJpegWorker and others
 * are children of this class
 */
class CreateFromWorker : public AsyncWorker {
  public:
    CreateFromWorker(napi_env env, const char* resource_name)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)) {
      }

  protected:
    virtual void OnOK() override {
      // create new instance of Gd::Image with resulting image
      Napi::Value argv = Napi::External<gdImagePtr>::New(Env(), &image);
      Napi::Object instance = Gd::Image::constructor.New({argv});

      // resolve Promise with instance of Gd::Image
      _deferred.Resolve(instance);
    }

    virtual void OnError(const Napi::Error& e) override {
      // reject Promise with error message
      _deferred.Reject(Napi::String::New(Env(), e.Message()));
    }

    gdImagePtr image;

    Promise::Deferred _deferred;

    std::string path;
};

// JPG
class CreateFromJpegWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromJpegWorker* worker = new CreateFromJpegWorker(info.Env(), "CreateFromJpegWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open JPEG file");
      }
      image = gdImageCreateFromJpeg(in);
      fclose(in);
    }

  private:
    CreateFromJpegWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// PNG
class CreateFromPngWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromPngWorker* worker = new CreateFromPngWorker(info.Env(), "CreateFromPngWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open PNG file");
      }
      image = gdImageCreateFromPng(in);
      fclose(in);
    }

  private:
    CreateFromPngWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// GIF
class CreateFromGifWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromGifWorker* worker = new CreateFromGifWorker(info.Env(), "CreateFromGifWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open GIF file");
      }
      image = gdImageCreateFromGif(in);
      fclose(in);
    }

  private:
    CreateFromGifWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// Gd2
class CreateFromGd2Worker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromGd2Worker* worker = new CreateFromGd2Worker(info.Env(), "CreateFromGd2WorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open GD2 file");
      }
      image = gdImageCreateFromGd2(in);
      fclose(in);
    }

  private:
    CreateFromGd2Worker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// WBMP
class CreateFromWBMPWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromWBMPWorker* worker = new CreateFromWBMPWorker(info.Env(), "CreateFromWBMPWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open WBMP file");
      }
      image = gdImageCreateFromWBMP(in);
      fclose(in);
    }

  private:
    CreateFromWBMPWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// Webp
class CreateFromWebpWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromWebpWorker* worker = new CreateFromWebpWorker(info.Env(), "CreateFromWebpWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open WEBP file");
      }
      image = gdImageCreateFromWebp(in);
      fclose(in);
    }

  private:
    CreateFromWebpWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// Bmp
class CreateFromBmpWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromBmpWorker* worker = new CreateFromBmpWorker(info.Env(), "CreateFromBmpWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open BMP file");
      }
      image = gdImageCreateFromBmp(in);
      fclose(in);
    }

  private:
    CreateFromBmpWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// Tiff
class CreateFromTiffWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromTiffWorker* worker = new CreateFromTiffWorker(info.Env(), "CreateFromTiffWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == NULL) {
        SetError("Cannot open TIFF file");
      }
      image = gdImageCreateFromTiff(in);
      fclose(in);
    }

  private:
    CreateFromTiffWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};









// File
class FileWorker : public AsyncWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);

      FileWorker* worker = new FileWorker(info.Env(), "FileWorkerResource");

      worker->path = path;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      _success = gdImageFile(*_gdImage, path.c_str());

      if (!_success) {
        SetError("Cannot save file");
      }
    }

    virtual void OnOK() override {
      // resolve Promise with boolean
      _deferred.Resolve(Napi::Boolean::New(Env(), _success));
    }

    virtual void OnError(const Napi::Error& e) override {
      // reject Promise with error message
      _deferred.Reject(Napi::String::New(Env(), e.Message()));
    }

  private:
    FileWorker(napi_env env, const char* resource_name)
        : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)) {
        }

    gdImagePtr* _gdImage;

    Promise::Deferred _deferred;

    std::string path;

    bool _success;
};











/**
 * Async worker class to make the I/O from gdImageCreateFromFile async in JavaScript
 * Returns a Promise
 */
class CreateFromFileWorker : public AsyncWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

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

    gdImagePtr image;

    Promise::Deferred _deferred;

    std::string path;

  private:
    CreateFromFileWorker(napi_env env, const char* resource_name)
        : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)) {
        }
};

class CreateWorker : public AsyncWorker {
  public:
    CreateWorker(napi_env env, const char* resource_name, int width, int height, int trueColor)
    : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)), _width(width), _height(height), _trueColor(trueColor) {
    }

    Promise::Deferred _deferred;

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
