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

#ifndef NODE_GD_H
#define NODE_GD_H

#include <napi.h>
#include <gd.h>

#define SUPPORTS_GD_2_2_5 (GD_MINOR_VERSION == 2                        \
                  && GD_RELEASE_VERSION >= 5)

#define SUPPORTS_GD_2_2_4 (GD_MINOR_VERSION == 2                        \
                  && GD_RELEASE_VERSION >= 4)

#define SUPPORTS_GD_2_2_0 (SUPPORTS_GD_2_2_4                            \
                  || (GD_MINOR_VERSION == 2                             \
                  && GD_RELEASE_VERSION >= 0))

#define SUPPORTS_GD_2_1_1 (SUPPORTS_GD_2_2_0                            \
                  || (GD_MINOR_VERSION == 1                             \
                  && GD_RELEASE_VERSION == 1))

#define SUPPORTS_GD_2_1_0 (SUPPORTS_GD_2_1_1                            \
                  || (GD_MINOR_VERSION == 1                             \
                  && GD_RELEASE_VERSION == 0))

#define SUPPORTS_GD_2_0_36 (SUPPORTS_GD_2_1_0                           \
                  || SUPPORTS_GD_2_1_0                                  \
                  || (GD_MINOR_VERSION == 0                             \
                  && GD_RELEASE_VERSION <= 36))

#define SUPPORTS_UNTIL_GD_2_0_36 (GD_MINOR_VERSION == 0                 \
                  && GD_RELEASE_VERSION <= 36)

#define HAS_LIBTIFF (HAVE_LIBTIFF && SUPPORTS_GD_2_2_4)
#define HAS_LIBWEBP (HAVE_LIBWEBP && SUPPORTS_GD_2_1_0)

// Since gd 2.0.28, these are always built in
#define GD_GIF          1
#define GD_GIFANIM      1
#define GD_OPENPOLYGON  1

#define COLOR_ANTIALIASED    gdAntiAliased
#define COLOR_BRUSHED        gdBrushed
#define COLOR_STYLED         gdStyled
#define COLOR_STYLEDBRUSHED  gdStyledBrushed
#define COLOR_TITLED         gdTiled
#define COLOR_TRANSPARENT    gdTransparent


#define REQ_ARGS(N)                                                     \
  if (info.Length() < (N)) {                                            \
    Napi::Error::New(info.Env(),                                        \
      "Expected " #N " arguments")                                      \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }

#define REQ_STR_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I].IsString()) {                    \
    Napi::TypeError::New(info.Env(),                                    \
      "Argument " #I " must be a string")                               \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }                                                                     \
  std::string VAR = info[I].As<Napi::String>().Utf8Value().c_str();

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (info.Length() <= (I) || !info[I].IsNumber()) {                    \
    Napi::TypeError::New(info.Env(),                                    \
      "Argument " #I " must be an integer")                             \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }                                                                     \
  VAR = info[I].ToNumber();

#define INT_ARG_RANGE(I, PROP)                                          \
  if ((I) < 1) {                                                        \
    Napi::RangeError::New(info.Env(),                                   \
      "Value for " #PROP " must be greater than 0")                     \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }

#define REQ_FN_ARG(I, VAR)                                              \
  if (info.Length() <= (I) || !info[I].IsFunction()) {                  \
    Napi::TypeError::New(info.Env(),                                    \
      "Argument " #I " must be a function")                             \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }                                                                     \
  Napi::Function VAR = info[I].As<Napi::Function>();

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (info.Length() <= (I) || !info[I].IsNumber()) {                    \
    Napi::TypeError::New(info.Env(),                                    \
      "Argument " #I " must be a number")                               \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }                                                                     \
  VAR = info[I].ToNumber();

#define REQ_EXT_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I].IsExternal()) {                  \
    Napi::TypeError::New(info.Env(),                                    \
      "Argument " #I " invalid")                                        \
      .ThrowAsJavaScriptException();                                    \
    return;                                           \
  }                                                                     \
  Napi::External<gdImagePtr> VAR =                                      \
    info[I].As<Napi::External<gdImagePtr>>();

#define REQ_IMG_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I].IsObject()) {                    \
    Napi::TypeError::New(info.Env(),                                    \
      "Argument " #I " must be an object")                              \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }                                                                     \
  Gd::Image* _obj_ =                                                    \
    Napi::ObjectWrap<Gd::Image>::Unwrap(info[I].As<Napi::Object>());    \
  gdImagePtr VAR = _obj_->getGdImagePtr();

#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (info.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (info[I].IsNumber()) {                                      \
    VAR = info[I].ToNumber();                                           \
  } else {                                                              \
    Napi::TypeError::New(info.Env(),                                    \
      "Optional argument " #I " must be an integer")                    \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }

#define OPT_BOOL_ARG(I, VAR, DEFAULT)                                   \
  bool VAR;                                                             \
  if (info.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (info[I].IsBoolean()) {                                     \
    VAR = info[I].ToBoolean();                                          \
  } else {                                                              \
    Napi::TypeError::New(info.Env(),                                    \
      "Optional argument " #I " must be a boolean")                     \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }

#define RETURN_IMAGE(IMG)                                               \
  if (!IMG) {                                                           \
    return info.Env().Null();                                           \
  } else {                                                              \
    Napi::Value argv =                                                  \
      Napi::External<gdImagePtr>::New(info.Env(), &IMG);                \
    Napi::Object instance = Gd::Image::constructor.New({argv});         \
    return instance;                                                    \
  }

#define DECLARE_CREATE_FROM(TYPE)                                       \
  Napi::Value Gd::CreateFrom##TYPE(const Napi::CallbackInfo& info) {    \
    return CreateFrom##TYPE##Worker::DoWork(info);                      \
  }                                                                     \
  Napi::Value Gd::CreateFrom##TYPE##Ptr                                 \
      (const Napi::CallbackInfo& info) {                                \
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
    Napi::TypeError::New(info.Env(), "Argument not a Buffer")           \
      .ThrowAsJavaScriptException();                                    \
    return info.Env().Null();                                           \
  }

#define RETURN_DATA                                                     \
  Napi::Buffer<char> result =                                           \
    Napi::Buffer<char>::Copy(info.Env(), data, size);                   \
  gdFree(data);                                                         \
  return result;

#define CHECK_IMAGE_EXISTS                                              \
  if (_isDestroyed) {                                                   \
    Napi::Error::New(info.Env(), "Image is already destroyed")          \
            .ThrowAsJavaScriptException();                              \
    return info.Env().Undefined();                                      \
  }                                                                     \
  if (_image == nullptr) {                                              \
    Napi::Error::New(info.Env(), "Image does not exist")                \
            .ThrowAsJavaScriptException();                              \
    return info.Env().Undefined();                                      \
  }

class Gd : public Napi::ObjectWrap<Gd> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  class Image : public Napi::ObjectWrap<Image> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    Image(const Napi::CallbackInfo &info);
    ~Image();

    static Napi::FunctionReference constructor;

    gdImagePtr getGdImagePtr() const { return _image; }
  private:
    gdImagePtr _image{nullptr};

    bool _isDestroyed{true};

    operator gdImagePtr () const { return _image; }

    /**
     * Destruction, Loading and Saving Functions
     */
    Napi::Value Destroy(const Napi::CallbackInfo& info);
    Napi::Value Jpeg(const Napi::CallbackInfo& info);
    Napi::Value JpegPtr(const Napi::CallbackInfo& info);
    Napi::Value Gif(const Napi::CallbackInfo& info);
    Napi::Value GifPtr(const Napi::CallbackInfo& info);
    Napi::Value Png(const Napi::CallbackInfo& info);
    Napi::Value PngPtr(const Napi::CallbackInfo& info);
    Napi::Value WBMP(const Napi::CallbackInfo& info);
    Napi::Value WBMPPtr(const Napi::CallbackInfo& info);
#if HAS_LIBWEBP
    Napi::Value Webp(const Napi::CallbackInfo& info);
    Napi::Value WebpPtr(const Napi::CallbackInfo& info);
#endif
    Napi::Value Gd(const Napi::CallbackInfo& info);
    Napi::Value GdPtr(const Napi::CallbackInfo& info);
    Napi::Value Gd2(const Napi::CallbackInfo& info);
    Napi::Value Gd2Ptr(const Napi::CallbackInfo& info);
#if SUPPORTS_GD_2_1_0
    Napi::Value Bmp(const Napi::CallbackInfo& info);
    Napi::Value BmpPtr(const Napi::CallbackInfo& info);
#endif

#if HAS_LIBTIFF
    Napi::Value Tiff(const Napi::CallbackInfo& info);
    Napi::Value TiffPtr(const Napi::CallbackInfo& info);
#endif

#if SUPPORTS_GD_2_1_1
    Napi::Value File(const Napi::CallbackInfo& info);
#endif
    /**
     * Drawing Functions
     */
    Napi::Value SetPixel(const Napi::CallbackInfo& info);
    Napi::Value Line(const Napi::CallbackInfo& info);
    Napi::Value DashedLine(const Napi::CallbackInfo& info);
    Napi::Value Polygon(const Napi::CallbackInfo& info);
    Napi::Value OpenPolygon(const Napi::CallbackInfo& info);
    Napi::Value FilledPolygon(const Napi::CallbackInfo& info);
    Napi::Value Rectangle(const Napi::CallbackInfo& info);
    Napi::Value FilledRectangle(const Napi::CallbackInfo& info);
    Napi::Value Arc(const Napi::CallbackInfo& info);
    Napi::Value FilledArc(const Napi::CallbackInfo& info);
#if SUPPORTS_GD_2_1_0
    Napi::Value Ellipse(const Napi::CallbackInfo& info);
#endif

    Napi::Value FilledEllipse(const Napi::CallbackInfo& info);
    Napi::Value FillToBorder(const Napi::CallbackInfo& info);
    Napi::Value Fill(const Napi::CallbackInfo& info);
    Napi::Value SetAntiAliased(const Napi::CallbackInfo& info);
    Napi::Value SetAntiAliasedDontBlend(const Napi::CallbackInfo& info);
    Napi::Value SetBrush(const Napi::CallbackInfo& info);
    Napi::Value SetTile(const Napi::CallbackInfo& info);
    Napi::Value SetStyle(const Napi::CallbackInfo& info);
    Napi::Value SetThickness(const Napi::CallbackInfo& info);
    Napi::Value AlphaBlending(const Napi::CallbackInfo& info);
    Napi::Value SaveAlpha(const Napi::CallbackInfo& info);
    Napi::Value SetClip(const Napi::CallbackInfo& info);
    Napi::Value GetClip(const Napi::CallbackInfo& info);
#if SUPPORTS_GD_2_1_0
    Napi::Value SetResolution(const Napi::CallbackInfo& info);
#endif

    /**
     * Query Functions
     */
    Napi::Value GetPixel(const Napi::CallbackInfo& info);
    Napi::Value GetTrueColorPixel(const Napi::CallbackInfo& info);
    // This is implementation of the PHP-GD specific method imagecolorat
    Napi::Value ImageColorAt(const Napi::CallbackInfo& info);
    Napi::Value GetBoundsSafe(const Napi::CallbackInfo& info);
    Napi::Value WidthGetter(const Napi::CallbackInfo& info);
    Napi::Value HeightGetter(const Napi::CallbackInfo& info);
    Napi::Value TrueColorGetter(const Napi::CallbackInfo& info);
    /**
     * Font and Text Handling Funcitons
     */
    Napi::Value StringFTBBox(const Napi::CallbackInfo& info);
    Napi::Value StringFT(const Napi::CallbackInfo& info);
    Napi::Value StringFTEx(const Napi::CallbackInfo& info);
    Napi::Value StringFTCircle(const Napi::CallbackInfo& info);
    /**
     * Color Handling Functions
     */
    Napi::Value ColorAllocate(const Napi::CallbackInfo& info);
    Napi::Value ColorAllocateAlpha(const Napi::CallbackInfo& info);
    Napi::Value ColorClosest(const Napi::CallbackInfo& info);
    Napi::Value ColorClosestAlpha(const Napi::CallbackInfo& info);
    Napi::Value ColorClosestHWB(const Napi::CallbackInfo& info);
    Napi::Value ColorExact(const Napi::CallbackInfo& info);
    Napi::Value ColorResolve(const Napi::CallbackInfo& info);
    Napi::Value ColorResolveAlpha(const Napi::CallbackInfo& info);
    Napi::Value ColorsTotalGetter(const Napi::CallbackInfo& info);
    Napi::Value Red(const Napi::CallbackInfo& info);
    Napi::Value Blue(const Napi::CallbackInfo& info);
    Napi::Value Green(const Napi::CallbackInfo& info);
    Napi::Value Alpha(const Napi::CallbackInfo& info);
    Napi::Value InterlaceGetter(const Napi::CallbackInfo& info);
    void InterlaceSetter(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetTransparent(const Napi::CallbackInfo& info);
    Napi::Value ColorDeallocate(const Napi::CallbackInfo& info);
    Napi::Value ColorTransparent(const Napi::CallbackInfo& info);
#if SUPPORTS_GD_2_1_0
    Napi::Value ColorReplace(const Napi::CallbackInfo& info);
    Napi::Value ColorReplaceThreshold(const Napi::CallbackInfo& info);
    Napi::Value ColorReplaceArray(const Napi::CallbackInfo& info);
    Napi::Value GrayScale(const Napi::CallbackInfo& info);
    Napi::Value GaussianBlur(const Napi::CallbackInfo& info);
    Napi::Value Negate(const Napi::CallbackInfo& info);
    Napi::Value Brightness(const Napi::CallbackInfo& info);
    Napi::Value Contrast(const Napi::CallbackInfo& info);
    Napi::Value SelectiveBlur(const Napi::CallbackInfo& info);
    Napi::Value FlipHorizontal(const Napi::CallbackInfo& info);
    Napi::Value FlipVertical(const Napi::CallbackInfo& info);
    Napi::Value FlipBoth(const Napi::CallbackInfo& info);
    Napi::Value Crop(const Napi::CallbackInfo& info);
    Napi::Value CropAuto(const Napi::CallbackInfo& info);
    Napi::Value CropThreshold(const Napi::CallbackInfo& info);
#endif

#if SUPPORTS_GD_2_1_1
    Napi::Value Emboss(const Napi::CallbackInfo& info);
#endif

    /**
     * Copying and Resizing Functions
     */
    Napi::Value Copy(const Napi::CallbackInfo& info);
    Napi::Value CopyResized(const Napi::CallbackInfo& info);
    Napi::Value CopyResampled(const Napi::CallbackInfo& info);
    Napi::Value CopyRotated(const Napi::CallbackInfo& info);
    Napi::Value CopyMerge(const Napi::CallbackInfo& info);
    Napi::Value CopyMergeGray(const Napi::CallbackInfo& info);
    Napi::Value PaletteCopy(const Napi::CallbackInfo& info);
    Napi::Value SquareToCircle(const Napi::CallbackInfo& info);
    Napi::Value Sharpen(const Napi::CallbackInfo& info);
    Napi::Value CreatePaletteFromTrueColor(const Napi::CallbackInfo& info);
    Napi::Value TrueColorToPalette(const Napi::CallbackInfo& info);

#if SUPPORTS_GD_2_1_0
    Napi::Value PaletteToTrueColor(const Napi::CallbackInfo& info);
    Napi::Value ColorMatch(const Napi::CallbackInfo& info);
#endif

    Napi::Value GifAnimBegin(const Napi::CallbackInfo& info);
    Napi::Value GifAnimAdd(const Napi::CallbackInfo& info);
    Napi::Value GifAnimEnd(const Napi::CallbackInfo& info);
    /**
     * Miscellaneous Functions
     */
    Napi::Value Compare(const Napi::CallbackInfo& info);
  };

private:
  /**
   * Section A - Creation of new image in memory
   */
  static Napi::Value ImageCreate(const Napi::CallbackInfo& info);
  static Napi::Value ImageCreateTrueColor(const Napi::CallbackInfo& info);
  static Napi::Value ImageCreateSync(const Napi::CallbackInfo& info);
  static Napi::Value ImageCreateTrueColorSync(const Napi::CallbackInfo& info);

  /**
   * Section B - Creation of image in memory from a source (either file or Buffer)
   */
  static Napi::Value CreateFromJpeg(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromJpegPtr(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromPng(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromPngPtr(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromGif(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromGifPtr(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromGd2(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromGd2Ptr(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromWBMP(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromWBMPPtr(const Napi::CallbackInfo& info);
#if HAS_LIBWEBP
  static Napi::Value CreateFromWebp(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromWebpPtr(const Napi::CallbackInfo& info);
#endif

#if SUPPORTS_GD_2_1_0
  static Napi::Value CreateFromBmp(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromBmpPtr(const Napi::CallbackInfo& info);
#endif
#if HAS_LIBTIFF
  static Napi::Value CreateFromTiff(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromTiffPtr(const Napi::CallbackInfo& info);
#endif

/**
 * Section C - Creation of image in memory from a file, type based on file extension
 */
#if SUPPORTS_GD_2_1_1
  static Napi::Value CreateFromFile(const Napi::CallbackInfo& info);
#endif

  static Napi::Value CreateFromGd2Part(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromGd2PartPtr(const Napi::CallbackInfo& info);

  /**
   * Section D - Calculate functions
   */
  static Napi::Value TrueColor(const Napi::CallbackInfo& info);
  static Napi::Value TrueColorAlpha(const Napi::CallbackInfo& info);

  /**
   * Section E - Meta information
   */
  static Napi::Value GdVersionGetter(const Napi::CallbackInfo& info);
};

#endif
