/**
 * Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>
 * Copyright (c) 2014-2020, Vincent Bruijn <vebruijn@gmail.com>
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
#include <sstream>
#include <cstring>
#include "node_gd.h"
#include "createworker.cc"
#if SUPPORTS_GD_2_1_0
  #include <gd_errors.h>
#endif


#if SUPPORTS_GD_2_1_0
void nodeGdErrorWrapper(int priority, const char *format, va_list args)
{
  static char error[256];
  static char msg[256];
  vsprintf(msg, format, args);

  switch (priority) {
  case GD_ERROR:
    snprintf(error, 256, "GD Error: %s", msg);
    break;
  case GD_WARNING:
    snprintf(error, 256, "GD Warning: %s", msg);
    break;
  case GD_NOTICE:
    snprintf(error, 256, "GD Notice: %s", msg);
    break;
  case GD_INFO:
    snprintf(error, 256, "GD Info: %s", msg);
    break;
  case GD_DEBUG:
    snprintf(error, 256, "GD Debug: %s", msg);
    break;
  }
  // Napi::Error::New(info.Env(), Napi::String::New(info.Env(), error)).ThrowAsJavaScriptException();
  // return info.Env().Null();
}
#endif

// Since gd 2.0.28, these are always built in
#define GD_GIF 1
#define GD_GIFANIM 1
#define GD_OPENPOLYGON 1

#define REQ_ARGS(N)                                                     \
  if (info.Length() < (N)) {                                            \
    Napi::Error::New(info.Env(), "Expected " #N " arguments").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }

#define REQ_STR_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I].IsString()) {                    \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be a string").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }                                                                     \
  std::string VAR = info[I].As<Napi::String>().Utf8Value().c_str();

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (info.Length() <= (I) || !info[I].IsNumber()) {                    \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be an integer").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }                                                                     \
  VAR = info[I].ToNumber();

#define INT_ARG_RANGE(I, PROP)                                          \
  if ((I) < 1) {                                                        \
    Napi::RangeError::New(info.Env(), "Value for " #PROP  " must be greater than 0").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }

#define REQ_FN_ARG(I, VAR)                                              \
  if (info.Length() <= (I) || !info[I].IsFunction()) {                  \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be a function").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }                                                                     \
  Napi::Function VAR = info[I].As<Napi::Function>();

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (info.Length() <= (I) || !info[I].IsNumber()) {                    \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be a number").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }                                                                     \
  VAR = info[I].ToNumber();

#define REQ_EXT_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I].IsExternal()) {                  \
    Napi::TypeError::New(info.Env(), "Argument " #I " invalid").ThrowAsJavaScriptException(); \
  }                                                                     \
  Napi::External<gdImagePtr> VAR = info[I].As<Napi::External<gdImagePtr>>();

#define REQ_IMG_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I].IsObject()) {                    \
    Napi::TypeError::New(info.Env(), "Argument " #I " must be an object").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }                                                                     \
  Gd::Image* _obj_ = Napi::ObjectWrap<Gd::Image>::Unwrap(info[I].As<Napi::Object>()); \
  gdImagePtr VAR = _obj_->getGdImagePtr();

#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (info.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (info[I].IsNumber()) {                                      \
    VAR = info[I].ToNumber();                                           \
  } else {                                                              \
    Napi::TypeError::New(info.Env(), "Optional argument " #I " must be an integer").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }

#define OPT_BOOL_ARG(I, VAR, DEFAULT)                                   \
  bool VAR;                                                             \
  if (info.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (info[I].IsBoolean()) {                                     \
    VAR = info[I].ToBoolean();                                          \
  } else {                                                              \
    Napi::TypeError::New(info.Env(), "Optional argument " #I " must be a boolean").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }

#define RETURN_IMAGE(IMG)                                               \
  if (!IMG) {                                                           \
    return info.Env().Null();                                           \
  } else {                                                              \
    Napi::Value argv = Napi::External<gdImagePtr>::New(info.Env(), &IMG); \
    Napi::Object instance = Gd::Image::constructor.New({argv});         \
    return instance;                                                    \
  }

#define DECLARE_CREATE_FROM(TYPE)                                       \
  Napi::Value Gd::CreateFrom##TYPE(const Napi::CallbackInfo& info) {    \
    REQ_STR_ARG(0, path);                                               \
    FILE *in; in = fopen(path.c_str(), "rb");                           \
    gdImagePtr im = gdImageCreateFrom##TYPE(in);                        \
    fclose(in);                                                         \
    RETURN_IMAGE(im)                                                    \
  }                                                                     \
  Napi::Value Gd::CreateFrom##TYPE##Ptr(const Napi::CallbackInfo& info) { \
    REQ_ARGS(1);                                                        \
    ASSERT_IS_BUFFER(info[0]);                                          \
    gdImagePtr im;                                                      \
    Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();       \
    char *buffer_data = buffer.Data();                                  \
    size_t buffer_length = buffer.Length();                             \
    im = gdImageCreateFrom##TYPE##Ptr(buffer_length, buffer_data);      \
    RETURN_IMAGE(im)                                                    \
  }

#define ASSERT_IS_BUFFER(val)                                           \
  if (!val.IsBuffer()) {                                                \
    Napi::TypeError::New(info.Env(), "Argument not a Buffer").ThrowAsJavaScriptException(); \
    return info.Env().Null();                                           \
  }

#define RETURN_DATA                                                     \
  Napi::Buffer<char> result = Napi::Buffer<char>::Copy(info.Env(), data, size); \
  delete[] data;                                                        \
  return result;

#define COLOR_ANTIALIASED    gdAntiAliased
#define COLOR_BRUSHED        gdBrushed
#define COLOR_STYLED         gdStyled
#define COLOR_STYLEDBRUSHED  gdStyledBrushed
#define COLOR_TITLED         gdTiled
#define COLOR_TRANSPARENT    gdTransparent

Napi::Object Gd::Init(Napi::Env env, Napi::Object exports) {
#if SUPPORTS_GD_2_1_0
  gdSetErrorMethod(nodeGdErrorWrapper);
#endif
  exports.Set("COLOR_ANTIALIASED", Napi::Number::New(env, COLOR_ANTIALIASED));
  exports.Set("COLOR_BRUSHED", Napi::Number::New(env, COLOR_BRUSHED));
  exports.Set("COLOR_STYLED", Napi::Number::New(env, COLOR_STYLED));
  exports.Set("COLOR_STYLEDBRUSHED", Napi::Number::New(env, COLOR_STYLEDBRUSHED));
  exports.Set("COLOR_TITLED", Napi::Number::New(env, COLOR_TITLED));
  exports.Set("COLOR_TRANSPARENT", Napi::Number::New(env, COLOR_TRANSPARENT));

#ifdef HAVE_LIBTIFF
  exports.Set("GD_TIFF", Napi::Number::New(env, GD_TIFF));
#endif

#ifdef HAVE_LIBXPM
  exports.Set("GD_XPM", Napi::Number::New(env, GD_XPM));
#endif

#ifdef HAVE_LIBJPEG
  exports.Set("GD_JPEG", Napi::Number::New(env, GD_JPEG));
#endif

#ifdef HAVE_LIBFONTCONFIG
  exports.Set("GD_FONTCONFIG", Napi::Number::New(env, GD_FONTCONFIG));
#endif

#ifdef HAVE_LIBFREETYPE
  exports.Set("GD_FREETYPE", Napi::Number::New(env, GD_FREETYPE));
#endif

#ifdef HAVE_LIBPNG
  exports.Set("GD_PNG", Napi::Number::New(env, GD_PNG));
#endif

#ifdef HAVE_LIBWEBP
  exports.Set("GD_WEBP", Napi::Number::New(env, GD_WEBP));
#endif

#ifdef HAVE_LIBVPX
  exports.Set("GD_VPX", Napi::Number::New(env, GD_VPX));
#endif

  exports.Set("GD_GIF", Napi::Number::New(env, GD_GIF));
  exports.Set("GD_GIFANIM", Napi::Number::New(env, GD_GIFANIM));
  exports.Set("GD_OPENPOLYGON", Napi::Number::New(env, GD_OPENPOLYGON));

  // Image creation, loading and saving
  exports.Set(Napi::String::New(env, "create"), Napi::Function::New(env, ImageCreate));
  exports.Set(Napi::String::New(env, "createSync"), Napi::Function::New(env, ImageCreateSync));
  exports.Set(Napi::String::New(env, "createTrueColor"), Napi::Function::New(env, ImageCreateTrueColor));
  exports.Set(Napi::String::New(env, "createTrueColorSync"), Napi::Function::New(env, ImageCreateTrueColorSync));
  exports.Set(Napi::String::New(env, "createFromJpeg"), Napi::Function::New(env, CreateFromJpeg));
  exports.Set(Napi::String::New(env, "createFromJpegPtr"), Napi::Function::New(env, CreateFromJpegPtr));
  exports.Set(Napi::String::New(env, "createFromPng"), Napi::Function::New(env, CreateFromPng));
  exports.Set(Napi::String::New(env, "createFromPngPtr"), Napi::Function::New(env, CreateFromPngPtr));
  exports.Set(Napi::String::New(env, "createFromGif"), Napi::Function::New(env, CreateFromGif));
  exports.Set(Napi::String::New(env, "createFromGifPtr"), Napi::Function::New(env, CreateFromGifPtr));
  exports.Set(Napi::String::New(env, "createFromGd2"), Napi::Function::New(env, CreateFromGd2));
  exports.Set(Napi::String::New(env, "createFromGd2Ptr"), Napi::Function::New(env, CreateFromGd2Ptr));
  exports.Set(Napi::String::New(env, "createFromGd2Part"), Napi::Function::New(env, CreateFromGd2Part));
  exports.Set(Napi::String::New(env, "createFromGd2PartPtr"), Napi::Function::New(env, CreateFromGd2PartPtr));
  exports.Set(Napi::String::New(env, "createFromWBMP"), Napi::Function::New(env, CreateFromWBMP));
  exports.Set(Napi::String::New(env, "createFromWBMPPtr"), Napi::Function::New(env, CreateFromWBMPPtr));
#if HAS_LIBWEBP
  exports.Set(Napi::String::New(env, "createFromWebp"), Napi::Function::New(env, CreateFromWebp));
  exports.Set(Napi::String::New(env, "createFromWebpPtr"), Napi::Function::New(env, CreateFromWebpPtr));
#endif

#if SUPPORTS_GD_2_1_0
  exports.Set(Napi::String::New(env, "createFromBmp"), Napi::Function::New(env, CreateFromBmp));
  exports.Set(Napi::String::New(env, "createFromBmpPtr"), Napi::Function::New(env, CreateFromBmpPtr));
#endif
#if HAS_LIBTIFF
  exports.Set(Napi::String::New(env, "createFromTiff"), Napi::Function::New(env, CreateFromTiff));
  exports.Set(Napi::String::New(env, "createFromTiffPtr"), Napi::Function::New(env, CreateFromTiffPtr));
#endif

#if SUPPORTS_GD_2_1_1
  exports.Set(Napi::String::New(env, "createFromFile"), Napi::Function::New(env, CreateFromFile));
#endif
  exports.Set(Napi::String::New(env, "trueColor"), Napi::Function::New(env, TrueColor));
  exports.Set(Napi::String::New(env, "trueColorAlpha"), Napi::Function::New(env, TrueColorAlpha));
  exports.Set(Napi::String::New(env, "getGDVersion"), Napi::Function::New(env, GdVersionGetter));

  Gd::Image::Init(env, exports);

  return exports;
}

Napi::Value Gd::ImageCreate(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_INT_ARG(0, width);
  REQ_INT_ARG(1, height);
  REQ_FN_ARG(2, cb);

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  CreateWorker* cw = new CreateWorker(cb, width, height, 0);
  cw->Queue();

  return info.Env().Undefined();
}

Napi::Value Gd::ImageCreateSync(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, width);
  REQ_INT_ARG(1, height);

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  gdImagePtr im = gdImageCreate(width, height);

  RETURN_IMAGE(im);
}

Napi::Value Gd::ImageCreateTrueColor(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_INT_ARG(0, width);
  REQ_INT_ARG(1, height);
  REQ_FN_ARG(2, cb);

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  CreateWorker* cw = new CreateWorker(cb, width, height, 1);
  cw->Queue();

  return info.Env().Undefined();
}

Napi::Value Gd::ImageCreateTrueColorSync(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, width);
  REQ_INT_ARG(1, height);

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  gdImagePtr im = gdImageCreateTrueColor(width, height);

  RETURN_IMAGE(im);
}

DECLARE_CREATE_FROM(Jpeg);
DECLARE_CREATE_FROM(Png);
DECLARE_CREATE_FROM(Gif);
DECLARE_CREATE_FROM(Gd2);
DECLARE_CREATE_FROM(WBMP);
#if HAS_LIBWEBP
DECLARE_CREATE_FROM(Webp);
#endif

#if SUPPORTS_GD_2_1_0
DECLARE_CREATE_FROM(Bmp);
#endif
#if HAS_LIBTIFF
DECLARE_CREATE_FROM(Tiff);
#endif

#if SUPPORTS_GD_2_1_1
Napi::Value Gd::CreateFromFile(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);

  gdImagePtr im = gdImageCreateFromFile(path.c_str());

  RETURN_IMAGE(im);
}
#endif

Napi::Value Gd::CreateFromGd2Part(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_STR_ARG(0, path);
  REQ_INT_ARG(1, srcX);
  REQ_INT_ARG(2, srcY);
  REQ_INT_ARG(3, width);
  REQ_INT_ARG(4, height);

  FILE *in;
  in = fopen(path.c_str(), "rb");
  gdImagePtr im = gdImageCreateFromGd2Part(in, srcX, srcY, width, height);
  fclose(in);

  RETURN_IMAGE(im);
}

Napi::Value Gd::CreateFromGd2PartPtr(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(1, srcX);
  REQ_INT_ARG(2, srcY);
  REQ_INT_ARG(3, width);
  REQ_INT_ARG(4, height);

  const char *str = info[0].As<Napi::String>().Utf8Value().c_str();
  ssize_t len = strlen(str);

  gdImagePtr im = gdImageCreateFromGd2PartPtr(len, &str, srcX, srcY, width, height);

  RETURN_IMAGE(im);
}

Napi::Value Gd::TrueColor(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_INT_ARG(0, r);
  REQ_INT_ARG(1, g);
  REQ_INT_ARG(2, b);

  uint32_t result = gdTrueColor(r, g, b);

  return Napi::Number::New(info.Env(), result);
}

Napi::Value Gd::TrueColorAlpha(const Napi::CallbackInfo& info) {
  REQ_ARGS(4);
  REQ_INT_ARG(0, r);
  REQ_INT_ARG(1, g);
  REQ_INT_ARG(2, b);
  REQ_INT_ARG(3, a);

  uint32_t result = gdTrueColorAlpha(r, g, b, a);

  return Napi::Number::New(info.Env(), result);
}

Napi::Value Gd::GdVersionGetter(const Napi::CallbackInfo& info) {
  std::stringstream version_string;
  version_string << GD_MAJOR_VERSION << "." << GD_MINOR_VERSION << "." << GD_RELEASE_VERSION;
  Napi::String s = Napi::String::New(info.Env(), version_string.str());
  return s;
}

/**
 * Image is a subclass of Gd
 */
Napi::Object Gd::Image::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func =
    DefineClass(env, "Image", {
      InstanceMethod("destroy", &Gd::Image::Destroy),

      /**
       * Type transform functions
       */
      InstanceMethod("jpeg", &Gd::Image::Jpeg),
      InstanceMethod("jpegPtr", &Gd::Image::JpegPtr),
      InstanceMethod("png", &Gd::Image::Png),
      InstanceMethod("pngPtr", &Gd::Image::PngPtr),
      InstanceMethod("gif", &Gd::Image::Gif),
      InstanceMethod("gifPtr", &Gd::Image::GifPtr),
      InstanceMethod("gd", &Gd::Image::Gd),
      InstanceMethod("gdPtr", &Gd::Image::GdPtr),
      InstanceMethod("gd2", &Gd::Image::Gd2),
      InstanceMethod("gd2Ptr", &Gd::Image::Gd2Ptr),
      InstanceMethod("wbmp", &Gd::Image::WBMP),
      InstanceMethod("wbmpPtr", &Gd::Image::WBMPPtr),
#if HAS_LIBWEBP
      InstanceMethod("webp", &Gd::Image::Webp),
      InstanceMethod("webpPtr", &Gd::Image::WebpPtr),
#endif
#if SUPPORTS_GD_2_1_0
      InstanceMethod("bmp", &Gd::Image::Bmp),
      InstanceMethod("bmpPtr", &Gd::Image::BmpPtr),
#endif
#if HAS_LIBTIFF
      InstanceMethod("tiff", &Gd::Image::Tiff),
      InstanceMethod("tiffPtr", &Gd::Image::TiffPtr),
#endif
#if SUPPORTS_GD_2_1_1
      InstanceMethod("file", &Gd::Image::File),
      InstanceMethod("fileCallback", &Gd::Image::FileCallback),
#endif

      /**
       * Drawing Functions
       */
      InstanceMethod("setPixel", &Gd::Image::SetPixel),
      InstanceMethod("line", &Gd::Image::Line),
      InstanceMethod("dashedLine", &Gd::Image::DashedLine),
      InstanceMethod("polygon", &Gd::Image::Polygon),
      InstanceMethod("openPolygon", &Gd::Image::OpenPolygon),
      InstanceMethod("filledPolygon", &Gd::Image::FilledPolygon),
      InstanceMethod("rectangle", &Gd::Image::Rectangle),
      InstanceMethod("filledRectangle", &Gd::Image::FilledRectangle),
      InstanceMethod("arc", &Gd::Image::Arc),
      InstanceMethod("filledArc", &Gd::Image::FilledArc),
#if SUPPORTS_GD_2_1_0
      InstanceMethod("ellipse", &Gd::Image::Ellipse),
#endif
      InstanceMethod("filledEllipse", &Gd::Image::FilledEllipse),
      InstanceMethod("fillToBorder", &Gd::Image::FillToBorder),
      InstanceMethod("fill", &Gd::Image::Fill),
      InstanceMethod("setAntiAliased", &Gd::Image::SetAntiAliased),
      InstanceMethod("setAntiAliasedDontBlend", &Gd::Image::SetAntiAliasedDontBlend),
      InstanceMethod("setBrush", &Gd::Image::SetBrush),
      InstanceMethod("setTile", &Gd::Image::SetTile),
      InstanceMethod("setStyle", &Gd::Image::SetStyle),
      InstanceMethod("setThickness", &Gd::Image::SetThickness),
      InstanceMethod("alphaBlending", &Gd::Image::AlphaBlending),
      InstanceMethod("saveAlpha", &Gd::Image::SaveAlpha),
      InstanceMethod("setClip", &Gd::Image::SetClip),
      InstanceMethod("getClip", &Gd::Image::GetClip),
#if SUPPORTS_GD_2_1_0
      InstanceMethod("setResolution", &Gd::Image::SetResolution),
#endif

      /**
       * Query Functions
       */
      InstanceMethod("getPixel", &Gd::Image::GetPixel),
      InstanceMethod("getTrueColorPixel", &Gd::Image::GetTrueColorPixel),
      InstanceMethod("imageColorAt", &Gd::Image::ImageColorAt),
      InstanceMethod("getBoundsSafe", &Gd::Image::GetBoundsSafe),

      /**
       * Font and Text Handling Functions
       */
      InstanceMethod("stringFTBBox", &Gd::Image::StringFTBBox),
      InstanceMethod("stringFT", &Gd::Image::StringFT),
      InstanceMethod("stringFTEx", &Gd::Image::StringFTEx),
      InstanceMethod("stringFTCircle", &Gd::Image::StringFTCircle),

      /**
       * Color Handling Functions
       */
      InstanceMethod("colorAllocate", &Gd::Image::ColorAllocate),
      InstanceMethod("colorAllocateAlpha", &Gd::Image::ColorAllocateAlpha),
      InstanceMethod("colorClosest", &Gd::Image::ColorClosest),
      InstanceMethod("colorClosestAlpha", &Gd::Image::ColorClosestAlpha),
      InstanceMethod("colorClosestHWB", &Gd::Image::ColorClosestHWB),
      InstanceMethod("colorExact", &Gd::Image::ColorExact),
      InstanceMethod("colorResolve", &Gd::Image::ColorResolve),
      InstanceMethod("colorResolveAlpha", &Gd::Image::ColorResolveAlpha),
      InstanceMethod("red", &Gd::Image::Red),
      InstanceMethod("green", &Gd::Image::Green),
      InstanceMethod("blue", &Gd::Image::Blue),
      InstanceMethod("alpha", &Gd::Image::Alpha),
      InstanceMethod("getTransparent", &Gd::Image::GetTransparent),
      InstanceMethod("colorDeallocate", &Gd::Image::ColorDeallocate),

      /**
       * Color Manipulation Functions
       */
      InstanceMethod("colorTransparent", &Gd::Image::ColorTransparent),
#if SUPPORTS_GD_2_1_0
      InstanceMethod("colorReplace", &Gd::Image::ColorReplace),
      InstanceMethod("colorReplaceThreshold", &Gd::Image::ColorReplaceThreshold),
      InstanceMethod("colorReplaceArray", &Gd::Image::ColorReplaceArray),

      /**
       * Effects / filters
       */
      InstanceMethod("grayscale", &Gd::Image::GrayScale),
      InstanceMethod("gaussianBlur", &Gd::Image::GaussianBlur),
      InstanceMethod("negate", &Gd::Image::Negate),
      InstanceMethod("brightness", &Gd::Image::Brightness),
      InstanceMethod("contrast", &Gd::Image::Contrast),
      InstanceMethod("selectiveBlur", &Gd::Image::SelectiveBlur),
      InstanceMethod("flipHorizontal", &Gd::Image::FlipHorizontal),
      InstanceMethod("flipVertical", &Gd::Image::FlipVertical),
      InstanceMethod("flipBoth", &Gd::Image::FlipBoth),
      InstanceMethod("crop", &Gd::Image::Crop),
      InstanceMethod("cropAuto", &Gd::Image::CropAuto),
      InstanceMethod("cropThreshold", &Gd::Image::CropThreshold),
#endif
#if SUPPORTS_GD_2_1_1
      InstanceMethod("emboss", &Gd::Image::Emboss),
#endif
      InstanceMethod("sharpen", &Gd::Image::Sharpen),
      InstanceMethod("createPaletteFromTrueColor", &Gd::Image::CreatePaletteFromTrueColor),
      InstanceMethod("trueColorToPalette", &Gd::Image::TrueColorToPalette),
#if SUPPORTS_GD_2_1_0
      InstanceMethod("paletteToTrueColor", &Gd::Image::PaletteToTrueColor),
      InstanceMethod("colorMatch", &Gd::Image::ColorMatch),
#endif
      InstanceMethod("gifAnimBegin", &Gd::Image::GifAnimBegin),
      InstanceMethod("gifAnimAdd", &Gd::Image::GifAnimAdd),
      InstanceMethod("gifAnimEnd", &Gd::Image::GifAnimEnd),

      /**
       * Copying and Resizing Functions
       */
      InstanceMethod("copy", &Gd::Image::Copy),
      InstanceMethod("copyResized", &Gd::Image::CopyResized),
      InstanceMethod("copyResampled", &Gd::Image::CopyResampled),
      InstanceMethod("copyRotated", &Gd::Image::CopyRotated),
      InstanceMethod("copyMerge", &Gd::Image::CopyMerge),
      InstanceMethod("copyMergeGray", &Gd::Image::CopyMergeGray),
      InstanceMethod("paletteCopy", &Gd::Image::PaletteCopy),
      InstanceMethod("squareToCircle", &Gd::Image::SquareToCircle),

      /**
       * Miscellaneous Functions
       */
      InstanceMethod("compare", &Gd::Image::Compare),

      /**
       * Instance getters and setters
       */
      InstanceAccessor("trueColor", &Gd::Image::TrueColorGetter, 0, napi_default, nullptr),
      InstanceAccessor("width", &Gd::Image::WidthGetter, 0, napi_default, nullptr),
      InstanceAccessor("height", &Gd::Image::HeightGetter, 0, napi_default, nullptr),
      InstanceAccessor("interlace", &Gd::Image::InterlaceGetter, &Gd::Image::InterlaceSetter, napi_default, nullptr),
      InstanceAccessor("colorsTotal", &Gd::Image::ColorsTotalGetter, 0, napi_default, nullptr)
  });


  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
  exports.Set("Image", func);
  return exports;
}

Gd::Image::Image(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Image>(info) {
  REQ_EXT_ARG(0, image);

  gdImagePtr *imgPtr = image.Data();

  this->_image = *imgPtr;
}

Gd::Image::~Image() {
  if(this->_image) {
    gdImageDestroy(this->_image);
    this->_image = NULL;
  }
}

/**
 * Destruction, Loading and Saving Functions
 */
Napi::Value Gd::Image::Destroy(const Napi::CallbackInfo& info) {
  if(this->_image){
    gdImageDestroy(this->_image);
    this->_image = NULL;
  }

  this->_image = NULL;
  return info.Env().Undefined();
}

Napi::Value Gd::Image::Jpeg(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);
  OPT_INT_ARG(1, quality, -1);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageJpeg(this->_image, out, quality);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::JpegPtr(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, quality, -1);

  int size;
  char *data = (char*)gdImageJpegPtr(this->_image, &size, quality);

  RETURN_DATA;
}

Napi::Value Gd::Image::Gif(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageGif(this->_image, out);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::GifPtr(const Napi::CallbackInfo& info) {
  int size;
  char *data = (char*)gdImageGifPtr(this->_image, &size);

  RETURN_DATA
}

Napi::Value Gd::Image::Png(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);
  OPT_INT_ARG(1, level, -1);

  FILE *out = fopen(path.c_str(), "wb");
  gdImagePngEx(this->_image, out, level);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::PngPtr(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, level, -1);

  int size;
  char *data = (char*)gdImagePngPtrEx(this->_image, &size, level);

  RETURN_DATA
}

Napi::Value Gd::Image::WBMP(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);
  REQ_INT_ARG(1, foreground);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageWBMP(this->_image, foreground, out);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::WBMPPtr(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, foreground);

  int size;
  char *data = (char*)gdImageWBMPPtr(this->_image, &size, foreground);

  RETURN_DATA
}

#if HAS_LIBWEBP
Napi::Value Gd::Image::Webp(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);
  OPT_INT_ARG(1, level, -1);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageWebpEx(this->_image, out, level);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::WebpPtr(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, level, -1);

  int size;
  char *data = (char*)gdImageWebpPtrEx(this->_image, &size, level);

  RETURN_DATA
}
#endif

Napi::Value Gd::Image::Gd(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageGd(this->_image, out);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::GdPtr(const Napi::CallbackInfo& info) {
  int size;
  char *data = (char*)gdImageGdPtr(this->_image, &size);

  RETURN_DATA
}

Napi::Value Gd::Image::Gd2(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);
  REQ_INT_ARG(1, chunkSize);
  OPT_INT_ARG(2, format, GD2_FMT_RAW);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageGd2(this->_image, out, chunkSize, format);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::Gd2Ptr(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, chunkSize);
  OPT_INT_ARG(1, format, GD2_FMT_RAW);

  int size;
  char *data = (char*)gdImageGd2Ptr(this->_image, chunkSize, format, &size);

  RETURN_DATA
}

#if SUPPORTS_GD_2_1_0
Napi::Value Gd::Image::Bmp(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_STR_ARG(0, path);
  REQ_INT_ARG(1, compression);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageBmp(this->_image, out, compression);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::BmpPtr(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, compression, 0);

  int size;
  char *data = (char*)gdImageBmpPtr(this->_image, &size, compression);

  RETURN_DATA
}
#endif

#if HAS_LIBTIFF
Napi::Value Gd::Image::Tiff(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);

  FILE *out = fopen(path.c_str(), "wb");
  gdImageTiff(this->_image, out);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::TiffPtr(const Napi::CallbackInfo& info) {
  int size;
  char *data = (char*)gdImageTiffPtr(this->_image, &size);

  RETURN_DATA
}
#endif

#if SUPPORTS_GD_2_1_1
Napi::Value Gd::Image::File(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);

  gdImageFile(this->_image, path.c_str());

  return info.This();
}

Napi::Value Gd::Image::FileCallback(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_STR_ARG(0, path);
  REQ_FN_ARG(1, cb);

  bool result = gdImageFile(this->_image, path.c_str());
  if (result == false) {
    cb.Call({
      Napi::String::New(info.Env(), "Unable to write file.")
    });
  } else {
    cb.Call({
      info.Env().Null()
    });
  }

  return info.This();
}
#endif

/**
 * Drawing Functions
 */
Napi::Value Gd::Image::SetPixel(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);
  REQ_INT_ARG(2, color);

  gdImageSetPixel(this->_image, x, y, color);

  return info.This();
}

Napi::Value Gd::Image::Line(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(0, x1);
  REQ_INT_ARG(1, y1);
  REQ_INT_ARG(2, x2);
  REQ_INT_ARG(3, y2);
  REQ_INT_ARG(4, color);

  gdImageLine(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::DashedLine(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(0, x1);
  REQ_INT_ARG(1, y1);
  REQ_INT_ARG(2, x2);
  REQ_INT_ARG(3, y2);
  REQ_INT_ARG(4, color);

  gdImageDashedLine(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::Polygon(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(1, color);

  if (!info[0].IsArray()) {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::String x = Napi::String::New(info.Env(), "x");
  Napi::String y = Napi::String::New(info.Env(), "y");

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  gdPoint *points =  new gdPoint[len];

  for(unsigned int i = 0; i < len; i++) {
    Napi::Value v = array.Get(i);
    if (!v.IsObject()) continue;

    Napi::Object o = v.ToObject();
    if ( !o.Has(x) || !o.Has(y)) continue;

    points[i].x = o.Get(x).As<Napi::Number>().Int32Value();
    points[i].y = o.Get(y).As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImagePolygon(this->_image, points, _len, color);

  delete[] points;

  return info.This();
}

Napi::Value Gd::Image::OpenPolygon(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(1, color);

  if (!info[0].IsArray()) {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::String x = Napi::String::New(info.Env(), "x");
  Napi::String y = Napi::String::New(info.Env(), "y");

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  gdPoint *points =  new gdPoint[len];

  for(unsigned int i = 0; i < len; i++) {
    Napi::Value v = array.Get(i);
    if (!v.IsObject()) continue;

    Napi::Object o = v.ToObject();
    if ( !o.Has(x) || !o.Has(y)) continue;

    points[i].x = o.Get(x).As<Napi::Number>().Int32Value();
    points[i].y = o.Get(y).As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImageOpenPolygon(this->_image, points, _len, color);

  delete[] points;

  return info.This();
}

Napi::Value Gd::Image::FilledPolygon(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(1, color);

  if (!info[0].IsArray()) {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::String x = Napi::String::New(info.Env(), "x");
  Napi::String y = Napi::String::New(info.Env(), "y");

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  gdPoint *points =  new gdPoint[len];

  for(unsigned int i = 0; i < len; i++) {
    Napi::Value v = array.Get(i);
    if (!v.IsObject()) continue;

    Napi::Object o = v.ToObject();
    if ( !o.Has(x) || !o.Has(y)) continue;

    points[i].x = o.Get(x).As<Napi::Number>().Int32Value();
    points[i].y = o.Get(y).As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImageFilledPolygon(this->_image, points, _len, color);

  delete[] points;

  return info.This();
}

Napi::Value Gd::Image::Rectangle(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(0, x1);
  REQ_INT_ARG(1, y1);
  REQ_INT_ARG(2, x2);
  REQ_INT_ARG(3, y2);
  REQ_INT_ARG(4, color);

  gdImageRectangle(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::FilledRectangle(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(0, x1);
  REQ_INT_ARG(1, y1);
  REQ_INT_ARG(2, x2);
  REQ_INT_ARG(3, y2);
  REQ_INT_ARG(4, color);

  gdImageFilledRectangle(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::Arc(const Napi::CallbackInfo& info) {
  REQ_ARGS(7);
  REQ_INT_ARG(0, cx);
  REQ_INT_ARG(1, cy);
  REQ_INT_ARG(2, width);
  REQ_INT_ARG(3, height);
  REQ_INT_ARG(4, begin);
  REQ_INT_ARG(5, end);
  REQ_INT_ARG(6, color);

  gdImageArc(this->_image, cx, cy, width, height, begin, end, color);

  return info.This();
}

Napi::Value Gd::Image::FilledArc(const Napi::CallbackInfo& info) {
  REQ_ARGS(8);
  REQ_INT_ARG(0, cx);
  REQ_INT_ARG(1, cy);
  REQ_INT_ARG(2, width);
  REQ_INT_ARG(3, height);
  REQ_INT_ARG(4, begin);
  REQ_INT_ARG(5, end);
  REQ_INT_ARG(6, color);
  REQ_INT_ARG(7, style);

  gdImageFilledArc(this->_image, cx, cy, width, height, begin, end, color, style);

  return info.This();
}

#if SUPPORTS_GD_2_1_0
Napi::Value Gd::Image::Ellipse(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(0, cx);
  REQ_INT_ARG(1, cy);
  REQ_INT_ARG(2, width);
  REQ_INT_ARG(3, height);
  REQ_INT_ARG(4, color);

  gdImageEllipse(this->_image, cx, cy, width, height, color);

  return info.This();
}
#endif

Napi::Value Gd::Image::FilledEllipse(const Napi::CallbackInfo& info) {
  REQ_ARGS(5);
  REQ_INT_ARG(0, cx);
  REQ_INT_ARG(1, cy);
  REQ_INT_ARG(2, width);
  REQ_INT_ARG(3, height);
  REQ_INT_ARG(4, color);

  gdImageFilledEllipse(this->_image, cx, cy, width, height, color);

  return info.This();
}

Napi::Value Gd::Image::FillToBorder(const Napi::CallbackInfo& info) {
  REQ_ARGS(4);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);
  REQ_INT_ARG(2, border);
  REQ_INT_ARG(3, color);

  gdImageFillToBorder(this->_image, x, y, border, color);

  return info.This();
}

Napi::Value Gd::Image::Fill(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);
  REQ_INT_ARG(2, color);

  gdImageFill(this->_image, x, y, color);

  return info.This();
}

Napi::Value Gd::Image::SetAntiAliased(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);

  gdImageSetAntiAliased(this->_image, color);

  return info.This();
}

Napi::Value Gd::Image::SetAntiAliasedDontBlend(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, color);
  REQ_INT_ARG(1, dont_blend); // what does this mean?

  gdImageSetAntiAliasedDontBlend(this->_image, color, dont_blend);

  return info.This();
}

Napi::Value Gd::Image::SetBrush(const Napi::CallbackInfo& info) {
  REQ_IMG_ARG(0, brush)
  gdImageSetBrush(this->_image, brush);

  return info.This();
}

Napi::Value Gd::Image::SetTile(const Napi::CallbackInfo& info) {
  REQ_IMG_ARG(0, tile)
  gdImageSetTile(this->_image, tile);

  return info.This();
}

Napi::Value Gd::Image::SetStyle(const Napi::CallbackInfo& info) {
  if (info.Length() < 1 || !info[0].IsArray())
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  int *sty =  new int[len];

  for(unsigned int i = 0; i < len; i++) {
    Napi::Value v = array.Get(i);
    if (!v.IsNumber()) continue;

    sty[i] = v.As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImageSetStyle(this->_image, sty, _len);

  delete[] sty;

  return info.This();
}

Napi::Value Gd::Image::SetThickness(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, thickness)

  gdImageSetThickness(this->_image, thickness);

  return info.This();
}

Napi::Value Gd::Image::AlphaBlending(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, blending)
  gdImageAlphaBlending(this->_image, blending);

  return info.This();
}

Napi::Value Gd::Image::SaveAlpha(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, saveFlag)
  gdImageSaveAlpha(this->_image, saveFlag);

  return info.This();
}

Napi::Value Gd::Image::SetClip(const Napi::CallbackInfo& info) {
  REQ_ARGS(4);
  REQ_INT_ARG(0, x1);
  REQ_INT_ARG(1, y1);
  REQ_INT_ARG(2, x2);
  REQ_INT_ARG(3, y2);

  gdImageSetClip(this->_image, x1, y1, x2, y2);

  return info.This();
}

Napi::Value Gd::Image::GetClip(const Napi::CallbackInfo& info) {
  int x1, y1, x2, y2;
  gdImageGetClip(this->_image, &x1, &y1, &x2, &y2);

  Napi::Object result = Napi::Object::New(info.Env());
  (result).Set(Napi::String::New(info.Env(), "x1"), Napi::Number::New(info.Env(), x1));
  (result).Set(Napi::String::New(info.Env(), "y1"), Napi::Number::New(info.Env(), y1));
  (result).Set(Napi::String::New(info.Env(), "x2"), Napi::Number::New(info.Env(), x2));
  (result).Set(Napi::String::New(info.Env(), "y2"), Napi::Number::New(info.Env(), y2));

  return result;
}

#if SUPPORTS_GD_2_1_0
Napi::Value Gd::Image::SetResolution(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, res_x);
  REQ_INT_ARG(1, res_y);

  gdImageSetResolution(this->_image, res_x, res_y);

  return info.This();
}
#endif

/**
 * Query Functions
 */
Napi::Value Gd::Image::GetPixel(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);

  if (x < 0 || y < 0) {
    Napi::RangeError::New(info.Env(), "Value for x and y must be greater than 0").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  int imageX = gdImageSX(this->_image);
  int imageY = gdImageSY(this->_image);

  Napi::Number result;
  if (x > imageX || y > imageY) {
    result = Napi::Number::New(info.Env(), 0);
  } else {
    result = Napi::Number::New(info.Env(), gdImageGetPixel(this->_image, x, y));
  }

  return result;
}

Napi::Value Gd::Image::GetTrueColorPixel(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);

  if (x < 0 || y < 0) {
    Napi::RangeError::New(info.Env(), "Value for x and y must be greater than 0").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  int imageX = gdImageSX(this->_image);
  int imageY = gdImageSY(this->_image);


  Napi::Number result;
  if (x > imageX || y > imageY) {
    result = Napi::Number::New(info.Env(), 0);
  } else {
    result = Napi::Number::New(info.Env(), gdImageGetPixel(this->_image, x, y));
  }

  result = Napi::Number::New(info.Env(), gdImageGetTrueColorPixel(this->_image, x, y));
  return result;
}

// This is implementation of the PHP-GD specific method imagecolorat
Napi::Value Gd::Image::ImageColorAt(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);

  Napi::Number result;
  gdImagePtr im = this->_image;
  if (gdImageTrueColor(im)) {
    if(im->tpixels && gdImageBoundsSafe(im, x, y)){
      result = Napi::Number::New(info.Env(), gdImageTrueColorPixel(im, x, y));
    } else {
      Napi::Error::New(info.Env(), "[imageColorAt] Invalid pixel").ThrowAsJavaScriptException();
      return info.Env().Null();
    }
  } else {
    if (im->pixels && gdImageBoundsSafe(im, x, y)) {
      result = Napi::Number::New(info.Env(), im->pixels[y][x]);
    } else {
      Napi::Error::New(info.Env(), "[imageColorAt] Invalid pixel").ThrowAsJavaScriptException();
      return info.Env().Null();
    }
  }
  return result;
}

Napi::Value Gd::Image::GetBoundsSafe(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageBoundsSafe(this->_image, x, y));
  return result;
}

Napi::Value Gd::Image::WidthGetter(const Napi::CallbackInfo& info) {
  Napi::Number result = Napi::Number::New(info.Env(), gdImageSX(this->_image));
  return result;
}

Napi::Value Gd::Image::HeightGetter(const Napi::CallbackInfo& info) {
  Napi::Number result = Napi::Number::New(info.Env(), gdImageSY(this->_image));
  return result;
}

Napi::Value Gd::Image::TrueColorGetter(const Napi::CallbackInfo& info) {
  Napi::Number result = Napi::Number::New(info.Env(), gdImageTrueColor(this->_image));
  return result;
}

/**
 * Font and Text Handling Functions
 */
Napi::Value Gd::Image::StringFTBBox(const Napi::CallbackInfo& info) {
  REQ_ARGS(7);
  REQ_INT_ARG(0, color);
  REQ_STR_ARG(1, font);
  REQ_DOUBLE_ARG(2, size);
  REQ_DOUBLE_ARG(3, angle);
  REQ_INT_ARG(4, x);
  REQ_INT_ARG(5, y);
  REQ_STR_ARG(6, str);

  int brect[8];
  char *err;
  char *fontlist = &font[0];
  char *text = &str[0];

  err = gdImageStringFT(NULL, &brect[0], color, fontlist, size, angle, x, y, text);
  if (err) {
    Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::Array result = Napi::Array::New(info.Env());

  for (unsigned int i = 0; i < 8; i++) {
    (result).Set(Napi::Number::New(info.Env(), i), Napi::Number::New(info.Env(), brect[i]));
  }

  return result;
}

Napi::Value Gd::Image::StringFT(const Napi::CallbackInfo& info) {
  REQ_ARGS(7);
  REQ_INT_ARG(0, color);
  REQ_STR_ARG(1, font);
  REQ_DOUBLE_ARG(2, size);
  REQ_DOUBLE_ARG(3, angle);
  REQ_INT_ARG(4, x);
  REQ_INT_ARG(5, y);
  REQ_STR_ARG(6, str);
  OPT_BOOL_ARG(7, return_rectangle, false);

  int brect[8];
  char *error;
  char *fontlist = &font[0];
  char *text = &str[0];

  if (return_rectangle) {
    error = gdImageStringFT(NULL, &brect[0], color, fontlist, size, angle, x, y, text);
    if (error) {
      Napi::Error::New(info.Env(), error).ThrowAsJavaScriptException();
      return info.Env().Null();
    }

    Napi::Array result = Napi::Array::New(info.Env());
    for(unsigned int i = 0; i < 8; i++) {
      (result).Set(Napi::Number::New(info.Env(), i), Napi::Number::New(info.Env(), brect[i]));
    }

    return result;
  }

  error = gdImageStringFT(this->_image, &brect[0], color, fontlist, size, angle, x, y, text);
  if (error) {
    Napi::Error::New(info.Env(), error).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  return info.This();
}

Napi::Value Gd::Image::StringFTEx(const Napi::CallbackInfo& info) {
  REQ_ARGS(8);
  REQ_INT_ARG(0, color);
  REQ_STR_ARG(1, font);
  REQ_DOUBLE_ARG(2, size);
  REQ_DOUBLE_ARG(3, angle);
  REQ_INT_ARG(4, x);
  REQ_INT_ARG(5, y);
  REQ_STR_ARG(6, str);

  if (!info[7].IsObject()) {
    Napi::TypeError::New(info.Env(), "Argument 8 must be an object").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  OPT_BOOL_ARG(8, return_rectangle, false);
  int brect[8];

  Napi::Object stringExtraParameter = info[7].As<Napi::Object>();

  Napi::String linespacing = Napi::String::New(info.Env(), "linespacing");
  Napi::String hdpi = Napi::String::New(info.Env(), "hdpi");
  Napi::String vdpi = Napi::String::New(info.Env(), "vdpi");
  Napi::String charmap = Napi::String::New(info.Env(), "charmap");
  Napi::String disable_kerning = Napi::String::New(info.Env(), "disable_kerning");
  Napi::String xshow = Napi::String::New(info.Env(), "xshow");
  Napi::String return_fontpathname = Napi::String::New(info.Env(), "return_fontpathname");
  Napi::String use_fontconfig = Napi::String::New(info.Env(), "use_fontconfig");
  Napi::String fontpath = Napi::String::New(info.Env(), "fontpath");

  gdFTStringExtra stringExtra;
  stringExtra.flags = 0;

  // linespacing
  if (stringExtraParameter.Has(linespacing)) {
    stringExtra.flags |= gdFTEX_LINESPACE;
    stringExtra.linespacing = stringExtraParameter.Get(linespacing).As<Napi::Number>().DoubleValue();
  }

  // charmap
  if (stringExtraParameter.Has(charmap)) {
    stringExtra.charmap = 0;

    const char *chmap = stringExtraParameter.Get(charmap).As<Napi::String>().Utf8Value().c_str();

    // gdFTEX_Unicode, gdFTEX_Shift_JIS, gdFTEX_Big5, gdFTEX_Adobe_Custom
    // unicode, shift_jis, big5, adobe_custom
    if (!std::strcmp(chmap, "unicode")) {
      stringExtra.charmap = gdFTEX_Unicode;
    } else if (!std::strcmp(chmap, "shift_jis")) {
      stringExtra.charmap = gdFTEX_Shift_JIS;
    } else if (!std::strcmp(chmap, "big5")) {
      stringExtra.charmap = gdFTEX_Big5;
    } else if (!std::strcmp(chmap, "adobe_custom")) {
      stringExtra.charmap = gdFTEX_Adobe_Custom;
    } else {
      Napi::Error::New(info.Env(), "Unknown value for charmap. Should be one of: unicode, shift_jis, big5, adobe_custom").ThrowAsJavaScriptException();

    }
    stringExtra.flags |= gdFTEX_CHARMAP;
  }

  // resolution
  // horizontal dpi
  // The JavaScript value of NaN will be cast to a 0 by v8?
  if (stringExtraParameter.Has(hdpi)) {
    stringExtra.flags |= gdFTEX_RESOLUTION;
    stringExtra.hdpi = stringExtraParameter.Get(hdpi).As<Napi::Number>().Int32Value();
    if (!stringExtraParameter.Has(vdpi)) {
      stringExtra.vdpi = stringExtra.hdpi;
    }
  }

  // vertical dpi
  if (stringExtraParameter.Has(vdpi)) {
    stringExtra.flags |= gdFTEX_RESOLUTION;
    stringExtra.vdpi = stringExtraParameter.Get(vdpi).As<Napi::Number>().Int32Value();
    if (!stringExtraParameter.Has(hdpi)) {
      stringExtra.hdpi = stringExtra.vdpi;
    }
  }

  // disable kerning
  if (stringExtraParameter.Has(disable_kerning)) {
    bool is_disable_kerning = stringExtraParameter.Get(disable_kerning).As<Napi::Boolean>().Value();
    if (is_disable_kerning) {
      stringExtra.flags |= gdFTEX_DISABLE_KERNING;
    }
  }

  // xshow
  bool is_xshow = false;
  if (stringExtraParameter.Has(xshow)) {
    is_xshow = stringExtraParameter.Get(xshow).As<Napi::Boolean>().Value();
    if (is_xshow) {
      stringExtra.flags |= gdFTEX_XSHOW;
    }
  }

  // fontpathname
  if (stringExtraParameter.Has(fontpath)) {
    std::string localFontpathname = stringExtraParameter.Get(fontpath).As<Napi::String>().Utf8Value();
    stringExtra.flags |= gdFTEX_FONTPATHNAME;
    stringExtra.fontpath = &localFontpathname[0];
  }

  // use_fontconfig
  if (stringExtraParameter.Has(use_fontconfig)) {
    bool is_use_fontconfig = stringExtraParameter.Get(use_fontconfig).As<Napi::Boolean>().Value();
    if (is_use_fontconfig) {
      stringExtra.flags |= gdFTEX_FONTCONFIG;
    }
  }

  // return_fontpathname
  bool is_use_fontpathname = false;
  if (stringExtraParameter.Has(return_fontpathname)) {
    is_use_fontpathname = stringExtraParameter.Get(return_fontpathname).As<Napi::Boolean>().Value();
    if (is_use_fontpathname) {
      stringExtra.flags |= gdFTEX_RETURNFONTPATHNAME;
      stringExtra.fontpath = NULL;
    }
  }

  const char *error;
  char *fontlist = &font[0];
  char *text = &str[0];

  if (return_rectangle) {
    error = gdImageStringFTEx(NULL, &brect[0], color, fontlist, size, angle, x, y, text, &stringExtra);
    if (error) {
      std::string prefix ("GD Error: ");
      std::string errormsg (error);
      const char *errorMessage = (prefix + error).c_str();
      Napi::Error::New(info.Env(), errorMessage).ThrowAsJavaScriptException();
      return info.Env().Null();
    }

    Napi::Array result = Napi::Array::New(info.Env());
    for(unsigned int i = 0; i < 8; i++) {
      (result).Set(Napi::Number::New(info.Env(), i), Napi::Number::New(info.Env(), brect[i]));
    }

    return result;
  }

  error = gdImageStringFTEx(this->_image, &brect[0], color, fontlist, size, angle, x, y, text, &stringExtra);

  if (error) {
    std::string prefix("GD Error: ");
    std::string errormsg(error);
    const char *errorMessage = (prefix + error).c_str();
    Napi::Error::New(info.Env(), errorMessage).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  if (is_use_fontpathname && stringExtra.fontpath != NULL) {
    (stringExtraParameter).Set(fontpath, Napi::String::New(info.Env(), stringExtra.fontpath));
  }

  if (is_xshow && stringExtra.xshow != NULL) {
    (stringExtraParameter).Set(xshow, Napi::String::New(info.Env(), stringExtra.xshow));
  }

  return stringExtraParameter;
}

Napi::Value Gd::Image::StringFTCircle(const Napi::CallbackInfo& info) {
  REQ_ARGS(9);
  REQ_INT_ARG(0, cx);
  REQ_INT_ARG(1, cy);
  REQ_DOUBLE_ARG(2, radius);
  REQ_DOUBLE_ARG(3, textRadius);
  REQ_DOUBLE_ARG(4, fillPortion);
  REQ_STR_ARG(5, font);
  REQ_DOUBLE_ARG(6, size);
  REQ_STR_ARG(7, top);
  REQ_STR_ARG(8, bottom);
  REQ_INT_ARG(9, color);

  char *error;
  char *fontlist = &font[0];
  char *topp = &top[0];
  char *bottomm = &bottom[0];

  error = gdImageStringFTCircle(this->_image, cx, cy, radius, textRadius, fillPortion, fontlist, size, topp, bottomm, color);
  if (error) {
    Napi::Error::New(info.Env(), error).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  return info.This();
}

/**
 * Color Handling Functions
 */
Napi::Value Gd::Image::ColorAllocate(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorAllocate(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorAllocateAlpha(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorAllocateAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorClosest(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorClosest(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorClosestAlpha(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorClosestAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorClosestHWB(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorClosestHWB(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorExact(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorExact(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorResolve(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorResolve(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorResolveAlpha(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorResolveAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorsTotalGetter(const Napi::CallbackInfo& info) {
  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorsTotal(this->_image));
  return result;
}

Napi::Value Gd::Image::Red(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageRed(this->_image, color));
  return result;
}

Napi::Value Gd::Image::Blue(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageBlue(this->_image, color));
  return result;
}

Napi::Value Gd::Image::Green(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageGreen(this->_image, color));
  return result;
}

Napi::Value Gd::Image::Alpha(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageAlpha(this->_image, color));
  return result;
}

Napi::Value Gd::Image::InterlaceGetter(const Napi::CallbackInfo& info) {
  bool interlaced = (gdImageGetInterlaced(this->_image) != 0);
  Napi::Boolean result;
  if (interlaced) {
    result = Napi::Boolean::New(info.Env(), true);
  } else {
    result = Napi::Boolean::New(info.Env(), false);
  }

  return result;
}

void Gd::Image::InterlaceSetter(const Napi::CallbackInfo& info, const Napi::Value& value) {
  if (value.IsBoolean()) {
    bool interlace = value.As<Napi::Boolean>().Value();

    gdImageInterlace(this->_image, interlace ? 1 : 0);
  }
}

Napi::Value Gd::Image::GetTransparent(const Napi::CallbackInfo& info) {
  Napi::Number result = Napi::Number::New(info.Env(), gdImageGetTransparent(this->_image));
  return result;
}

Napi::Value Gd::Image::ColorDeallocate(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);
  gdImageColorDeallocate(this->_image, color);

  return info.This();
}

Napi::Value Gd::Image::ColorTransparent(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);
  gdImageColorTransparent(this->_image, color);

  return info.This();
}

#if SUPPORTS_GD_2_1_0
Napi::Value Gd::Image::ColorReplace(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);
  REQ_INT_ARG(0, fromColor);
  REQ_INT_ARG(1, toColor);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorReplace(this->_image, fromColor, toColor));

  return result;
}

Napi::Value Gd::Image::ColorReplaceThreshold(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_INT_ARG(0, fromColor);
  REQ_INT_ARG(1, toColor);
  REQ_DOUBLE_ARG(2, threshold);

  Napi::Number result =
    Napi::Number::New(info.Env(), gdImageColorReplaceThreshold(this->_image, fromColor, toColor, threshold));

  return result;
}

Napi::Value Gd::Image::ColorReplaceArray(const Napi::CallbackInfo& info) {
  REQ_ARGS(2);

  Napi::Array fromArray = info[0].As<Napi::Array>();
  unsigned int flen = fromArray.Length(), _flen = 0;
  int *fromColors =  new int[flen];

  for(unsigned int i = 0; i < flen; i++) {
    Napi::Value v = fromArray.Get(i);
    fromColors[i] = v.As<Napi::Number>().Int32Value();
    _flen++;
  }

  Napi::Array toArray = info[1].As<Napi::Array>();
  unsigned int tlen = toArray.Length(), _tlen = 0;
  int *toColors =  new int[tlen];

  for(unsigned int j = 0; j < tlen; j++) {
    Napi::Value v = toArray.Get(j);
    toColors[j] = v.As<Napi::Number>().Int32Value();
    _tlen++;
  }

  if (_flen != _tlen) {
    Napi::Error::New(info.Env(), "Color arrays should have same length.").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::Number result =
    Napi::Number::New(info.Env(), gdImageColorReplaceArray(this->_image, _flen, fromColors, toColors));

  return result;
}

Napi::Value Gd::Image::GrayScale(const Napi::CallbackInfo& info) {

  gdImageGrayScale(this->_image);

  return info.This();
}

Napi::Value Gd::Image::GaussianBlur(const Napi::CallbackInfo& info) {

  gdImageGaussianBlur(this->_image);

  return info.This();
}

Napi::Value Gd::Image::Negate(const Napi::CallbackInfo& info) {

  gdImageNegate(this->_image);

  return info.This();
}

Napi::Value Gd::Image::Brightness(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, brightness);

  gdImageBrightness(this->_image, brightness);

  return info.This();
}

Napi::Value Gd::Image::Contrast(const Napi::CallbackInfo& info) {
  REQ_DOUBLE_ARG(0, contrast);

  gdImageContrast(this->_image, contrast);

  return info.This();
}

Napi::Value Gd::Image::SelectiveBlur(const Napi::CallbackInfo& info) {
  gdImageSelectiveBlur(this->_image);
  return info.This();
}

Napi::Value Gd::Image::FlipHorizontal(const Napi::CallbackInfo& info) {
  gdImageFlipHorizontal(this->_image);
  return info.This();
}

Napi::Value Gd::Image::FlipVertical(const Napi::CallbackInfo& info) {
  gdImageFlipVertical(this->_image);
  return info.This();
}

Napi::Value Gd::Image::FlipBoth(const Napi::CallbackInfo& info) {
  gdImageFlipBoth(this->_image);
  return info.This();
}

Napi::Value Gd::Image::Crop(const Napi::CallbackInfo& info) {
  REQ_ARGS(4);
  REQ_INT_ARG(0, x);
  REQ_INT_ARG(1, y);
  REQ_INT_ARG(2, width);
  REQ_INT_ARG(3, height);

  gdRect *rect = new gdRect;

  rect->x = x;
  rect->y = y;
  rect->width = (width == 0) ? 100 : width;
  rect->height = (height == 0) ? 100 : height;

  gdImagePtr newImage = gdImageCrop(this->_image, rect);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::CropAuto(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, mode);

  if (mode > 4) {
    Napi::RangeError::New(info.Env(), "Crop mode should be between 0 and 5").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  gdImagePtr newImage = gdImageCropAuto(this->_image, mode);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::CropThreshold(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, color);
  REQ_DOUBLE_ARG(1, threshold);

  gdImagePtr newImage = gdImageCropThreshold(this->_image, color, threshold);
  RETURN_IMAGE(newImage);
}
#endif

#if SUPPORTS_GD_2_1_1
Napi::Value Gd::Image::Emboss(const Napi::CallbackInfo& info) {
  gdImageEmboss(this->_image);
  return info.This();
}
#endif

/**
 * Copying and Resizing Functions
 */
Napi::Value Gd::Image::Copy(const Napi::CallbackInfo& info) {
  REQ_ARGS(7);
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX);
  REQ_INT_ARG(2, dstY);
  REQ_INT_ARG(3, srcX);
  REQ_INT_ARG(4, srcY);
  REQ_INT_ARG(5, width);
  REQ_INT_ARG(6, height);

  gdImageCopy(dest, this->_image, dstX, dstY, srcX, srcY, width, height);

  return info.This();
}

Napi::Value Gd::Image::CopyResized(const Napi::CallbackInfo& info) {
  REQ_ARGS(9);
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX);
  REQ_INT_ARG(2, dstY);
  REQ_INT_ARG(3, srcX);
  REQ_INT_ARG(4, srcY);
  REQ_INT_ARG(5, destW);
  REQ_INT_ARG(6, destH);
  REQ_INT_ARG(7, srcW);
  REQ_INT_ARG(8, srcH);

  gdImageCopyResized(dest, this->_image, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

  return info.This();
}

Napi::Value Gd::Image::CopyResampled(const Napi::CallbackInfo& info) {
  REQ_ARGS(9);
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX);
  REQ_INT_ARG(2, dstY);
  REQ_INT_ARG(3, srcX);
  REQ_INT_ARG(4, srcY);
  REQ_INT_ARG(5, destW);
  REQ_INT_ARG(6, destH);
  REQ_INT_ARG(7, srcW);
  REQ_INT_ARG(8, srcH);

  gdImageCopyResampled(dest, this->_image, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

  return info.This();
}

Napi::Value Gd::Image::CopyRotated(const Napi::CallbackInfo& info) {
  REQ_ARGS(8);
  REQ_IMG_ARG(0, dest);
  REQ_DOUBLE_ARG(1, dstX);
  REQ_DOUBLE_ARG(2, dstY);
  REQ_INT_ARG(3, srcX);
  REQ_INT_ARG(4, srcY);
  REQ_INT_ARG(5, srcW);
  REQ_INT_ARG(6, srcH);
  REQ_INT_ARG(7, angle);

  gdImageCopyRotated(dest, this->_image, dstX, dstY, srcX, srcY, srcW, srcH, angle);

  return info.This();
}

Napi::Value Gd::Image::CopyMerge(const Napi::CallbackInfo& info) {
  REQ_ARGS(8);
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX);
  REQ_INT_ARG(2, dstY);
  REQ_INT_ARG(3, srcX);
  REQ_INT_ARG(4, srcY);
  REQ_INT_ARG(5, width);
  REQ_INT_ARG(6, height);
  REQ_INT_ARG(7, pct);

  gdImageCopyMerge(dest, this->_image, dstX, dstY, srcX, srcY, width, height, pct);

  return info.This();
}

Napi::Value Gd::Image::CopyMergeGray(const Napi::CallbackInfo& info) {
  REQ_ARGS(8);
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX);
  REQ_INT_ARG(2, dstY);
  REQ_INT_ARG(3, srcX);
  REQ_INT_ARG(4, srcY);
  REQ_INT_ARG(5, width);
  REQ_INT_ARG(6, height);
  REQ_INT_ARG(7, pct);

  gdImageCopyMergeGray(dest, this->_image, dstX, dstY, srcX, srcY, width, height, pct);

  return info.This();
}

Napi::Value Gd::Image::PaletteCopy(const Napi::CallbackInfo& info) {
  REQ_IMG_ARG(0, dest);

  gdImagePaletteCopy(dest, this->_image);

  return info.This();
}

Napi::Value Gd::Image::SquareToCircle(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, radius);

  gdImagePtr newImage = gdImageSquareToCircle(this->_image, radius);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::Sharpen(const Napi::CallbackInfo& info) {
  REQ_INT_ARG(0, pct);

  gdImageSharpen(this->_image, pct);

  return info.This();
}

Napi::Value Gd::Image::CreatePaletteFromTrueColor(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, ditherFlag, 0);
  OPT_INT_ARG(1, colorsWanted, 256);

  gdImagePtr newImage = gdImageCreatePaletteFromTrueColor(this->_image, ditherFlag, colorsWanted);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::TrueColorToPalette(const Napi::CallbackInfo& info) {
  OPT_INT_ARG(0, ditherFlag, 0);
  OPT_INT_ARG(1, colorsWanted, 256);

#if SUPPORTS_GD_2_1_0
  Napi::Number result = Napi::Number::New(info.Env(), gdImageTrueColorToPalette(this->_image, ditherFlag, colorsWanted));
  return result;
#endif
#if SUPPORTS_UNTIL_GD_2_0_36
  gdImageTrueColorToPalette(this->_image, ditherFlag, colorsWanted);
  return info.Env().Undefined();
#endif
}

#if SUPPORTS_GD_2_1_0
Napi::Value Gd::Image::PaletteToTrueColor(const Napi::CallbackInfo& info) {
  Napi::Number result = Napi::Number::New(info.Env(), gdImagePaletteToTrueColor(this->_image));

  return result;
}

Napi::Value Gd::Image::ColorMatch(const Napi::CallbackInfo& info) {
  REQ_IMG_ARG(0, palette);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorMatch(this->_image, palette));

  return result;
}
#endif

Napi::Value Gd::Image::GifAnimBegin(const Napi::CallbackInfo& info) {
  REQ_ARGS(3);
  REQ_STR_ARG(0, path);
  REQ_INT_ARG(1, GlobalCM);
  REQ_INT_ARG(2, Loops);

  FILE *out = fopen(path.c_str(), "wb");

  gdImageGifAnimBegin(this->_image, out, GlobalCM, Loops);
  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::GifAnimAdd(const Napi::CallbackInfo& info) {
  REQ_ARGS(6);
  REQ_STR_ARG(0, path);
  REQ_INT_ARG(1, LocalCM);
  REQ_INT_ARG(2, LeftOfs);
  REQ_INT_ARG(3, TopOfs);
  REQ_INT_ARG(4, Delay);
  REQ_INT_ARG(5, Disposal);

  FILE *out = fopen(path.c_str(), "ab");

  if (info.Length() <= 6) {
    Napi::TypeError::New(info.Env(), "Argument 6 must be an object").ThrowAsJavaScriptException();
    return info.Env().Null();
  } else if (info[6].IsObject()) {
    Gd::Image* _obj_ = Napi::ObjectWrap<Gd::Image>::Unwrap(info[6].As<Napi::Object>());
    gdImagePtr prevFrame = _obj_->getGdImagePtr();
    gdImageGifAnimAdd(this->_image, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, prevFrame);
  } else {
    gdImageGifAnimAdd(this->_image, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, NULL);
  }

  fclose(out);

  return info.This();
}

Napi::Value Gd::Image::GifAnimEnd(const Napi::CallbackInfo& info) {
  REQ_STR_ARG(0, path);

  FILE *out = fopen(path.c_str(), "ab");
  gdImageGifAnimEnd(out);
  fclose(out);

  return info.This();
}

/**
 * Miscellaneous Functions
 */
Napi::Value Gd::Image::Compare(const Napi::CallbackInfo& info) {
  REQ_ARGS(1);
  if (!info[0].IsObject()) {
    Napi::TypeError::New(info.Env(), "Argument 0 must be an image").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Gd::Image* _obj_ = Napi::ObjectWrap<Gd::Image>::Unwrap(info[0].As<Napi::Object>());
  gdImagePtr im2 = _obj_->getGdImagePtr();

  Napi::Number result = Napi::Number::New(info.Env(), gdImageCompare(this->_image, im2));

  return result;
}

Napi::FunctionReference Gd::Image::constructor;
