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

/**
 * @see https://github.com/nodejs/node-addon-api
 */
using namespace Napi;

/**
 * CreateFromWorker only to be inherited from
 *
 * This worker class contains recurring code. The CreateFromJpegWorker and others
 * are decendants of this class. Returns a Promise.
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

/**
 * CreateFromJpegWorker for Jpeg files
 */
class CreateFromJpegWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromJpegWorker* worker = new CreateFromJpegWorker(info.Env(),
        "CreateFromJpegWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open JPEG file");
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

      CreateFromPngWorker* worker = new CreateFromPngWorker(info.Env(),
        "CreateFromPngWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open PNG file");
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

      CreateFromGifWorker* worker = new CreateFromGifWorker(info.Env(),
        "CreateFromGifWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open GIF file");
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

      CreateFromGd2Worker* worker = new CreateFromGd2Worker(info.Env(),
        "CreateFromGd2WorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open GD2 file");
      }
      image = gdImageCreateFromGd2(in);
      fclose(in);
    }

  private:
    CreateFromGd2Worker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

class CreateFromGd2PartWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_ARGS(5);
      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, srcX);
      REQ_INT_ARG(2, srcY);
      REQ_INT_ARG(3, width);
      REQ_INT_ARG(4, height);

      CreateFromGd2PartWorker* worker = new CreateFromGd2PartWorker(info.Env(),
        "CreateFromGd2PartWorkerResource");

      worker->path = path;
      worker->srcX = srcX;
      worker->srcY = srcY;
      worker->width = width;
      worker->height = height;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open GD2 file to read part of it");
      }
      image = gdImageCreateFromGd2Part(in, srcX, srcY, width, height);

      fclose(in);
    }

    int srcX;

    int srcY;

    int width;

    int height;

  private:
    CreateFromGd2PartWorker(napi_env env, const char* resource_name)
        : CreateFromWorker(env, resource_name) {
        }
};

// WBMP
class CreateFromWBMPWorker : public CreateFromWorker {
  public:
    static Value DoWork(const CallbackInfo& info) {
      REQ_STR_ARG(0, path);

      CreateFromWBMPWorker* worker = new CreateFromWBMPWorker(info.Env(),
        "CreateFromWBMPWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open WBMP file");
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

      CreateFromWebpWorker* worker = new CreateFromWebpWorker(info.Env(),
        "CreateFromWebpWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open WEBP file");
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

      CreateFromBmpWorker* worker = new CreateFromBmpWorker(info.Env(),
        "CreateFromBmpWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open BMP file");
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

      CreateFromTiffWorker* worker = new CreateFromTiffWorker(info.Env(),
        "CreateFromTiffWorkerResource");

      worker->path = path;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      // execute the async task
      FILE *in; in = fopen(path.c_str(), "rb");
      if (in == nullptr) {
        return SetError("Cannot open TIFF file");
      }
      image = gdImageCreateFromTiff(in);
      fclose(in);
    }

  private:
    CreateFromTiffWorker(napi_env env, const char* resource_name)
      : CreateFromWorker(env, resource_name) {
      }
};


/**
 * FileWorker handling gdImageFile via the AsyncWorker
 * Returns a Promise
 */
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
        return SetError("Cannot save file");
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

      CreateFromFileWorker* worker = new CreateFromFileWorker(info.Env(),
        "CreateFromFileWorkerResource");

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

/**
 * CreateWorker for async creation of images in memory
 */
class CreateWorker : public AsyncWorker {
  public:
    CreateWorker(napi_env env, const char* resource_name, int width, int height, int trueColor)
    : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)),
      _width(width), _height(height), _trueColor(trueColor) {
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
        return SetError("No image created!");
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



/**
 * Save workers
 */
class SaveWorker : public AsyncWorker {
  public:
    SaveWorker(napi_env env, const char* resource_name)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)) {
    }

  protected:
    virtual void OnOK() override {
      _deferred.Resolve(Napi::Boolean::New(Env(), true));
    }

    virtual void OnError(const Napi::Error& e) override {
      _deferred.Reject(Napi::String::New(Env(), e.Message()));
    }

    gdImagePtr* _gdImage;

    int quality;

    int level;

    int foreground;

    Promise::Deferred _deferred;

    std::string path;
};

class SaveJpegWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, quality, -1);

      SaveJpegWorker* worker = new SaveJpegWorker(info.Env(),
        "SaveJpegWorkerResource");

      worker->path = path;
      worker->quality = quality;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save JPEG file");
      }
      gdImageJpeg(*_gdImage, out, quality);
      fclose(out);
    }

  private:
    SaveJpegWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveGifWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);

      SaveGifWorker* worker = new SaveGifWorker(info.Env(),
        "SaveGifWorkerResource");

      worker->path = path;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save GIF file");
      }
      gdImageGif(*_gdImage, out);
      fclose(out);
    }

  private:
    SaveGifWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SavePngWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      SavePngWorker* worker = new SavePngWorker(info.Env(),
        "SavePngWorkerResource");

      worker->path = path;
      worker->level = level;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save PNG file");
      }
      gdImagePngEx(*_gdImage, out, level);
      fclose(out);
    }

  private:
    SavePngWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveWBMPWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      SaveWBMPWorker* worker = new SaveWBMPWorker(info.Env(),
        "SaveWBMPWorkerResource");

      worker->path = path;
      worker->foreground = foreground;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save WBMP file");
      }
      gdImageWBMP(*_gdImage, foreground, out);
      fclose(out);
    }

  private:
    SaveWBMPWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveWebpWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      SaveWebpWorker* worker = new SaveWebpWorker(info.Env(),
        "SaveWebpWorkerResource");

      worker->path = path;
      worker->level = level;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save WEBP file");
      }
      gdImageWebpEx(*_gdImage, out, level);
      fclose(out);
    }

  private:
    SaveWebpWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveGdWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);

      SaveGdWorker* worker = new SaveGdWorker(info.Env(),
        "SaveGdWorkerResource");

      worker->path = path;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save GD file");
      }
      gdImageGd(*_gdImage, out);
      fclose(out);
    }

  private:
    SaveGdWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveGd2Worker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, chunkSize);
      OPT_INT_ARG(2, format, GD2_FMT_RAW);

      SaveGd2Worker* worker = new SaveGd2Worker(info.Env(),
        "SaveGd2WorkerResource");

      worker->path = path;
      worker->chunkSize = chunkSize;
      worker->format = format;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save GD2 file");
      }
      gdImageGd2(*_gdImage, out, chunkSize, format);
      fclose(out);
    }

    int chunkSize;

    int format;

  private:
    SaveGd2Worker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveBmpWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_ARGS(2);
      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, compression);

      SaveBmpWorker* worker = new SaveBmpWorker(info.Env(),
        "SaveBmpWorkerResource");

      worker->path = path;
      worker->compression = compression;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save BMP file");
      }
      gdImageBmp(*_gdImage, out, compression);
      fclose(out);
    }

    int compression;

  private:
    SaveBmpWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};

class SaveTiffWorker : public SaveWorker {
  public:
    static Value DoWork(const CallbackInfo& info, gdImagePtr& gdImage) {
      REQ_STR_ARG(0, path);

      SaveTiffWorker* worker = new SaveTiffWorker(info.Env(),
        "SaveTiffWorkerResource");

      worker->path = path;
      worker->_gdImage = &gdImage;
      worker->Queue();
      return worker->_deferred.Promise();
    }

  protected:
    void Execute() override {
      FILE *out = fopen(path.c_str(), "wb");
      if (out == nullptr) {
        return SetError("Cannot save TIFF file");
      }
      gdImageTiff(*_gdImage, out);
      fclose(out);
    }

  private:
    SaveTiffWorker(napi_env env, const char* resource_name)
      : SaveWorker(env, resource_name) {
      }
};
