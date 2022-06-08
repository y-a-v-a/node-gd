/**
 * Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>
 * Copyright (c) 2014-2021, Vincent Bruijn <vebruijn@gmail.com>
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
#include "node_gd_workers.cc"
#include <gd_errors.h>

Napi::Object Gd::Init(Napi::Env env, Napi::Object exports)
{

  /**
   * Section E - Meta information
   */
  Napi::PropertyDescriptor ColorAntialiased = Napi::PropertyDescriptor::Value("COLOR_ANTIALIASED",
                                                                              Napi::Number::New(env, COLOR_ANTIALIASED));
  Napi::PropertyDescriptor ColorBrushed = Napi::PropertyDescriptor::Value("COLOR_BRUSHED",
                                                                          Napi::Number::New(env, COLOR_BRUSHED));
  Napi::PropertyDescriptor ColorStyled = Napi::PropertyDescriptor::Value("COLOR_STYLED",
                                                                         Napi::Number::New(env, COLOR_STYLED));
  Napi::PropertyDescriptor ColorStyledBrushed = Napi::PropertyDescriptor::Value("COLOR_STYLEDBRUSHED",
                                                                                Napi::Number::New(env, COLOR_STYLEDBRUSHED));
  Napi::PropertyDescriptor ColorTitled = Napi::PropertyDescriptor::Value("COLOR_TITLED",
                                                                         Napi::Number::New(env, COLOR_TITLED));
  Napi::PropertyDescriptor ColorTransparent = Napi::PropertyDescriptor::Value("COLOR_TRANSPARENT",
                                                                              Napi::Number::New(env, COLOR_TRANSPARENT));

  exports.DefineProperties({ColorAntialiased,
                            ColorBrushed,
                            ColorStyled,
                            ColorStyledBrushed,
                            ColorTitled,
                            ColorTransparent});

#ifdef HAVE_LIBHEIF
  Napi::PropertyDescriptor GdHeif = Napi::PropertyDescriptor::Value("GD_HEIF",
                                                                    Napi::Number::New(env, GD_HEIF));
  exports.DefineProperty(GdHeif);
#endif

#ifdef HAVE_LIBAVIF
  Napi::PropertyDescriptor GdAvif = Napi::PropertyDescriptor::Value("GD_AVIF",
                                                                    Napi::Number::New(env, GD_AVIF));
  exports.DefineProperty(GdAvif);
#endif

#ifdef HAVE_LIBTIFF
  Napi::PropertyDescriptor GdTiff = Napi::PropertyDescriptor::Value("GD_TIFF",
                                                                    Napi::Number::New(env, GD_TIFF));
  exports.DefineProperty(GdTiff);
#endif

#ifdef HAVE_LIBXPM
  Napi::PropertyDescriptor GdXpm = Napi::PropertyDescriptor::Value("GD_XPM",
                                                                   Napi::Number::New(env, GD_XPM));
  exports.DefineProperty(GdXpm);
#endif

#ifdef HAVE_LIBJPEG
  Napi::PropertyDescriptor GdJpeg = Napi::PropertyDescriptor::Value("GD_JPEG",
                                                                    Napi::Number::New(env, GD_JPEG));
  exports.DefineProperty(GdJpeg);
#endif

#ifdef HAVE_LIBFONTCONFIG
  Napi::PropertyDescriptor GdFontConfig = Napi::PropertyDescriptor::Value("GD_FONTCONFIG",
                                                                          Napi::Number::New(env, GD_FONTCONFIG));
  exports.DefineProperty(GdFontConfig);
#endif

#ifdef HAVE_LIBFREETYPE
  Napi::PropertyDescriptor GdFreetype = Napi::PropertyDescriptor::Value("GD_FREETYPE",
                                                                        Napi::Number::New(env, GD_FREETYPE));
  exports.DefineProperty(GdFreetype);
#endif

#ifdef HAVE_LIBPNG
  Napi::PropertyDescriptor GdPng = Napi::PropertyDescriptor::Value("GD_PNG",
                                                                   Napi::Number::New(env, GD_PNG));
  exports.DefineProperty(GdPng);
#endif

#ifdef HAVE_LIBWEBP
  Napi::PropertyDescriptor GdWebp = Napi::PropertyDescriptor::Value("GD_WEBP",
                                                                    Napi::Number::New(env, GD_WEBP));
  exports.DefineProperty(GdWebp);
#endif

#ifdef HAVE_LIBVPX
  Napi::PropertyDescriptor GdVpx = Napi::PropertyDescriptor::Value("GD_VPX",
                                                                   Napi::Number::New(env, GD_VPX));
  exports.DefineProperty(GdVpx);
#endif

  Napi::PropertyDescriptor GdGif = Napi::PropertyDescriptor::Value("GD_GIF",
                                                                   Napi::Number::New(env, GD_GIF));
  Napi::PropertyDescriptor GdGifAnim = Napi::PropertyDescriptor::Value("GD_GIFANIM",
                                                                       Napi::Number::New(env, GD_GIFANIM));
  Napi::PropertyDescriptor GdOpenPolygon = Napi::PropertyDescriptor::Value("GD_OPENPOLYGON",
                                                                           Napi::Number::New(env, GD_OPENPOLYGON));

  exports.DefineProperties({GdGif,
                            GdGifAnim,
                            GdOpenPolygon});

  // Image creation, loading and saving
  exports.Set(Napi::String::New(env, "create"), Napi::Function::New(env, ImageCreate));
  exports.Set(Napi::String::New(env, "createTrueColor"), Napi::Function::New(env, ImageCreateTrueColor));
  exports.Set(Napi::String::New(env, "createSync"), Napi::Function::New(env, ImageCreateSync));
  exports.Set(Napi::String::New(env, "createTrueColorSync"), Napi::Function::New(env, ImageCreateTrueColorSync));
  exports.Set(Napi::String::New(env, "createFromJpeg"), Napi::Function::New(env, CreateFromJpeg));
  exports.Set(Napi::String::New(env, "createFromJpegPtr"), Napi::Function::New(env, CreateFromJpegPtr));
  exports.Set(Napi::String::New(env, "createFromPng"), Napi::Function::New(env, CreateFromPng));
  exports.Set(Napi::String::New(env, "createFromPngPtr"), Napi::Function::New(env, CreateFromPngPtr));
  exports.Set(Napi::String::New(env, "createFromGif"), Napi::Function::New(env, CreateFromGif));
  exports.Set(Napi::String::New(env, "createFromGifPtr"), Napi::Function::New(env, CreateFromGifPtr));
  exports.Set(Napi::String::New(env, "createFromWBMP"), Napi::Function::New(env, CreateFromWBMP));
  exports.Set(Napi::String::New(env, "createFromWBMPPtr"), Napi::Function::New(env, CreateFromWBMPPtr));
#if HAS_LIBWEBP
  exports.Set(Napi::String::New(env, "createFromWebp"), Napi::Function::New(env, CreateFromWebp));
  exports.Set(Napi::String::New(env, "createFromWebpPtr"), Napi::Function::New(env, CreateFromWebpPtr));
#endif

  exports.Set(Napi::String::New(env, "createFromBmp"), Napi::Function::New(env, CreateFromBmp));
  exports.Set(Napi::String::New(env, "createFromBmpPtr"), Napi::Function::New(env, CreateFromBmpPtr));

#if HAS_LIBHEIF
  exports.Set(Napi::String::New(env, "createFromHeif"), Napi::Function::New(env, CreateFromHeif));
  exports.Set(Napi::String::New(env, "createFromHeifPtr"), Napi::Function::New(env, CreateFromHeifPtr));
#endif
#if HAS_LIBAVIF
  exports.Set(Napi::String::New(env, "createFromAvif"), Napi::Function::New(env, CreateFromAvif));
  exports.Set(Napi::String::New(env, "createFromAvifPtr"), Napi::Function::New(env, CreateFromAvifPtr));
#endif
#if HAS_LIBTIFF
  exports.Set(Napi::String::New(env, "createFromTiff"), Napi::Function::New(env, CreateFromTiff));
  exports.Set(Napi::String::New(env, "createFromTiffPtr"), Napi::Function::New(env, CreateFromTiffPtr));
#endif

  exports.Set(Napi::String::New(env, "createFromFile"), Napi::Function::New(env, CreateFromFile));
  exports.Set(Napi::String::New(env, "trueColor"), Napi::Function::New(env, TrueColor));
  exports.Set(Napi::String::New(env, "trueColorAlpha"), Napi::Function::New(env, TrueColorAlpha));
  exports.Set(Napi::String::New(env, "getGDVersion"), Napi::Function::New(env, GdVersionGetter));

  Gd::Image::Init(env, exports);

  return exports;
}

Napi::Value Gd::ImageCreate(const Napi::CallbackInfo &info)
{
  REQ_ARGS(2, "width and height.");
  REQ_INT_ARG(0, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(1, height, "A value for 'height' should be supplied.");

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  CreateWorker *worker = new CreateWorker(info.Env(), "CreateWorkerResource", width, height, 0);
  worker->Queue();

  return worker->_deferred.Promise();
}

Napi::Value Gd::ImageCreateTrueColor(const Napi::CallbackInfo &info)
{
  REQ_ARGS(2, "width and height.");
  REQ_INT_ARG(0, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(1, height, "A value for 'height' should be supplied.");

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  CreateWorker *worker = new CreateWorker(info.Env(), "CreateWorkerResource", width, height, 1);
  worker->Queue();

  return worker->_deferred.Promise();
}

Napi::Value Gd::ImageCreateSync(const Napi::CallbackInfo &info)
{
  REQ_ARGS(2, "width and height.");
  REQ_INT_ARG(0, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(1, height, "A value for 'height' should be supplied.");

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  gdImagePtr img = gdImageCreate(width, height);

  RETURN_IMAGE(img);
}

Napi::Value Gd::ImageCreateTrueColorSync(const Napi::CallbackInfo &info)
{
  REQ_ARGS(2, "width and height.");
  REQ_INT_ARG(0, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(1, height, "A value for 'height' should be supplied.");

  INT_ARG_RANGE(width, "width");
  INT_ARG_RANGE(height, "height");

  gdImagePtr img = gdImageCreateTrueColor(width, height);

  RETURN_IMAGE(img);
}

/**
 * These DECLARE_CREATE_FROM macro calls create functions
 * that return Promises
 */
DECLARE_CREATE_FROM(Jpeg);
DECLARE_CREATE_FROM(Png);
DECLARE_CREATE_FROM(Gif);
DECLARE_CREATE_FROM(WBMP);
DECLARE_CREATE_FROM(Bmp);
#if HAS_LIBWEBP
DECLARE_CREATE_FROM(Webp);
#endif

#if HAS_LIBHEIF
DECLARE_CREATE_FROM(Heif);
#endif
#if HAS_LIBAVIF
DECLARE_CREATE_FROM(Avif);
#endif
#if HAS_LIBTIFF
DECLARE_CREATE_FROM(Tiff);
#endif

/**
 * Returns a Promise
 */
Napi::Value Gd::CreateFromFile(const Napi::CallbackInfo &info)
{
  return CreateFromFileWorker::DoWork(info);
}

Napi::Value Gd::TrueColor(const Napi::CallbackInfo &info)
{
  REQ_ARGS(3, "red, green and blue.");
  REQ_INT_ARG(0, r, "A value for 'red' should be supplied.");
  REQ_INT_ARG(1, g, "A value for 'green' should be supplied.");
  REQ_INT_ARG(2, b, "A value for 'blue' should be supplied.");

  uint32_t result = gdTrueColor(r, g, b);

  return Napi::Number::New(info.Env(), result);
}

Napi::Value Gd::TrueColorAlpha(const Napi::CallbackInfo &info)
{
  REQ_ARGS(4, "red, green, blue and alpha.");
  REQ_INT_ARG(0, r, "A value for 'red' should be supplied.");
  REQ_INT_ARG(1, g, "A value for 'green' should be supplied.");
  REQ_INT_ARG(2, b, "A value for 'blue' should be supplied.");
  REQ_INT_ARG(3, a, "A value for 'alpha' should be supplied.");

  uint32_t result = gdTrueColorAlpha(r, g, b, a);

  return Napi::Number::New(info.Env(), result);
}

Napi::Value Gd::GdVersionGetter(const Napi::CallbackInfo &info)
{
  std::stringstream version_string;
  version_string << GD_MAJOR_VERSION << "." << GD_MINOR_VERSION << "." << GD_RELEASE_VERSION;
  Napi::String s = Napi::String::New(info.Env(), version_string.str());
  return s;
}

/**
 * Image is a subclass of Gd
 */
Napi::Object Gd::Image::Init(Napi::Env env, Napi::Object exports)
{
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
            InstanceMethod("wbmp", &Gd::Image::WBMP),
            InstanceMethod("wbmpPtr", &Gd::Image::WBMPPtr),
#if HAS_LIBWEBP
            InstanceMethod("webp", &Gd::Image::Webp),
            InstanceMethod("webpPtr", &Gd::Image::WebpPtr),
#endif
            InstanceMethod("bmp", &Gd::Image::Bmp),
            InstanceMethod("bmpPtr", &Gd::Image::BmpPtr),
#if HAS_LIBHEIF
            InstanceMethod("heif", &Gd::Image::Heif),
            InstanceMethod("heifPtr", &Gd::Image::HeifPtr),
#endif
#if HAS_LIBAVIF
            InstanceMethod("avif", &Gd::Image::Avif),
            InstanceMethod("avifPtr", &Gd::Image::AvifPtr),
#endif
#if HAS_LIBTIFF
            InstanceMethod("tiff", &Gd::Image::Tiff),
            InstanceMethod("tiffPtr", &Gd::Image::TiffPtr),
#endif
            InstanceMethod("file", &Gd::Image::File),

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
            InstanceMethod("ellipse", &Gd::Image::Ellipse),
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
            InstanceMethod("setResolution", &Gd::Image::SetResolution),

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
            InstanceMethod("colorExactAlpha", &Gd::Image::ColorExactAlpha),
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
            InstanceMethod("emboss", &Gd::Image::Emboss),
            InstanceMethod("sharpen", &Gd::Image::Sharpen),
            InstanceMethod("createPaletteFromTrueColor", &Gd::Image::CreatePaletteFromTrueColor),
            InstanceMethod("trueColorToPalette", &Gd::Image::TrueColorToPalette),
            InstanceMethod("paletteToTrueColor", &Gd::Image::PaletteToTrueColor),
            InstanceMethod("colorMatch", &Gd::Image::ColorMatch),
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
             *
             * static_cast<napi_property_attributes>(napi_default & napi_enumerable)
             */
            InstanceAccessor("trueColor", &Gd::Image::TrueColorGetter, nullptr, napi_enumerable, nullptr),
            InstanceAccessor("width", &Gd::Image::WidthGetter, nullptr, napi_enumerable, nullptr),
            InstanceAccessor("height", &Gd::Image::HeightGetter, nullptr, napi_enumerable, nullptr),
            InstanceAccessor("interlace", &Gd::Image::InterlaceGetter, &Gd::Image::InterlaceSetter, static_cast<napi_property_attributes>(napi_writable | napi_enumerable), nullptr),
            InstanceAccessor("colorsTotal", &Gd::Image::ColorsTotalGetter, nullptr, napi_enumerable, nullptr)
      });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();
  exports.Set("Image", func);
  return exports;
}

Gd::Image::Image(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Image>(info)
{
  if (info.Length() <= 0 || !info[0].IsExternal())
  {
    Napi::TypeError::New(info.Env(), "Argument invalid, should be an Image.").ThrowAsJavaScriptException();
    return;
  }
  Napi::External<gdImagePtr> image =
      info[0].As<Napi::External<gdImagePtr> >();

  gdImagePtr *imgPtr = image.Data();

  this->_image = *imgPtr;
  this->_isDestroyed = false;
}

Gd::Image::~Image()
{
  if (this->_image != nullptr)
  {
    gdImageDestroy(this->_image);

    this->_isDestroyed = true;
    this->_image = nullptr;
  }
}

/**
 * Destruction, Loading and Saving Functions
 */
Napi::Value Gd::Image::Destroy(const Napi::CallbackInfo &info)
{
  if (this->_image != nullptr)
  {
    gdImageDestroy(this->_image);
  }

  this->_isDestroyed = true;
  this->_image = nullptr;
  return info.Env().Undefined();
}

Napi::Value Gd::Image::Jpeg(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveJpegWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::JpegPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;
  OPT_INT_ARG(0, quality, -1);

  int size;
  char *data = (char *)gdImageJpegPtr(this->_image, &size, quality);

  RETURN_DATA;
}

Napi::Value Gd::Image::Gif(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveGifWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::GifPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  int size;
  char *data = (char *)gdImageGifPtr(this->_image, &size);

  RETURN_DATA;
}

Napi::Value Gd::Image::Png(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SavePngWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::PngPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, level, -1);

  int size;
  char *data = (char *)gdImagePngPtrEx(this->_image, &size, level);

  RETURN_DATA
}

Napi::Value Gd::Image::WBMP(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveWBMPWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::WBMPPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, foreground, "The index of the foreground color should be supplied.");

  int size;
  char *data = (char *)gdImageWBMPPtr(this->_image, &size, foreground);

  RETURN_DATA
}

#if HAS_LIBWEBP
Napi::Value Gd::Image::Webp(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveWebpWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::WebpPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, level, -1);

  int size;
  char *data = (char *)gdImageWebpPtrEx(this->_image, &size, level);

  RETURN_DATA
}
#endif

Napi::Value Gd::Image::Bmp(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveBmpWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::BmpPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, compression, 0);

  int size;
  char *data = (char *)gdImageBmpPtr(this->_image, &size, compression);

  RETURN_DATA
}

#if HAS_LIBTIFF
Napi::Value Gd::Image::Tiff(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveTiffWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::TiffPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  int size;
  char *data = (char *)gdImageTiffPtr(this->_image, &size);

  RETURN_DATA
}
#endif

#if HAS_LIBHEIF
Napi::Value Gd::Image::Heif(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveHeifWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::HeifPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;
  OPT_INT_ARG(0, quality, -1);
  OPT_INT_ARG(1, codec_param, 1);
  OPT_STR_ARG(2, chroma, "444");

  gdHeifCodec codec;
  const char *chroma_str = chroma.c_str();

  if (codec_param == 1)
  {
    codec = GD_HEIF_CODEC_HEVC;
  }
  else if (codec_param == 4)
  {
    codec = GD_HEIF_CODEC_AV1;
  }
  else
  {
    codec = GD_HEIF_CODEC_UNKNOWN;
  }

  if (quality < -1 || quality > 200)
  {
    Napi::RangeError::New(info.Env(), "Value for quality must be greater than or equal to 0 and less than or equal to 200").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  if (chroma.compare("444") != 0 && chroma.compare("422") != 0 && chroma.compare("420") != 0)
  {
    Napi::RangeError::New(info.Env(), "Value for chroma must be one of '420', '422' or '444' (default)").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  int size;
  char *data = (char *)gdImageHeifPtrEx(this->_image, &size, quality, codec, chroma_str);

  RETURN_DATA
}
#endif

#if HAS_LIBAVIF
Napi::Value Gd::Image::Avif(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return SaveAvifWorker::DoWork(info, this->_image);
}

Napi::Value Gd::Image::AvifPtr(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;
  OPT_INT_ARG(0, quality, -1);
  OPT_INT_ARG(1, speed, -1);

  int size;
  char *data = (char *)gdImageAvifPtrEx(this->_image, &size, quality, speed);

  RETURN_DATA
}
#endif

Napi::Value Gd::Image::File(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  return FileWorker::DoWork(info, this->_image);
}

/**
 * Drawing Functions
 */
Napi::Value Gd::Image::SetPixel(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(3, "x coordinate, y coordinate and color number.");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");
  REQ_INT_ARG(2, color, "A color number should supplied.");

  gdImageSetPixel(this->_image, x, y, color);

  return info.This();
}

Napi::Value Gd::Image::Line(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "x1, y1, x2, y2 and a color value.");
  REQ_INT_ARG(0, x1, "A value for the x1 coordinate should be supplied.");
  REQ_INT_ARG(1, y1, "A value for the y1 coordinate should be supplied.");
  REQ_INT_ARG(2, x2, "A value for the x2 coordinate should be supplied.");
  REQ_INT_ARG(3, y2, "A value for the y2 coordinate should be supplied.");
  REQ_INT_ARG(4, color, "A color number should supplied.");

  gdImageLine(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::DashedLine(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "x1, y1, x2, y2 and a color value.");
  REQ_INT_ARG(0, x1, "A value for the x1 coordinate should be supplied.");
  REQ_INT_ARG(1, y1, "A value for the y1 coordinate should be supplied.");
  REQ_INT_ARG(2, x2, "A value for the x2 coordinate should be supplied.");
  REQ_INT_ARG(3, y2, "A value for the y2 coordinate should be supplied.");
  REQ_INT_ARG(4, color, "A color number should supplied.");

  gdImageDashedLine(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::Polygon(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "an array of coordinates and a color value.");
  REQ_INT_ARG(1, color, "A color number should supplied.");

  if (!info[0].IsArray())
  {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::String x = Napi::String::New(info.Env(), "x");
  Napi::String y = Napi::String::New(info.Env(), "y");

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  gdPoint *points = new gdPoint[len];

  for (unsigned int i = 0; i < len; i++)
  {
    Napi::Value v = array.Get(i);
    if (!v.IsObject())
      continue;

    Napi::Object o = v.ToObject();
    if (!o.Has(x) || !o.Has(y))
      continue;

    points[i].x = o.Get(x).As<Napi::Number>().Int32Value();
    points[i].y = o.Get(y).As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImagePolygon(this->_image, points, _len, color);

  delete[] points;

  return info.This();
}

Napi::Value Gd::Image::OpenPolygon(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "an array of coordinates and a color value.");
  REQ_INT_ARG(1, color, "A color number should supplied.");

  if (!info[0].IsArray())
  {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::String x = Napi::String::New(info.Env(), "x");
  Napi::String y = Napi::String::New(info.Env(), "y");

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  gdPoint *points = new gdPoint[len];

  for (unsigned int i = 0; i < len; i++)
  {
    Napi::Value v = array.Get(i);
    if (!v.IsObject())
      continue;

    Napi::Object o = v.ToObject();
    if (!o.Has(x) || !o.Has(y))
      continue;

    points[i].x = o.Get(x).As<Napi::Number>().Int32Value();
    points[i].y = o.Get(y).As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImageOpenPolygon(this->_image, points, _len, color);

  delete[] points;

  return info.This();
}

Napi::Value Gd::Image::FilledPolygon(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "an array of coordinates and a color value.");
  REQ_INT_ARG(1, color, "A color number should supplied.");

  if (!info[0].IsArray())
  {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::String x = Napi::String::New(info.Env(), "x");
  Napi::String y = Napi::String::New(info.Env(), "y");

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  gdPoint *points = new gdPoint[len];

  for (unsigned int i = 0; i < len; i++)
  {
    Napi::Value v = array.Get(i);
    if (!v.IsObject())
      continue;

    Napi::Object o = v.ToObject();
    if (!o.Has(x) || !o.Has(y))
      continue;

    points[i].x = o.Get(x).As<Napi::Number>().Int32Value();
    points[i].y = o.Get(y).As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImageFilledPolygon(this->_image, points, _len, color);

  delete[] points;

  return info.This();
}

Napi::Value Gd::Image::Rectangle(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "for x1, y1, x2, y2 and a color value.");
  REQ_INT_ARG(0, x1, "A value for the x1 coordinate should be supplied.");
  REQ_INT_ARG(1, y1, "A value for the y1 coordinate should be supplied.");
  REQ_INT_ARG(2, x2, "A value for the x2 coordinate should be supplied.");
  REQ_INT_ARG(3, y2, "A value for the y2 coordinate should be supplied.");
  REQ_INT_ARG(4, color, "A color number should supplied.");

  gdImageRectangle(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::FilledRectangle(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "for x1, y1, x2, y2 and a color value.");
  REQ_INT_ARG(0, x1, "A value for the x1 coordinate should be supplied.");
  REQ_INT_ARG(1, y1, "A value for the y1 coordinate should be supplied.");
  REQ_INT_ARG(2, x2, "A value for the x2 coordinate should be supplied.");
  REQ_INT_ARG(3, y2, "A value for the y2 coordinate should be supplied.");
  REQ_INT_ARG(4, color, "A color number should supplied.");

  gdImageFilledRectangle(this->_image, x1, y1, x2, y2, color);

  return info.This();
}

Napi::Value Gd::Image::Arc(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(7, "center x, center y, width, height, begin degrees, end degrees and color value.");
  REQ_INT_ARG(0, cx, "A value for the center x coordinate should be supplied.");
  REQ_INT_ARG(1, cy, "A value for the center y coordinate should be supplied.");
  REQ_INT_ARG(2, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(3, height, "A value for 'height' should be supplied.");
  REQ_INT_ARG(4, begin, "A begin value, in degrees, for the arc needs to be supplied.");
  REQ_INT_ARG(5, end, "An end value, in degrees, for the arc needs to be supplied.");
  REQ_INT_ARG(6, color, "A color number should supplied.");

  gdImageArc(this->_image, cx, cy, width, height, begin, end, color);

  return info.This();
}

Napi::Value Gd::Image::FilledArc(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(8, "center x, center y, width, height, begin degrees, end degrees, color value and style.");
  REQ_INT_ARG(0, cx, "A value for the center x coordinate should be supplied.");
  REQ_INT_ARG(1, cy, "A value for the center y coordinate should be supplied.");
  REQ_INT_ARG(2, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(3, height, "A value for 'height' should be supplied.");
  REQ_INT_ARG(4, begin, "A begin value, in degrees, for the arc needs to be supplied.");
  REQ_INT_ARG(5, end, "An end value, in degrees, for the arc needs to be supplied.");
  REQ_INT_ARG(6, color, "A color number should supplied.");
  REQ_INT_ARG(7, style, "Value for style should be supplied.");

  gdImageFilledArc(this->_image, cx, cy, width, height, begin, end, color, style);

  return info.This();
}

Napi::Value Gd::Image::Ellipse(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "center x, center y, width, height and color value.");
  REQ_INT_ARG(0, cx, "A value for the center x coordinate should be supplied.");
  REQ_INT_ARG(1, cy, "A value for the center y coordinate should be supplied.");
  REQ_INT_ARG(2, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(3, height, "A value for 'height' should be supplied.");
  REQ_INT_ARG(4, color, "A color number should supplied.");

  gdImageEllipse(this->_image, cx, cy, width, height, color);

  return info.This();
}

Napi::Value Gd::Image::FilledEllipse(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "center x, center y, width, height and color value.");
  REQ_INT_ARG(0, cx, "A value for the center x coordinate should be supplied.");
  REQ_INT_ARG(1, cy, "A value for the center y coordinate should be supplied.");
  REQ_INT_ARG(2, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(3, height, "A value for 'height' should be supplied.");
  REQ_INT_ARG(4, color, "A color number should supplied.");

  gdImageFilledEllipse(this->_image, cx, cy, width, height, color);

  return info.This();
}

Napi::Value Gd::Image::FillToBorder(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(4, "x coordinate, y coordinate, border offset and color value.");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");
  REQ_INT_ARG(2, border, "A value for the border offset should be supplied.");
  REQ_INT_ARG(3, color, "A color number should supplied.");

  gdImageFillToBorder(this->_image, x, y, border, color);

  return info.This();
}

Napi::Value Gd::Image::Fill(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(3, "x coordinate, y coordinate and a color value.");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");
  REQ_INT_ARG(2, color, "A color number should supplied.");

  gdImageFill(this->_image, x, y, color);

  return info.This();
}

Napi::Value Gd::Image::SetAntiAliased(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");

  gdImageSetAntiAliased(this->_image, color);

  return info.This();
}

Napi::Value Gd::Image::SetAntiAliasedDontBlend(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "a color number and a color value not to blend");
  REQ_INT_ARG(0, color, "A color number should supplied.");
  REQ_INT_ARG(1, dont_blend, "A color number, whether or not to blend the supplied color, should be supplied."); // what does this mean?

  gdImageSetAntiAliasedDontBlend(this->_image, color, dont_blend);

  return info.This();
}

Napi::Value Gd::Image::SetBrush(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_IMG_ARG(0, brush)
  gdImageSetBrush(this->_image, brush);

  return info.This();
}

Napi::Value Gd::Image::SetTile(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_IMG_ARG(0, tile)
  gdImageSetTile(this->_image, tile);

  return info.This();
}

Napi::Value Gd::Image::SetStyle(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  if (info.Length() < 1 || !info[0].IsArray())
  {
    Napi::TypeError::New(info.Env(), "Arguments 0 must be an array").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::Array array = info[0].As<Napi::Array>();
  unsigned int len = array.Length(), _len = 0;
  int *sty = new int[len];

  for (unsigned int i = 0; i < len; i++)
  {
    Napi::Value v = array.Get(i);
    if (!v.IsNumber())
      continue;

    sty[i] = v.As<Napi::Number>().Int32Value();
    _len++;
  }

  gdImageSetStyle(this->_image, sty, _len);

  delete[] sty;

  return info.This();
}

Napi::Value Gd::Image::SetThickness(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, thickness, "Thickness value in pixels should be supplied.");

  gdImageSetThickness(this->_image, thickness);

  return info.This();
}

Napi::Value Gd::Image::AlphaBlending(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, blending, "A value of 0 or 1 for the alpha blending flag should be supplied.")
  gdImageAlphaBlending(this->_image, blending);

  return info.This();
}

Napi::Value Gd::Image::SaveAlpha(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, saveFlag, "A value of 0 or 1 should be supplied to whether or not save the alpha channel of the pixles.")
  gdImageSaveAlpha(this->_image, saveFlag);

  return info.This();
}

Napi::Value Gd::Image::SetClip(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(4, "x1 coordinate, y1 coordinate, x2 coordinate and y2 coordinate.");
  REQ_INT_ARG(0, x1, "A value for the x1 coordinate should be supplied.");
  REQ_INT_ARG(1, y1, "A value for the y1 coordinate should be supplied.");
  REQ_INT_ARG(2, x2, "A value for the x2 coordinate should be supplied.");
  REQ_INT_ARG(3, y2, "A value for the y2 coordinate should be supplied.");

  gdImageSetClip(this->_image, x1, y1, x2, y2);

  return info.This();
}

Napi::Value Gd::Image::GetClip(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  int x1, y1, x2, y2;
  gdImageGetClip(this->_image, &x1, &y1, &x2, &y2);

  Napi::Object result = Napi::Object::New(info.Env());
  (result).Set(Napi::String::New(info.Env(), "x1"), Napi::Number::New(info.Env(), x1));
  (result).Set(Napi::String::New(info.Env(), "y1"), Napi::Number::New(info.Env(), y1));
  (result).Set(Napi::String::New(info.Env(), "x2"), Napi::Number::New(info.Env(), x2));
  (result).Set(Napi::String::New(info.Env(), "y2"), Napi::Number::New(info.Env(), y2));

  return result;
}

Napi::Value Gd::Image::SetResolution(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "horizontal resolution and vertical resolution values.");
  REQ_INT_ARG(0, res_x, "The horizontal resolution in DPI should be supplied.");
  REQ_INT_ARG(1, res_y, "The vertical resolution in DPI should be supplied.");

  gdImageSetResolution(this->_image, res_x, res_y);

  return info.This();
}

/**
 * Query Functions
 */
Napi::Value Gd::Image::GetPixel(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "x coordinate and y coordinate");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");

  if (x < 0 || y < 0)
  {
    Napi::RangeError::New(info.Env(), "Value for x and y must be greater than 0").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  int imageX = gdImageSX(this->_image);
  int imageY = gdImageSY(this->_image);

  Napi::Number result;
  if (x > imageX || y > imageY)
  {
    result = Napi::Number::New(info.Env(), 0);
  }
  else
  {
    result = Napi::Number::New(info.Env(), gdImageGetPixel(this->_image, x, y));
  }

  return result;
}

Napi::Value Gd::Image::GetTrueColorPixel(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "x coordinate and y coordinate");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");

  if (x < 0 || y < 0)
  {
    Napi::RangeError::New(info.Env(), "Value for x and y must be greater than 0").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  int imageX = gdImageSX(this->_image);
  int imageY = gdImageSY(this->_image);

  Napi::Number result;
  if (x > imageX || y > imageY)
  {
    result = Napi::Number::New(info.Env(), 0);
  }
  else
  {
    result = Napi::Number::New(info.Env(), gdImageGetPixel(this->_image, x, y));
  }

  result = Napi::Number::New(info.Env(), gdImageGetTrueColorPixel(this->_image, x, y));
  return result;
}

// This is implementation of the PHP-GD specific method imagecolorat
Napi::Value Gd::Image::ImageColorAt(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "x coordinate and y coordinate");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");

  Napi::Number result;
  gdImagePtr im = this->_image;
  if (gdImageTrueColor(im))
  {
    if (im->tpixels && gdImageBoundsSafe(im, x, y))
    {
      result = Napi::Number::New(info.Env(), gdImageTrueColorPixel(im, x, y));
    }
    else
    {
      Napi::Error::New(info.Env(), "[imageColorAt] Invalid pixel").ThrowAsJavaScriptException();
      return info.Env().Null();
    }
  }
  else
  {
    if (im->pixels && gdImageBoundsSafe(im, x, y))
    {
      result = Napi::Number::New(info.Env(), im->pixels[y][x]);
    }
    else
    {
      Napi::Error::New(info.Env(), "[imageColorAt] Invalid pixel").ThrowAsJavaScriptException();
      return info.Env().Null();
    }
  }
  return result;
}

Napi::Value Gd::Image::GetBoundsSafe(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "x coordinate and y coordinate");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");

  Napi::Number result = Napi::Number::New(info.Env(), gdImageBoundsSafe(this->_image, x, y));
  return result;
}

Napi::Value Gd::Image::WidthGetter(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  Napi::Number result = Napi::Number::New(info.Env(), gdImageSX(this->_image));
  return result;
}

Napi::Value Gd::Image::HeightGetter(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  Napi::Number result = Napi::Number::New(info.Env(), gdImageSY(this->_image));
  return result;
}

Napi::Value Gd::Image::TrueColorGetter(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  Napi::Number result = Napi::Number::New(info.Env(), gdImageTrueColor(this->_image));
  return result;
}

/**
 * Font and Text Handling Functions
 */
Napi::Value Gd::Image::StringFTBBox(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(7, "color value, font list, font size, angle, x coordinate, y coordinate and text to render.");
  REQ_INT_ARG(0, color, "A color number should supplied.");
  REQ_STR_ARG(1, font, "Font list missing.");
  REQ_DOUBLE_ARG(2, size);
  REQ_DOUBLE_ARG(3, angle);
  REQ_INT_ARG(4, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(5, y, "A value for the y coordinate should be supplied.");
  REQ_STR_ARG(6, str, "Text to display in the image missing");

  int brect[8];
  char *err;
  char *fontlist = &font[0];
  char *text = &str[0];

  err = gdImageStringFT(nullptr, &brect[0], color, fontlist, size, angle, x, y, text);
  if (err)
  {
    Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::Array result = Napi::Array::New(info.Env());

  for (unsigned int i = 0; i < 8; i++)
  {
    (result).Set(Napi::Number::New(info.Env(), i), Napi::Number::New(info.Env(), brect[i]));
  }

  return result;
}

Napi::Value Gd::Image::StringFT(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(7, "color value, font list, font size, angle, x coordinate, y coordinate and text to render (optionally 8th argument if the rectangle coordinates should be returnd).");
  REQ_INT_ARG(0, color, "A color number should supplied.");
  REQ_STR_ARG(1, font, "Font list missing.");
  REQ_DOUBLE_ARG(2, size);
  REQ_DOUBLE_ARG(3, angle);
  REQ_INT_ARG(4, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(5, y, "A value for the y coordinate should be supplied.");
  REQ_STR_ARG(6, str, "Text to display is missing.");
  OPT_BOOL_ARG(7, return_rectangle, false);

  int brect[8];
  char *error;
  char *fontlist = &font[0];
  char *text = &str[0];

  if (return_rectangle)
  {
    error = gdImageStringFT(nullptr, &brect[0], color, fontlist, size, angle, x, y, text);
    if (error)
    {
      Napi::Error::New(info.Env(), error).ThrowAsJavaScriptException();
      return info.Env().Null();
    }

    Napi::Array result = Napi::Array::New(info.Env());
    for (unsigned int i = 0; i < 8; i++)
    {
      (result).Set(Napi::Number::New(info.Env(), i), Napi::Number::New(info.Env(), brect[i]));
    }

    return result;
  }

  error = gdImageStringFT(this->_image, &brect[0], color, fontlist, size, angle, x, y, text);
  if (error)
  {
    Napi::Error::New(info.Env(), error).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  return info.This();
}

Napi::Value Gd::Image::StringFTEx(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(8, "color number, font list, font size, angle, x coordinate, y coordinate, text to render, object containing options.");
  REQ_INT_ARG(0, color, "A color number should supplied.");
  REQ_STR_ARG(1, font, "Font list missing.");
  REQ_DOUBLE_ARG(2, size);
  REQ_DOUBLE_ARG(3, angle);
  REQ_INT_ARG(4, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(5, y, "A value for the y coordinate should be supplied.");
  REQ_STR_ARG(6, str, "Text to display missing.");

  if (!info[7].IsObject())
  {
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
  if (stringExtraParameter.Has(linespacing))
  {
    stringExtra.flags |= gdFTEX_LINESPACE;
    stringExtra.linespacing = stringExtraParameter.Get(linespacing)
                                  .As<Napi::Number>()
                                  .DoubleValue();
  }

  // charmap
  if (stringExtraParameter.Has(charmap))
  {
    stringExtra.charmap = 0;

    std::string str = stringExtraParameter.Get(charmap).As<Napi::String>().Utf8Value();
    const char *chmap = str.c_str();

    // gdFTEX_Unicode, gdFTEX_Shift_JIS, gdFTEX_Big5, gdFTEX_Adobe_Custom
    // unicode, shift_jis, big5, adobe_custom
    if (!std::strcmp(chmap, "unicode"))
    {
      stringExtra.charmap = gdFTEX_Unicode;
    }
    else if (!std::strcmp(chmap, "shift_jis"))
    {
      stringExtra.charmap = gdFTEX_Shift_JIS;
    }
    else if (!std::strcmp(chmap, "big5"))
    {
      stringExtra.charmap = gdFTEX_Big5;
    }
    else if (!std::strcmp(chmap, "adobe_custom"))
    {
      stringExtra.charmap = gdFTEX_Adobe_Custom;
    }
    else
    {
      Napi::Error::New(info.Env(),
                       "Unknown value for charmap. Should be one of: unicode, shift_jis, big5, adobe_custom")
          .ThrowAsJavaScriptException();
    }
    stringExtra.flags |= gdFTEX_CHARMAP;
  }

  // resolution
  // horizontal dpi
  // The JavaScript value of NaN will be cast to a 0 by v8?
  if (stringExtraParameter.Has(hdpi))
  {
    stringExtra.flags |= gdFTEX_RESOLUTION;
    stringExtra.hdpi = stringExtraParameter.Get(hdpi).As<Napi::Number>().Int32Value();
    if (!stringExtraParameter.Has(vdpi))
    {
      stringExtra.vdpi = stringExtra.hdpi;
    }
  }

  // vertical dpi
  if (stringExtraParameter.Has(vdpi))
  {
    stringExtra.flags |= gdFTEX_RESOLUTION;
    stringExtra.vdpi = stringExtraParameter.Get(vdpi).As<Napi::Number>().Int32Value();
    if (!stringExtraParameter.Has(hdpi))
    {
      stringExtra.hdpi = stringExtra.vdpi;
    }
  }

  // disable kerning
  if (stringExtraParameter.Has(disable_kerning))
  {
    bool is_disable_kerning = stringExtraParameter.Get(disable_kerning).As<Napi::Boolean>().Value();
    if (is_disable_kerning)
    {
      stringExtra.flags |= gdFTEX_DISABLE_KERNING;
    }
  }

  // xshow
  bool is_xshow = false;
  if (stringExtraParameter.Has(xshow))
  {
    is_xshow = stringExtraParameter.Get(xshow).As<Napi::Boolean>().Value();
    if (is_xshow)
    {
      stringExtra.flags |= gdFTEX_XSHOW;
    }
  }

  // fontpathname
  if (stringExtraParameter.Has(fontpath))
  {
    std::string localFontpathname = stringExtraParameter.Get(fontpath)
                                        .As<Napi::String>()
                                        .Utf8Value();
    stringExtra.flags |= gdFTEX_FONTPATHNAME;
    stringExtra.fontpath = &localFontpathname[0];
  }

  // use_fontconfig
  if (stringExtraParameter.Has(use_fontconfig))
  {
    bool is_use_fontconfig = stringExtraParameter.Get(use_fontconfig).As<Napi::Boolean>().Value();
    if (is_use_fontconfig)
    {
      stringExtra.flags |= gdFTEX_FONTCONFIG;
    }
  }

  // return_fontpathname
  bool is_use_fontpathname = false;
  if (stringExtraParameter.Has(return_fontpathname))
  {
    is_use_fontpathname = stringExtraParameter.Get(return_fontpathname)
                              .As<Napi::Boolean>()
                              .Value();
    if (is_use_fontpathname)
    {
      stringExtra.flags |= gdFTEX_RETURNFONTPATHNAME;
      stringExtra.fontpath = nullptr;
    }
  }

  const char *error;
  char *fontlist = &font[0];
  char *text = &str[0];

  if (return_rectangle)
  {
    error = gdImageStringFTEx(nullptr, &brect[0], color, fontlist, size, angle, x, y, text, &stringExtra);
    if (error)
    {
      std::string prefix("GD Error: ");
      std::string errormsg(error);
      std::string concatenatedStr = (prefix + error);
      const char *errorMessage = concatenatedStr.c_str();
      Napi::Error::New(info.Env(), errorMessage).ThrowAsJavaScriptException();
      return info.Env().Null();
    }

    Napi::Array result = Napi::Array::New(info.Env());
    for (unsigned int i = 0; i < 8; i++)
    {
      (result).Set(Napi::Number::New(info.Env(), i), Napi::Number::New(info.Env(), brect[i]));
    }

    return result;
  }

  error = gdImageStringFTEx(this->_image, &brect[0], color, fontlist, size, angle, x, y, text, &stringExtra);

  if (error)
  {
    std::string prefix("GD Error: ");
    std::string errormsg(error);
    std::string concatenatedStr = (prefix + error);
    const char *errorMessage = concatenatedStr.c_str();
    Napi::Error::New(info.Env(), errorMessage).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  if (is_use_fontpathname && stringExtra.fontpath != nullptr)
  {
    (stringExtraParameter).Set(fontpath, Napi::String::New(info.Env(), stringExtra.fontpath));
  }

  if (is_xshow && stringExtra.xshow != nullptr)
  {
    (stringExtraParameter).Set(xshow, Napi::String::New(info.Env(), stringExtra.xshow));
  }

  return stringExtraParameter;
}

Napi::Value Gd::Image::StringFTCircle(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(9, "center x coordinate, center y coordinate, radius, text radius, fill portion, font list, font size, top distance, bottom distance, color number.");
  REQ_INT_ARG(0, cx, "A value for the center x coordinate should be supplied.");
  REQ_INT_ARG(1, cy, "A value for the center y coordinate should be supplied.");
  REQ_DOUBLE_ARG(2, radius);
  REQ_DOUBLE_ARG(3, textRadius);
  REQ_DOUBLE_ARG(4, fillPortion);
  REQ_STR_ARG(5, font, "Font list missing.");
  REQ_DOUBLE_ARG(6, size);
  REQ_STR_ARG(7, top, "Distance from top in pixels missing");
  REQ_STR_ARG(8, bottom, "Distance form bottom in pixels missing.");
  REQ_INT_ARG(9, color, "A color number should supplied.");

  char *error;
  char *fontlist = &font[0];
  char *topp = &top[0];
  char *bottomm = &bottom[0];

  error = gdImageStringFTCircle(this->_image, cx, cy, radius, textRadius, fillPortion, fontlist,
                                size, topp, bottomm, color);
  if (error)
  {
    Napi::Error::New(info.Env(), error).ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  return info.This();
}

/**
 * Color Handling Functions
 */
Napi::Value Gd::Image::ColorAllocate(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorAllocate(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorAllocateAlpha(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorAllocateAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorClosest(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorClosest(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorClosestAlpha(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorClosestAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorClosestHWB(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorClosestHWB(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorExact(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorExact(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorExactAlpha(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorExactAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorResolve(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorResolve(this->_image, r, g, b));
  return result;
}

Napi::Value Gd::Image::ColorResolveAlpha(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, r, 0);
  OPT_INT_ARG(1, g, 0);
  OPT_INT_ARG(2, b, 0);
  OPT_INT_ARG(3, a, 100);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorResolveAlpha(this->_image, r, g, b, a));
  return result;
}

Napi::Value Gd::Image::ColorsTotalGetter(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorsTotal(this->_image));
  return result;
}

Napi::Value Gd::Image::Red(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");

  Napi::Number result = Napi::Number::New(info.Env(), gdImageRed(this->_image, color));
  return result;
}

Napi::Value Gd::Image::Blue(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");

  Napi::Number result = Napi::Number::New(info.Env(), gdImageBlue(this->_image, color));
  return result;
}

Napi::Value Gd::Image::Green(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");

  Napi::Number result = Napi::Number::New(info.Env(), gdImageGreen(this->_image, color));
  return result;
}

Napi::Value Gd::Image::Alpha(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");

  Napi::Number result = Napi::Number::New(info.Env(), gdImageAlpha(this->_image, color));
  return result;
}

Napi::Value Gd::Image::InterlaceGetter(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  bool interlaced = (gdImageGetInterlaced(this->_image) != 0);
  Napi::Boolean result;
  if (interlaced)
  {
    result = Napi::Boolean::New(info.Env(), true);
  }
  else
  {
    result = Napi::Boolean::New(info.Env(), false);
  }

  return result;
}

void Gd::Image::InterlaceSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  if (this->_isDestroyed)
  {
    Napi::Error::New(info.Env(), "Image is already destroyed.")
        .ThrowAsJavaScriptException();
  }

  if (value.IsBoolean())
  {
    bool interlace = value.As<Napi::Boolean>().Value();

    gdImageInterlace(this->_image, interlace ? 1 : 0);
  }
}

Napi::Value Gd::Image::GetTransparent(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  Napi::Number result = Napi::Number::New(info.Env(), gdImageGetTransparent(this->_image));
  return result;
}

Napi::Value Gd::Image::ColorDeallocate(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");
  gdImageColorDeallocate(this->_image, color);

  return info.This();
}

Napi::Value Gd::Image::ColorTransparent(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");
  gdImageColorTransparent(this->_image, color);

  return info.This();
}

Napi::Value Gd::Image::ColorReplace(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "'from' color value and 'to' color value.");
  REQ_INT_ARG(0, fromColor, "A 'from' color number should supplied.");
  REQ_INT_ARG(1, toColor, "A 'to' color number should supplied.");

  Napi::Number result = Napi::Number::New(info.Env(), gdImageColorReplace(this->_image, fromColor, toColor));

  return result;
}

Napi::Value Gd::Image::ColorReplaceThreshold(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(3, "'from' color value, 'to' color value and threshold");
  REQ_INT_ARG(0, fromColor, "A 'from' color number should supplied.");
  REQ_INT_ARG(1, toColor, "A 'to' color number should supplied.");
  REQ_DOUBLE_ARG(2, threshold);

  Napi::Number result =
      Napi::Number::New(info.Env(), gdImageColorReplaceThreshold(this->_image, fromColor, toColor, threshold));

  return result;
}

Napi::Value Gd::Image::ColorReplaceArray(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "array of 'from' colors, array of 'to' colors.");

  Napi::Array fromArray = info[0].As<Napi::Array>();
  unsigned int flen = fromArray.Length(), _flen = 0;
  int *fromColors = new int[flen];

  for (unsigned int i = 0; i < flen; i++)
  {
    Napi::Value v = fromArray.Get(i);
    fromColors[i] = v.As<Napi::Number>().Int32Value();
    _flen++;
  }

  Napi::Array toArray = info[1].As<Napi::Array>();
  unsigned int tlen = toArray.Length(), _tlen = 0;
  int *toColors = new int[tlen];

  for (unsigned int j = 0; j < tlen; j++)
  {
    Napi::Value v = toArray.Get(j);
    toColors[j] = v.As<Napi::Number>().Int32Value();
    _tlen++;
  }

  if (_flen != _tlen)
  {
    Napi::Error::New(info.Env(), "Color arrays should have same length.")
        .ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Napi::Number result =
      Napi::Number::New(info.Env(),
                        gdImageColorReplaceArray(this->_image, _flen, fromColors, toColors));

  return result;
}

Napi::Value Gd::Image::GrayScale(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageGrayScale(this->_image);

  return info.This();
}

Napi::Value Gd::Image::GaussianBlur(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageGaussianBlur(this->_image);

  return info.This();
}

Napi::Value Gd::Image::Negate(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageNegate(this->_image);

  return info.This();
}

Napi::Value Gd::Image::Brightness(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, brightness, "A value between -255 and 255 should be supplied for brightness.");

  gdImageBrightness(this->_image, brightness);

  return info.This();
}

Napi::Value Gd::Image::Contrast(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_DOUBLE_ARG(0, contrast);

  gdImageContrast(this->_image, contrast);

  return info.This();
}

Napi::Value Gd::Image::SelectiveBlur(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageSelectiveBlur(this->_image);
  return info.This();
}

Napi::Value Gd::Image::FlipHorizontal(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageFlipHorizontal(this->_image);
  return info.This();
}

Napi::Value Gd::Image::FlipVertical(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageFlipVertical(this->_image);
  return info.This();
}

Napi::Value Gd::Image::FlipBoth(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageFlipBoth(this->_image);
  return info.This();
}

Napi::Value Gd::Image::Crop(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(4, "x coordinate, y coordinate, width and height.");
  REQ_INT_ARG(0, x, "A value for the x coordinate should be supplied.");
  REQ_INT_ARG(1, y, "A value for the y coordinate should be supplied.");
  REQ_INT_ARG(2, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(3, height, "A value for 'height' should be supplied.");

  gdRect *rect = new gdRect;

  rect->x = x;
  rect->y = y;
  rect->width = (width == 0) ? 100 : width;
  rect->height = (height == 0) ? 100 : height;

  gdImagePtr newImage = gdImageCrop(this->_image, rect);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::CropAuto(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, mode, "A value for crop mode should be supplied, between 0 and 5.");

  if (mode > 4)
  {
    Napi::RangeError::New(info.Env(), "Crop mode should be between 0 and 5.")
        .ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  gdImagePtr newImage = gdImageCropAuto(this->_image, mode);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::CropThreshold(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, color, "A color number should supplied.");
  REQ_DOUBLE_ARG(1, threshold);

  gdImagePtr newImage = gdImageCropThreshold(this->_image, color, threshold);
  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::Emboss(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  gdImageEmboss(this->_image);
  return info.This();
}

/**
 * Copying and Resizing Functions
 */
Napi::Value Gd::Image::Copy(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(7, "destination image, dest x coordinate, dest y coordinate, source x cooridinate, source y coordinate, width and height.");
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX, "A value for the destination x coordinate should be supplied.");
  REQ_INT_ARG(2, dstY, "A value for the destination y coordinate should be supplied.");
  REQ_INT_ARG(3, srcX, "A value for the source x coordinate should be supplied.");
  REQ_INT_ARG(4, srcY, "A value for the source y coordinate should be supplied.");
  REQ_INT_ARG(5, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(6, height, "A value for 'height' should be supplied.");

  gdImageCopy(dest, this->_image, dstX, dstY, srcX, srcY, width, height);

  return info.This();
}

Napi::Value Gd::Image::CopyResized(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(9, "destination image, dest x coordinate, dest y coordinate, source x cooridinate, source y coordinate, destination width, destination height, source width and source height.");
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX, "A value for the destination x coordinate should be supplied.");
  REQ_INT_ARG(2, dstY, "A value for the destination y coordinate should be supplied.");
  REQ_INT_ARG(3, srcX, "A value for the source x coordinate should be supplied.");
  REQ_INT_ARG(4, srcY, "A value for the source y coordinate should be supplied.");
  REQ_INT_ARG(5, destW, "A value for the destination width should be supplied.");
  REQ_INT_ARG(6, destH, "A value for the destination height should be supplied.");
  REQ_INT_ARG(7, srcW, "A value for the source width should be supplied.");
  REQ_INT_ARG(8, srcH, "A value for the source height should be supplied.");

  gdImageCopyResized(dest, this->_image, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

  return info.This();
}

Napi::Value Gd::Image::CopyResampled(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(9, "destination image, dest x coordinate, dest y coordinate, source x cooridinate, source y coordinate, destination width, destination height, source width and source height.");
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX, "A value for the destination x coordinate should be supplied.");
  REQ_INT_ARG(2, dstY, "A value for the destination y coordinate should be supplied.");
  REQ_INT_ARG(3, srcX, "A value for the source x coordinate should be supplied.");
  REQ_INT_ARG(4, srcY, "A value for the source y coordinate should be supplied.");
  REQ_INT_ARG(5, destW, "A value for the destination width should be supplied.");
  REQ_INT_ARG(6, destH, "A value for the destination height should be supplied.");
  REQ_INT_ARG(7, srcW, "A value for the source width should be supplied.");
  REQ_INT_ARG(8, srcH, "A value for the source height should be supplied.");

  gdImageCopyResampled(dest, this->_image, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

  return info.This();
}

Napi::Value Gd::Image::CopyRotated(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(8, "destination image, dest x coordinate, dest y coordinate, source x cooridinate, source y coordinate, destination width, destination height and angle in degrees.");
  REQ_IMG_ARG(0, dest);
  REQ_DOUBLE_ARG(1, dstX);
  REQ_DOUBLE_ARG(2, dstY);
  REQ_INT_ARG(3, srcX, "A value for the source x coordinate should be supplied.");
  REQ_INT_ARG(4, srcY, "A value for the source y coordinate should be supplied.");
  REQ_INT_ARG(5, srcW, "A value for the source width should be supplied.");
  REQ_INT_ARG(6, srcH, "A value for the source height should be supplied.");
  REQ_INT_ARG(7, angle, "A value for angle in degrees should be supplied.");

  gdImageCopyRotated(dest, this->_image, dstX, dstY, srcX, srcY, srcW, srcH, angle);

  return info.This();
}

Napi::Value Gd::Image::CopyMerge(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(8, "destination image, dest x coordinate, dest y coordinate, source x cooridinate, source y coordinate, width, height and percentage value.");
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX, "A value for the destination x coordinate should be supplied.");
  REQ_INT_ARG(2, dstY, "A value for the destination y coordinate should be supplied.");
  REQ_INT_ARG(3, srcX, "A value for the source x coordinate should be supplied.");
  REQ_INT_ARG(4, srcY, "A value for the source y coordinate should be supplied.");
  REQ_INT_ARG(5, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(6, height, "A value for 'height' should be supplied.");
  REQ_INT_ARG(7, pct, "A percentage value between 0 and 100 should be supplied.");

  gdImageCopyMerge(dest, this->_image, dstX, dstY, srcX, srcY, width, height, pct);

  return info.This();
}

Napi::Value Gd::Image::CopyMergeGray(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(8, "destination image, destination x coordinate, destination y coordinate, source x coordinate, source y coordinate, width, height and percentage value.");
  REQ_IMG_ARG(0, dest);
  REQ_INT_ARG(1, dstX, "A value for the destination x coordinate should be supplied.");
  REQ_INT_ARG(2, dstY, "A value for the destination y coordinate should be supplied.");
  REQ_INT_ARG(3, srcX, "A value for the source x coordinate should be supplied.");
  REQ_INT_ARG(4, srcY, "A value for the source y coordinate should be supplied.");
  REQ_INT_ARG(5, width, "A value for 'width' should be supplied.");
  REQ_INT_ARG(6, height, "A value for 'height' should be supplied.");
  REQ_INT_ARG(7, pct, "A value for the percentage of the source color intensity in range 0..100 should be supplied.");

  gdImageCopyMergeGray(dest, this->_image, dstX, dstY, srcX, srcY, width, height, pct);

  return info.This();
}

Napi::Value Gd::Image::PaletteCopy(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_IMG_ARG(0, dest);

  gdImagePaletteCopy(dest, this->_image);

  return info.This();
}

Napi::Value Gd::Image::SquareToCircle(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, radius, "A value for the radius of the new image should be supplied.");

  gdImagePtr newImage = gdImageSquareToCircle(this->_image, radius);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::Sharpen(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_INT_ARG(0, pct, "A percentage value for sharpening should be supplied. This can be greater than 100.");

  gdImageSharpen(this->_image, pct);

  return info.This();
}

Napi::Value Gd::Image::CreatePaletteFromTrueColor(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, ditherFlag, 0);
  OPT_INT_ARG(1, colorsWanted, 256);

  gdImagePtr newImage = gdImageCreatePaletteFromTrueColor(this->_image, ditherFlag, colorsWanted);

  RETURN_IMAGE(newImage);
}

Napi::Value Gd::Image::TrueColorToPalette(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  OPT_INT_ARG(0, ditherFlag, 0);
  OPT_INT_ARG(1, colorsWanted, 256);

  Napi::Number result = Napi::Number::New(info.Env(), gdImageTrueColorToPalette(this->_image, ditherFlag, colorsWanted));
  return result;
}

Napi::Value Gd::Image::PaletteToTrueColor(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  Napi::Number result = Napi::Number::New(info.Env(), gdImagePaletteToTrueColor(this->_image));

  return result;
}

Napi::Value Gd::Image::ColorMatch(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_IMG_ARG(0, palette);

  int gdResult = gdImageColorMatch(this->_image, palette);

  if (gdResult == -1)
  {
    Napi::Error::New(info.Env(), "Image of this should be truecolor")
        .ThrowAsJavaScriptException();
  }
  if (gdResult == -2)
  {
    Napi::Error::New(info.Env(), "Image from argument must be palette / indexed")
        .ThrowAsJavaScriptException();
  }
  if (gdResult == -3)
  {
    Napi::Error::New(info.Env(), "Images must have same dimensions")
        .ThrowAsJavaScriptException();
  }
  if (gdResult == -4)
  {
    Napi::Error::New(info.Env(), "At least 1 color must be allocated")
        .ThrowAsJavaScriptException();
  }

  Napi::Number result = Napi::Number::New(info.Env(), gdResult);

  return result;
}

Napi::Value Gd::Image::GifAnimBegin(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(2, "for global color map and amount of loops.");
  REQ_INT_ARG(0, GlobalCM, "A value for the global color map should be supplied."); // global color map
  REQ_INT_ARG(1, Loops, "A value for the amount of loops should be supplied.");

  int size;
  // create new buffer containing result
  char *data = (char *)gdImageGifAnimBeginPtr(this->_image, &size, GlobalCM, Loops);

  if (data == nullptr)
  {
    return Napi::Boolean::New(info.Env(), false);
  }

  RETURN_DATA;
}

Napi::Value Gd::Image::GifAnimAdd(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(5, "local color map flag, left offset, top offset, delay and disposal mode.");
  REQ_INT_ARG(0, LocalCM, "Flag. If 1, use a local Color Map for this frame");
  REQ_INT_ARG(1, LeftOfs, "A value for the left offset of image in frame should be supplied.");
  REQ_INT_ARG(2, TopOfs, "A value for the top offset of image in frame should be supplied.");
  REQ_INT_ARG(3, Delay, "A value for the delay before next frame (in 1/100 seconds) should be supplied.");
  REQ_INT_ARG(4, Disposal, "A value for the disposal mode should be supplied.");

  int size;
  char *data;

  if (info.Length() <= 5)
  {
    Napi::TypeError::New(info.Env(), "Argument 5 must be an object").ThrowAsJavaScriptException();
    return info.Env().Null();
  }
  else if (info[5].IsObject())
  {
    Gd::Image *_obj_ = Napi::ObjectWrap<Gd::Image>::Unwrap(info[5].As<Napi::Object>());
    gdImagePtr prevFrame = _obj_->getGdImagePtr();
    data = (char *)gdImageGifAnimAddPtr(this->_image, &size, LocalCM, LeftOfs, TopOfs, Delay, Disposal, prevFrame);
  }
  else
  {
    data = (char *)gdImageGifAnimAddPtr(this->_image, &size, LocalCM, LeftOfs, TopOfs, Delay, Disposal, nullptr);
  }

  if (data == nullptr)
  {
    return Napi::Boolean::New(info.Env(), false);
  }

  RETURN_DATA;
}

Napi::Value Gd::Image::GifAnimEnd(const Napi::CallbackInfo &info)
{
  int size;
  char *data = (char *)gdImageGifAnimEndPtr(&size);

  if (data == 0)
  {
    return Napi::Boolean::New(info.Env(), false);
  }

  return Napi::Boolean::New(info.Env(), true);
}

/**
 * Miscellaneous Functions
 */
Napi::Value Gd::Image::Compare(const Napi::CallbackInfo &info)
{
  CHECK_IMAGE_EXISTS;

  REQ_ARGS(1, "of type Image.");
  if (!info[0].IsObject())
  {
    Napi::TypeError::New(info.Env(), "Argument 0 must be an image").ThrowAsJavaScriptException();
    return info.Env().Null();
  }

  Gd::Image *_obj_ = Napi::ObjectWrap<Gd::Image>::Unwrap(info[0].As<Napi::Object>());
  gdImagePtr im2 = _obj_->getGdImagePtr();

  Napi::Number result = Napi::Number::New(info.Env(), gdImageCompare(this->_image, im2));

  return result;
}

Napi::FunctionReference Gd::Image::constructor;
