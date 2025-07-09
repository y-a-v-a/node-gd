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
class CreateFromWorker : public AsyncWorker
{
public:
  CreateFromWorker(napi_env env, const char *resource_name)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env))
  {
  }

protected:
  virtual void OnOK() override
  {
    // create new instance of Gd::Image with resulting image
    Napi::Value argv = Napi::External<gdImagePtr>::New(Env(), &image);
    Napi::Object instance = Gd::Image::constructor.New({argv});

    // resolve Promise with instance of Gd::Image
    _deferred.Resolve(instance);
  }

  virtual void OnError(const Napi::Error &e) override
  {
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
class CreateFromJpegWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the JPEG file to load.");

    CreateFromJpegWorker *worker = new CreateFromJpegWorker(info.Env(),
                                                            "CreateFromJpegWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open JPEG file");
    }
    image = gdImageCreateFromJpeg(in);
    fclose(in);
  }

private:
  CreateFromJpegWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// PNG
class CreateFromPngWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the PNG file to load.");

    CreateFromPngWorker *worker = new CreateFromPngWorker(info.Env(),
                                                          "CreateFromPngWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open PNG file");
    }
    image = gdImageCreateFromPng(in);
    fclose(in);
  }

private:
  CreateFromPngWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// GIF
class CreateFromGifWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the Gif file to load.");

    CreateFromGifWorker *worker = new CreateFromGifWorker(info.Env(),
                                                          "CreateFromGifWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open GIF file");
    }
    image = gdImageCreateFromGif(in);
    fclose(in);
  }

private:
  CreateFromGifWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// WBMP
class CreateFromWBMPWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the WBMP file to load.");

    CreateFromWBMPWorker *worker = new CreateFromWBMPWorker(info.Env(),
                                                            "CreateFromWBMPWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open WBMP file");
    }
    image = gdImageCreateFromWBMP(in);
    fclose(in);
  }

private:
  CreateFromWBMPWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// Webp
class CreateFromWebpWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the Webp file to load.");

    CreateFromWebpWorker *worker = new CreateFromWebpWorker(info.Env(),
                                                            "CreateFromWebpWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open WEBP file");
    }
    image = gdImageCreateFromWebp(in);
    fclose(in);
  }

private:
  CreateFromWebpWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// Bmp
class CreateFromBmpWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the Bmp file to load.");

    CreateFromBmpWorker *worker = new CreateFromBmpWorker(info.Env(),
                                                          "CreateFromBmpWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open BMP file");
    }
    image = gdImageCreateFromBmp(in);
    fclose(in);
  }

private:
  CreateFromBmpWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// Tiff
class CreateFromTiffWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the TIFF file to load.");

    CreateFromTiffWorker *worker = new CreateFromTiffWorker(info.Env(),
                                                            "CreateFromTiffWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open TIFF file");
    }
    image = gdImageCreateFromTiff(in);
    fclose(in);
  }

private:
  CreateFromTiffWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};

// Avif
#if HAS_LIBAVIF
class CreateFromAvifWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the Avif file to load.");

    CreateFromAvifWorker *worker = new CreateFromAvifWorker(info.Env(),
                                                            "CreateFromAvifWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open Avif file");
    }
    image = gdImageCreateFromAvif(in);
    fclose(in);
  }

private:
  CreateFromAvifWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};
#endif

#if HAS_LIBHEIF
// Heif
class CreateFromHeifWorker : public CreateFromWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the Heif file to load.");

    CreateFromHeifWorker *worker = new CreateFromHeifWorker(info.Env(),
                                                            "CreateFromHeifWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    FILE *in;
    in = fopen(path.c_str(), "rb");
    if (in == nullptr)
    {
      return SetError("Cannot open Heif file");
    }
    image = gdImageCreateFromHeif(in);
    fclose(in);
  }

private:
  CreateFromHeifWorker(napi_env env, const char *resource_name)
      : CreateFromWorker(env, resource_name)
  {
  }
};
#endif

/**
 * FileWorker handling gdImageFile via the AsyncWorker
 * Returns a Promise
 */
class FileWorker : public AsyncWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the image file to load.");

    FileWorker *worker = new FileWorker(info.Env(), "FileWorkerResource");

    worker->path = path;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    _success = gdImageFile(*_gdImage, path.c_str());

    if (!_success)
    {
      return SetError("Cannot save file");
    }
  }

  virtual void OnOK() override
  {
    // resolve Promise with boolean
    _deferred.Resolve(Napi::Boolean::New(Env(), _success));
  }

  virtual void OnError(const Napi::Error &e) override
  {
    // reject Promise with error message
    _deferred.Reject(Napi::String::New(Env(), e.Message()));
  }

private:
  FileWorker(napi_env env, const char *resource_name)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env))
  {
  }

  gdImagePtr *_gdImage;

  Promise::Deferred _deferred;

  std::string path;

  bool _success;
};

/**
 * Async worker class to make the I/O from gdImageCreateFromFile async in JavaScript
 * Returns a Promise
 */
class CreateFromFileWorker : public AsyncWorker
{
public:
  static Value DoWork(const CallbackInfo &info)
  {
    REQ_STR_ARG(0, path, "Argument should be a path to the image file to load.");

    CreateFromFileWorker *worker = new CreateFromFileWorker(info.Env(),
                                                            "CreateFromFileWorkerResource");

    worker->path = path;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    // execute the async task
    image = gdImageCreateFromFile(path.c_str());
  }

  virtual void OnOK() override
  {
    // create new instance of Gd::Image with resulting image
    Napi::Value argv = Napi::External<gdImagePtr>::New(Env(), &image);
    Napi::Object instance = Gd::Image::constructor.New({argv});

    // resolve Promise with instance of Gd::Image
    _deferred.Resolve(instance);
  }

  virtual void OnError(const Napi::Error &e) override
  {
    _deferred.Reject(Napi::String::New(Env(), e.Message()));
  }

  gdImagePtr image;

  Promise::Deferred _deferred;

  std::string path;

private:
  CreateFromFileWorker(napi_env env, const char *resource_name)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env))
  {
  }
};

/**
 * CreateWorker for async creation of images in memory
 */
class CreateWorker : public AsyncWorker
{
public:
  CreateWorker(napi_env env, const char *resource_name, int width, int height, int trueColor)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env)),
        _width(width), _height(height), _trueColor(trueColor)
  {
  }

  Promise::Deferred _deferred;

protected:
  void Execute() override
  {
    if (_trueColor == 0)
    {
      image = gdImageCreate(_width, _height);
    }
    else
    {
      image = gdImageCreateTrueColor(_width, _height);
    }
    if (!image)
    {
      return SetError("No image created!");
    }
  }

  virtual void OnOK() override
  {
    Napi::Value _argv = Napi::External<gdImagePtr>::New(Env(), &image);
    Napi::Object instance = Gd::Image::constructor.New({_argv});

    _deferred.Resolve(instance);
  }

  virtual void OnError(const Napi::Error &e) override
  {
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
class SaveWorker : public AsyncWorker
{
public:
  SaveWorker(napi_env env, const char *resource_name)
      : AsyncWorker(env, resource_name), _deferred(Promise::Deferred::New(env))
  {
  }

protected:
  virtual void OnOK() override
  {
    _deferred.Resolve(Napi::Boolean::New(Env(), true));
  }

  virtual void OnError(const Napi::Error &e) override
  {
    _deferred.Reject(Napi::String::New(Env(), e.Message()));
  }

  gdImagePtr *_gdImage;

  int quality;

  int level;

  int foreground;

  Promise::Deferred _deferred;

  std::string path;
};

class SaveJpegWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the JPEG.");
    OPT_INT_ARG(1, quality, -1);

    SaveJpegWorker *worker = new SaveJpegWorker(info.Env(),
                                                "SaveJpegWorkerResource");

    worker->path = path;
    worker->quality = quality;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save JPEG file");
    }
    gdImageJpeg(*_gdImage, out, quality);
    fclose(out);
  }

private:
  SaveJpegWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

class SaveGifWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the Gif.");

    SaveGifWorker *worker = new SaveGifWorker(info.Env(),
                                              "SaveGifWorkerResource");

    worker->path = path;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save GIF file");
    }
    gdImageGif(*_gdImage, out);
    fclose(out);
  }

private:
  SaveGifWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

class SavePngWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the PNG.");
    OPT_INT_ARG(1, level, -1);

    SavePngWorker *worker = new SavePngWorker(info.Env(),
                                              "SavePngWorkerResource");

    worker->path = path;
    worker->level = level;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save PNG file");
    }
    gdImagePngEx(*_gdImage, out, level);
    fclose(out);
  }

private:
  SavePngWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

class SaveWBMPWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the WBMP.");
    REQ_INT_ARG(1, foreground, "The index of the foreground color should be supplied.");

    SaveWBMPWorker *worker = new SaveWBMPWorker(info.Env(),
                                                "SaveWBMPWorkerResource");

    worker->path = path;
    worker->foreground = foreground;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save WBMP file");
    }
    gdImageWBMP(*_gdImage, foreground, out);
    fclose(out);
  }

private:
  SaveWBMPWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

class SaveWebpWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the Webp.");
    OPT_INT_ARG(1, level, -1);

    SaveWebpWorker *worker = new SaveWebpWorker(info.Env(),
                                                "SaveWebpWorkerResource");

    worker->path = path;
    worker->level = level;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save WEBP file");
    }
    gdImageWebpEx(*_gdImage, out, level);
    fclose(out);
  }

private:
  SaveWebpWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

class SaveBmpWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_ARGS(2, "destination file path and compression flag.");
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the BMP.");
    REQ_INT_ARG(1, compression, "BMP compression flag should be either 0 (no compression) or 1 (compression).");

    SaveBmpWorker *worker = new SaveBmpWorker(info.Env(),
                                              "SaveBmpWorkerResource");

    worker->path = path;
    worker->compression = compression;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save BMP file");
    }
    gdImageBmp(*_gdImage, out, compression);
    fclose(out);
  }

  int compression;

private:
  SaveBmpWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

class SaveTiffWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the TIFF.");

    SaveTiffWorker *worker = new SaveTiffWorker(info.Env(),
                                                "SaveTiffWorkerResource");

    worker->path = path;
    worker->_gdImage = &gdImage;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save TIFF file");
    }
    gdImageTiff(*_gdImage, out);
    fclose(out);
  }

private:
  SaveTiffWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};

#if HAS_LIBHEIF
class SaveHeifWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the Heif.");
    OPT_INT_ARG(1, quality, -1);
    OPT_INT_ARG(2, codec_param, 1);
    OPT_STR_ARG(3, chroma_param, "444");

    const char *chroma = chroma_param.c_str();

    SaveHeifWorker *worker = new SaveHeifWorker(info.Env(),
                                                "SaveHeifWorkerResource");

    worker->path = path;
    worker->_gdImage = &gdImage;
    worker->quality = quality;
    if (codec_param == 1)
    {
      worker->codec = GD_HEIF_CODEC_HEVC;
    }
    else if (codec_param == 4)
    {
      worker->codec = GD_HEIF_CODEC_AV1;
    }
    else
    {
      worker->codec = GD_HEIF_CODEC_UNKNOWN;
    }
    worker->chroma = chroma;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save Heif file");
    }
    gdImageHeifEx(*_gdImage, out, quality, codec, chroma);
    fclose(out);
  }

  gdHeifCodec codec;

  const char *chroma;

private:
  SaveHeifWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};
#endif

#if HAS_LIBAVIF
class SaveAvifWorker : public SaveWorker
{
public:
  static Value DoWork(const CallbackInfo &info, gdImagePtr &gdImage)
  {
    REQ_STR_ARG(0, path, "Argument should be a path and filename to the destination to save the Avif.");
    OPT_INT_ARG(1, quality, -1);
    OPT_INT_ARG(2, speed, -1);

    SaveAvifWorker *worker = new SaveAvifWorker(info.Env(),
                                                "SaveAvifWorkerResource");

    worker->path = path;
    worker->_gdImage = &gdImage;
    worker->quality = quality;
    worker->speed = speed;
    worker->Queue();
    return worker->_deferred.Promise();
  }

protected:
  void Execute() override
  {
    FILE *out = fopen(path.c_str(), "wb");
    if (out == nullptr)
    {
      return SetError("Cannot save Avif file");
    }
    
    // Check if image is valid
    if (*_gdImage == nullptr)
    {
      fclose(out);
      return SetError("Invalid image for AVIF conversion");
    }
    
    // Try to save AVIF - this can fail if codec is not available
    gdImageAvifEx(*_gdImage, out, quality, speed);
    fclose(out);
  }

  int speed;

private:
  SaveAvifWorker(napi_env env, const char *resource_name)
      : SaveWorker(env, resource_name)
  {
  }
};
#endif
