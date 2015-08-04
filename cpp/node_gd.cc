/**
 * Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>
 * Copyright (c) 2014-2015, Vincent Bruijn <vebruijn@gmail.com>
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
#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include <string.h>
#include <assert.h>
#include <nan.h>
#include <sstream>

using namespace v8;
using namespace node;

#define SUPPORTS_GD_2_2_0 (GD_MINOR_VERSION == 2                        \
                  && GD_RELEASE_VERSION == 0)

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

#define SUPPORTS_UNTIL_GD_2_0_36 (GD_MINOR_VERSION == 0                  \
                  && GD_RELEASE_VERSION <= 36)

#define HAS_LIBTIFF (HAVE_LIBTIFF && SUPPORTS_GD_2_1_0)
#define HAS_LIBWEBP (HAVE_LIBWEBP && SUPPORTS_GD_2_1_0)

// Since gd 2.0.28, these are always built in
#define GD_GIF 1
#define GD_GIFANIM 1
#define GD_OPENPOLYGON 1

#define THROW_ERROR(TYPE, STR)                                          \
  return Nan::ThrowError(STR);

#define REQ_ARGS(N)                                                     \
  if (info.Length() < (N))                                              \
    return Nan::ThrowError("Expected " #N " arguments");

#define REQ_STR_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I]->IsString())                     \
    return Nan::ThrowError("Argument " #I " must be a string");         \
  String::Utf8Value VAR(info[I]->ToString());

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (info.Length() <= (I) || !info[I]->IsInt32())                      \
    return Nan::ThrowError("Argument " #I " must be an integer");       \
  VAR = info[I]->Int32Value();

#define REQ_FN_ARG(I, VAR)                                              \
  if (info.Length() <= (I) || !info[I]->IsFunction())                   \
    return Nan::ThrowError("Argument " #I " must be a function");       \
  Local<Function> VAR = info[I].As<Function>();

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (info.Length() <= (I) || !info[I]->IsNumber())                     \
    return Nan::ThrowError("Argument " #I " must be a number");         \
  VAR = info[I]->NumberValue();

#define REQ_EXT_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I]->IsExternal())                   \
    return Nan::ThrowError("Argument " #I " invalid");                  \
  Local<External> VAR = Local<External>::Cast(info[I]);

#define REQ_IMG_ARG(I, VAR)                                             \
  if (info.Length() <= (I) || !info[I]->IsObject())                     \
    return Nan::ThrowError("Argument " #I " must be an object");        \
  Local<Object> _obj_ = Local<Object>::Cast(info[I]);                   \
  Image *VAR = ObjectWrap::Unwrap<Image>(_obj_);

#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (info.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (info[I]->IsInt32()) {                                      \
    VAR = info[I]->Int32Value();                                        \
  } else {                                                              \
    return Nan::ThrowError("Argument " #I " must be an integer");       \
  }

#define RETURN_IMAGE(IMG)                                               \
  if (!IMG) info.GetReturnValue().SetNull();                            \
  Local<Value> _arg_ = Nan::New<External>(IMG);                         \
  Local<FunctionTemplate> func = Nan::New(Image::constructor);          \
  Local<Object> _image_ = func->GetFunction()->NewInstance(1, &_arg_);  \
  info.GetReturnValue().Set(_image_);

#define DECLARE_CREATE_FROM(TYPE)                                       \
  static NAN_METHOD(CreateFrom##TYPE) {                                 \
    REQ_STR_ARG(0, path);                                               \
    FILE *in; in = fopen(*path, "rb");                                  \
    gdImagePtr im = gdImageCreateFrom##TYPE(in);                        \
    fclose(in);                                                         \
    RETURN_IMAGE(im)                                                    \
  }                                                                     \
  static NAN_METHOD(CreateFrom##TYPE##Ptr) {                            \
    REQ_ARGS(1);                                                        \
    ASSERT_IS_STRING_OR_BUFFER(info[0]);                                \
    gdImagePtr im;                                                      \
    if(Buffer::HasInstance(info[0])) {                                  \
      Local<Value> obj = info[0]->ToObject();                           \
      char *buffer_data = Buffer::Data(obj);                            \
      size_t buffer_length = Buffer::Length(obj);                       \
      im = gdImageCreateFrom##TYPE##Ptr(buffer_length, buffer_data);    \
    } else {                                                            \
      ssize_t len = DecodeBytes(info[0], BINARY);                       \
      char* buf = new char[len];                                        \
      ssize_t written = DecodeWrite(buf, len, info[0]);                 \
      assert(written == len);                                           \
      im = gdImageCreateFrom##TYPE##Ptr(len, buf);                      \
      delete[] buf;                                                     \
    }                                                                   \
    RETURN_IMAGE(im)                                                    \
  }

#define ASSERT_IS_STRING_OR_BUFFER(val)                                 \
  if (!val->IsString() && !Buffer::HasInstance(val)) {                  \
    return Nan::ThrowError("Not a string or buffer");                   \
  }

#define RETURN_DATA()                                                   \
  Local<Value> result = Nan::Encode(data, size, Nan::BINARY);           \
  delete[] data;                                                        \
  info.GetReturnValue().Set(result);


#define COLOR_ANTIALIASED    gdAntiAliased
#define COLOR_BRUSHED        gdBrushed
#define COLOR_STYLED         gdStyled
#define COLOR_STYLEDBRUSHED  gdStyledBrushed
#define COLOR_TITLED         gdTiled
#define COLOR_TRANSPARENT    gdTransparent

class Gd : public node::ObjectWrap {
public:
  static void Init(Local<Object> exports) {
    NODE_DEFINE_CONSTANT(exports, COLOR_ANTIALIASED);
    NODE_DEFINE_CONSTANT(exports, COLOR_BRUSHED);
    NODE_DEFINE_CONSTANT(exports, COLOR_STYLED);
    NODE_DEFINE_CONSTANT(exports, COLOR_STYLEDBRUSHED);
    NODE_DEFINE_CONSTANT(exports, COLOR_TITLED);
    NODE_DEFINE_CONSTANT(exports, COLOR_TRANSPARENT);

#ifdef HAVE_LIBTIFF
    NODE_DEFINE_CONSTANT(exports, GD_TIFF);
#endif

#ifdef HAVE_LIBXPM
    NODE_DEFINE_CONSTANT(exports, GD_XPM);
#endif

#ifdef HAVE_LIBJPEG
    NODE_DEFINE_CONSTANT(exports, GD_JPEG);
#endif

#ifdef HAVE_LIBFONTCONFIG
    NODE_DEFINE_CONSTANT(exports, GD_FONTCONFIG);
#endif

#ifdef HAVE_LIBFREETYPE
    NODE_DEFINE_CONSTANT(exports, GD_FREETYPE);
#endif

#ifdef HAVE_LIBPNG
    NODE_DEFINE_CONSTANT(exports, GD_PNG);
#endif

#ifdef HAVE_LIBWEBP
    NODE_DEFINE_CONSTANT(exports, GD_WEBP);
#endif

#ifdef HAVE_LIBVPX
    NODE_DEFINE_CONSTANT(exports, GD_VPX);
#endif

    NODE_DEFINE_CONSTANT(exports, GD_GIF);
    NODE_DEFINE_CONSTANT(exports, GD_GIFANIM);
    NODE_DEFINE_CONSTANT(exports, GD_OPENPOLYGON);

    Nan::SetMethod(exports, "create", ImageCreate);
    Nan::SetMethod(exports, "createTrueColor", ImageCreateTrueColor);
    Nan::SetMethod(exports, "createFromJpeg", CreateFromJpeg);
    Nan::SetMethod(exports, "createFromJpegPtr", CreateFromJpegPtr);
    Nan::SetMethod(exports, "createFromPng", CreateFromPng);
    Nan::SetMethod(exports, "createFromPngPtr", CreateFromPngPtr);
    Nan::SetMethod(exports, "createFromGif", CreateFromGif);
    Nan::SetMethod(exports, "createFromGifPtr", CreateFromGifPtr);
    Nan::SetMethod(exports, "createFromGd2", CreateFromGd2);
    Nan::SetMethod(exports, "createFromGd2Ptr", CreateFromGd2Ptr);
    Nan::SetMethod(exports, "createFromGd2Part", CreateFromGd2Part);
    Nan::SetMethod(exports, "createFromGd2PartPtr", CreateFromGd2PartPtr);
    Nan::SetMethod(exports, "createFromWBMP", CreateFromWBMP);
    Nan::SetMethod(exports, "createFromWBMPPtr", CreateFromWBMPPtr);
#if HAS_LIBWEBP
    Nan::SetMethod(exports, "createFromWebp", CreateFromWebp);
    Nan::SetMethod(exports, "createFromWebpPtr", CreateFromWebpPtr);
#endif

#if SUPPORTS_GD_2_1_0
    Nan::SetMethod(exports, "createFromBmp", CreateFromBmp);
    Nan::SetMethod(exports, "createFromBmpPtr", CreateFromBmpPtr);
    // Nan::SetMethod(exports, "createFromTiff", CreateFromTiff);
    // Nan::SetMethod(exports, "createFromTiffPtr", CreateFromTiffPtr);
#endif

#if SUPPORTS_GD_2_1_1
    Nan::SetMethod(exports, "createFromFile", CreateFromFile);
#endif
    Nan::SetMethod(exports, "trueColor", TrueColor);
    Nan::SetMethod(exports, "trueColorAlpha", TrueColorAlpha);
    Nan::SetMethod(exports, "getGDVersion", GdVersionGetter);

    Image::Init(exports);
  }

private:
  static NAN_METHOD(ImageCreate) {
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreate(width, height);

    RETURN_IMAGE(im);
  }

  static NAN_METHOD(ImageCreateTrueColor) {
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreateTrueColor(width, height);

    RETURN_IMAGE(im);
  }

  DECLARE_CREATE_FROM(Jpeg)
  DECLARE_CREATE_FROM(Png)
  DECLARE_CREATE_FROM(Gif)
  DECLARE_CREATE_FROM(Gd2)
  DECLARE_CREATE_FROM(WBMP)
#if HAS_LIBWEBP
  DECLARE_CREATE_FROM(Webp)
#endif

#if SUPPORTS_GD_2_1_0
  DECLARE_CREATE_FROM(Bmp);
#endif
  // libgd appears to open tiff's buggy...
  // DECLARE_CREATE_FROM(Tiff)

#if SUPPORTS_GD_2_1_1
  static NAN_METHOD(CreateFromFile) {
    REQ_STR_ARG(0, path);

    gdImagePtr im = gdImageCreateFromFile(*path);

    RETURN_IMAGE(im);
  }
#endif

  static NAN_METHOD(CreateFromGd2Part) {
    REQ_STR_ARG(0, path);
    REQ_INT_ARG(1, srcX);
    REQ_INT_ARG(2, srcY);
    REQ_INT_ARG(3, width);
    REQ_INT_ARG(4, height);

    FILE *in;
    in = fopen(*path, "rb");
    gdImagePtr im = gdImageCreateFromGd2Part(in, srcX, srcY, width, height);
    fclose(in);

    RETURN_IMAGE(im);
  }

  static NAN_METHOD(CreateFromGd2PartPtr) {
    REQ_ARGS(5);
    REQ_INT_ARG(1, srcX);
    REQ_INT_ARG(2, srcY);
    REQ_INT_ARG(3, width);
    REQ_INT_ARG(4, height);

    ssize_t len = DecodeBytes(info[0]);
    char *buf   = new char[len];
    ssize_t written = DecodeWrite(buf, len, info[0]);
    assert(written == len);

    gdImagePtr im = gdImageCreateFromGd2PartPtr(len, buf, srcX, srcY, width, height);
    delete[] buf;

    RETURN_IMAGE(im);
  }

  static NAN_METHOD(TrueColor) {
    REQ_ARGS(3);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);

    uint32_t result = Nan::New<Uint32>(gdTrueColor(r, g, b))->Uint32Value();

    info.GetReturnValue().Set(result);
  }

  static NAN_METHOD(TrueColorAlpha) {
    REQ_ARGS(4);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);
    REQ_INT_ARG(3, a);

    uint32_t result = Nan::New<Uint32>(gdTrueColorAlpha(r, g, b, a))->Uint32Value();

    info.GetReturnValue().Set(result);
  }

  static NAN_METHOD(GdVersionGetter) {
    std::stringstream version_string;
    version_string << GD_MAJOR_VERSION << "." << GD_MINOR_VERSION << "." << GD_RELEASE_VERSION;
    v8::Local<String> s = Nan::New<String>(version_string.str()).ToLocalChecked();
    info.GetReturnValue().Set(s);
  }


  class Image : public node::ObjectWrap {
  public:
    static Nan::Persistent<FunctionTemplate> constructor;

    static void Init(v8::Local<Object> exports) {
      Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(New);
      t->SetClassName(Nan::New<String>("Image").ToLocalChecked());
      t->InstanceTemplate()->SetInternalFieldCount(1);

      Nan::SetPrototypeMethod(t, "jpeg", Jpeg);
      Nan::SetPrototypeMethod(t, "jpegPtr", JpegPtr);
      Nan::SetPrototypeMethod(t, "png", Png);
      Nan::SetPrototypeMethod(t, "pngPtr", PngPtr);
      Nan::SetPrototypeMethod(t, "gif", Gif);
      Nan::SetPrototypeMethod(t, "gifPtr", GifPtr);
      Nan::SetPrototypeMethod(t, "gd", Gd);
      Nan::SetPrototypeMethod(t, "gdPtr", GdPtr);
      Nan::SetPrototypeMethod(t, "gd2", Gd2);
      Nan::SetPrototypeMethod(t, "gd2Ptr", Gd2Ptr);
      Nan::SetPrototypeMethod(t, "wbmp", WBMP);
      Nan::SetPrototypeMethod(t, "wbmpPtr", WBMPPtr);
#if HAS_LIBWEBP
      Nan::SetPrototypeMethod(t, "webp", Webp);
      Nan::SetPrototypeMethod(t, "webpPtr", WebpPtr);
#endif

#if SUPPORTS_GD_2_1_0
      Nan::SetPrototypeMethod(t, "bmp", Bmp);
      Nan::SetPrototypeMethod(t, "bmpPtr", BmpPtr);
#endif

#if HAS_LIBTIFF
      Nan::SetPrototypeMethod(t, "tiff", Tiff);
      Nan::SetPrototypeMethod(t, "tiffPtr", TiffPtr);
#endif

#if SUPPORTS_GD_2_1_1
      Nan::SetPrototypeMethod(t, "file", File);
      Nan::SetPrototypeMethod(t, "fileCallback", FileCallback);
#endif

      Nan::SetPrototypeMethod(t, "destroy", Destroy);

      /**
       * Drawing Functions
       */
      Nan::SetPrototypeMethod(t, "setPixel", SetPixel);
      Nan::SetPrototypeMethod(t, "line", Line);
      Nan::SetPrototypeMethod(t, "dashedLine", DashedLine);
      Nan::SetPrototypeMethod(t, "polygon", Polygon);
      Nan::SetPrototypeMethod(t, "openPolygon", OpenPolygon);
      Nan::SetPrototypeMethod(t, "filledPolygon", FilledPolygon);
      Nan::SetPrototypeMethod(t, "rectangle", Rectangle);
      Nan::SetPrototypeMethod(t, "filledRectangle", FilledRectangle);
      Nan::SetPrototypeMethod(t, "arc", Arc);
      Nan::SetPrototypeMethod(t, "filledArc", FilledArc);
#if SUPPORTS_GD_2_1_0
      Nan::SetPrototypeMethod(t, "ellipse", Ellipse);
#endif
      Nan::SetPrototypeMethod(t, "filledEllipse", FilledEllipse);
      Nan::SetPrototypeMethod(t, "fillToBorder", FillToBorder);
      Nan::SetPrototypeMethod(t, "fill", Fill);
      Nan::SetPrototypeMethod(t, "setAntiAliased", SetAntiAliased);
      Nan::SetPrototypeMethod(t, "setAntiAliasedDontBlend", SetAntiAliasedDontBlend);
      Nan::SetPrototypeMethod(t, "setBrush", SetBrush);
      Nan::SetPrototypeMethod(t, "setTile", SetTile);
      Nan::SetPrototypeMethod(t, "setStyle", SetStyle);
      Nan::SetPrototypeMethod(t, "setThickness", SetThickness);
      Nan::SetPrototypeMethod(t, "alphaBlending", AlphaBlending);
      Nan::SetPrototypeMethod(t, "saveAlpha", SaveAlpha);
      Nan::SetPrototypeMethod(t, "setClip", SetClip);
      Nan::SetPrototypeMethod(t, "getClip", GetClip);
#if SUPPORTS_GD_2_1_0
      Nan::SetPrototypeMethod(t, "setResolution", SetResolution);
#endif

      /**
       * Query Functions
       */
      Nan::SetPrototypeMethod(t, "Alpha", Alpha);
      Nan::SetPrototypeMethod(t, "getPixel", GetPixel);
      Nan::SetPrototypeMethod(t, "getTrueColorPixel", GetTrueColorPixel);
      Nan::SetPrototypeMethod(t, "imageColorAt", ImageColorAt);
      Nan::SetPrototypeMethod(t, "getBoundsSafe", GetBoundsSafe);
      // trueColor
      Nan::SetAccessor(t->InstanceTemplate(), Nan::New("trueColor").ToLocalChecked(),
        TrueColorGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      // width, height
      Nan::SetAccessor(t->InstanceTemplate(), Nan::New("width").ToLocalChecked(),
        WidthGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      Nan::SetAccessor(t->InstanceTemplate(), Nan::New("height").ToLocalChecked(),
        HeightGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

      /**
       * Font and Text Handling Functions
       */
      Nan::SetPrototypeMethod(t, "stringFTBBox", StringFTBBox);
      Nan::SetPrototypeMethod(t, "stringFT", StringFT);

      /**
       * Color Handling Functions
       */
      Nan::SetPrototypeMethod(t, "colorAllocate", ColorAllocate);
      Nan::SetPrototypeMethod(t, "colorAllocateAlpha", ColorAllocateAlpha);
      Nan::SetPrototypeMethod(t, "colorClosest", ColorClosest);
      Nan::SetPrototypeMethod(t, "colorClosestAlpha", ColorClosestAlpha);
      Nan::SetPrototypeMethod(t, "colorClosestHWB", ColorClosestHWB);
      Nan::SetPrototypeMethod(t, "colorExact", ColorExact);
      Nan::SetPrototypeMethod(t, "colorResolve", ColorResolve);
      Nan::SetPrototypeMethod(t, "colorResolveAlpha", ColorResolveAlpha);
      Nan::SetPrototypeMethod(t, "red", Red);
      Nan::SetPrototypeMethod(t, "green", Green);
      Nan::SetPrototypeMethod(t, "blue", Blue);
      Nan::SetPrototypeMethod(t, "getTransparent", GetTransparent);
      Nan::SetPrototypeMethod(t, "colorDeallocate", ColorDeallocate);

      /**
       * Color Manipulation Functions
       */
      Nan::SetPrototypeMethod(t, "colorTransparent", ColorTransparent);
#if SUPPORTS_GD_2_1_0
      Nan::SetPrototypeMethod(t, "colorReplace", ColorReplace);
      Nan::SetPrototypeMethod(t, "colorReplaceThreshold", ColorReplaceThreshold);
      Nan::SetPrototypeMethod(t, "colorReplaceArray", ColorReplaceArray);

      /**
       * Effects
       */
      Nan::SetPrototypeMethod(t, "grayscale", GrayScale);
      Nan::SetPrototypeMethod(t, "gaussianBlur", GaussianBlur);
      Nan::SetPrototypeMethod(t, "negate", Negate);
      Nan::SetPrototypeMethod(t, "brightness", Brightness);
      Nan::SetPrototypeMethod(t, "contrast", Contrast);
      Nan::SetPrototypeMethod(t, "selectiveBlur", SelectiveBlur);
      Nan::SetPrototypeMethod(t, "flipHorizontal", FlipHorizontal);
      Nan::SetPrototypeMethod(t, "flipVertical", FlipVertical);
      Nan::SetPrototypeMethod(t, "flipBoth", FlipBoth);
      Nan::SetPrototypeMethod(t, "crop", Crop);
      Nan::SetPrototypeMethod(t, "cropAuto", CropAuto);
      Nan::SetPrototypeMethod(t, "cropThreshold", CropThreshold);
#endif

#if SUPPORTS_GD_2_1_1
      Nan::SetPrototypeMethod(t, "emboss", Emboss);
#endif
      Nan::SetPrototypeMethod(t, "sharpen", Sharpen);
      Nan::SetPrototypeMethod(t, "createPaletteFromTrueColor", CreatePaletteFromTrueColor);
      Nan::SetPrototypeMethod(t, "trueColorToPalette", TrueColorToPalette);
#if SUPPORTS_GD_2_1_0
      Nan::SetPrototypeMethod(t, "paletteToTrueColor", PaletteToTrueColor);
      Nan::SetPrototypeMethod(t, "colorMatch", ColorMatch);
#endif
      Nan::SetPrototypeMethod(t, "gifAnimBegin", GifAnimBegin);
      Nan::SetPrototypeMethod(t, "gifAnimAdd", GifAnimAdd);
      Nan::SetPrototypeMethod(t, "gifAnimEnd", GifAnimEnd);

      // interlace
      Nan::SetAccessor(t->InstanceTemplate(), Nan::New("interlace").ToLocalChecked(),
        InterlaceGetter, InterlaceSetter, v8::Local<v8::Value>(), PROHIBITS_OVERWRITING);
      Nan::SetAccessor(t->InstanceTemplate(), Nan::New("colorsTotal").ToLocalChecked(),
        ColorsTotalGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

      /**
       * Copying and Resizing Functions
       */
      Nan::SetPrototypeMethod(t, "copy", Copy);
      Nan::SetPrototypeMethod(t, "copyResized", CopyResized);
      Nan::SetPrototypeMethod(t, "copyResampled", CopyResampled);
      Nan::SetPrototypeMethod(t, "copyRotated", CopyRotated);
      Nan::SetPrototypeMethod(t, "copyMerge", CopyMerge);
      Nan::SetPrototypeMethod(t, "copyMergeGray", CopyMergeGray);
      Nan::SetPrototypeMethod(t, "paletteCopy", PaletteCopy);
      Nan::SetPrototypeMethod(t, "squareToCircle", SquareToCircle);

      /**
       * Miscellaneous Functions
       */
      Nan::SetPrototypeMethod(t, "compare", Compare);

      constructor.Reset(t);
      exports->Set(Nan::New<String>("Image").ToLocalChecked(), t->GetFunction());
    }

    static NAN_METHOD(New) {
      REQ_EXT_ARG(0, image);

      gdImagePtr imgPtr = (gdImagePtr)image->Value();
      (new Image(imgPtr))->Wrap(info.This());

      info.GetReturnValue().Set(info.This());
    }
  protected:
    Image(gdImagePtr image) : _image(image) {}
    ~Image() { if(_image) gdImageDestroy(_image);  }

    gdImagePtr _image;

    operator gdImagePtr () const { return _image; }

    /**
     * Destruction, Loading and Saving Functions
     */
    static NAN_METHOD(Destroy) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());
      if(im->_image){
        gdImageDestroy(*im);
        im->_image = NULL;
      }

      info.GetReturnValue().SetUndefined();
    }

    static NAN_METHOD(Jpeg) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, quality, -1);

      FILE *out = fopen(*path, "wb");
      gdImageJpeg(*im, out, quality);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(JpegPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, quality, -1);

      int size;
      char *data = (char*)gdImageJpegPtr(*im, &size, quality);

      RETURN_DATA()
    }

    static NAN_METHOD(Gif) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGif(*im, out);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(GifPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      int size;
      char *data = (char*)gdImageGifPtr(*im, &size);

      RETURN_DATA()
    }

    static NAN_METHOD(Png) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      FILE *out = fopen(*path, "wb");
      gdImagePngEx(*im, out, level);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(PngPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, level, -1);

      int size;
      char *data = (char*)gdImagePngPtrEx(*im, &size, level);

      RETURN_DATA()
    }

    static NAN_METHOD(WBMP) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      FILE *out = fopen(*path, "wb");
      gdImageWBMP(*im, foreground, out);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(WBMPPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, foreground);

      int size;
      char *data = (char*)gdImageWBMPPtr(*im, &size, foreground);

      RETURN_DATA()
    }

#if HAS_LIBWEBP
    static NAN_METHOD(Webp) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      FILE *out = fopen(*path, "wb");
      gdImageWebpEx(*im, out, level);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(WebpPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, level, -1);

      int size;
      char *data = (char*)gdImageWebpPtrEx(*im, &size, level);

      RETURN_DATA()
    }
#endif

    static NAN_METHOD(Gd) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGd(*im, out);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(GdPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      int size;
      char *data = (char*)gdImageGdPtr(*im, &size);

      RETURN_DATA()
    }

    static NAN_METHOD(Gd2) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, chunkSize);
      OPT_INT_ARG(2, format, GD2_FMT_RAW);

      FILE *out = fopen(*path, "wb");
      gdImageGd2(*im, out, chunkSize, format);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Gd2Ptr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, chunkSize);
      OPT_INT_ARG(1, format, GD2_FMT_RAW);

      int size;
      char *data = (char*)gdImageGd2Ptr(*im, chunkSize, format, &size);

      RETURN_DATA()
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(Bmp) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, compression);

      FILE *out = fopen(*path, "wb");
      gdImageBmp(*im, out, compression);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(BmpPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, compression, 0);

      int size;
      char *data = (char*)gdImageBmpPtr(*im, &size, compression);

      RETURN_DATA()
    }
#endif

#if HAS_LIBTIFF
    static NAN_METHOD(Tiff) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageTiff(*im, out);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(TiffPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      int size;
      char *data = (char*)gdImageTiffPtr(*im, &size);

      RETURN_DATA()
    }
#endif

#if SUPPORTS_GD_2_1_1
    static NAN_METHOD(File) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);

      gdImageFile(*im, *path);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FileCallback) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_STR_ARG(0, path);
      REQ_FN_ARG(1, cb);

      Nan::Callback callback(cb);

      bool result = gdImageFile(*im, *path);
      if (result == false) {
        Local<Value> argv[] = {
          Nan::Error(Nan::New<String>("Unable to write file.").ToLocalChecked())
        };
        callback.Call(1, argv);
      } else {
        Local<Value> argv[] = {
          Nan::Null()
        };
        callback.Call(1, argv);
      }

      // delete callback;
      info.GetReturnValue().Set(info.This());
    }
#endif

    /**
     * Drawing Functions
     */
    static NAN_METHOD(SetPixel) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageSetPixel(*im, x, y, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Line) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageLine(*im, x1, y1, x2, y2, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(DashedLine) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageDashedLine(*im, x1, y1, x2, y2, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Polygon) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!info[0]->IsArray()) {
        return Nan::ThrowError("Arguments 0 must be an array");
      }

      Local<String> x = Nan::New("x").ToLocalChecked();
      Local<String> y = Nan::New("y").ToLocalChecked();

      Local<Array> array = Local<Array>::Cast(info[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Nan::New<Integer>(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImagePolygon(*im, points, _len, color);

      delete[] points;

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(OpenPolygon) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!info[0]->IsArray()) {
        return Nan::ThrowError("Arguments 0 must be an array");
      }

      Local<String> x = Nan::New("x").ToLocalChecked();
      Local<String> y = Nan::New("y").ToLocalChecked();

      Local<Array> array = Local<Array>::Cast(info[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Nan::New<Integer>(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImageOpenPolygon(*im, points, _len, color);

      delete[] points;

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FilledPolygon) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!info[0]->IsArray()) {
        return Nan::ThrowError("Arguments 0 must be an array");
      }

      Local<String> x = Nan::New("x").ToLocalChecked();
      Local<String> y = Nan::New("y").ToLocalChecked();

      Local<Array> array = Local<Array>::Cast(info[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Nan::New<Integer>(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImageFilledPolygon(*im, points, _len, color);

      delete[] points;

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Rectangle) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageRectangle(*im, x1, y1, x2, y2, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FilledRectangle) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageFilledRectangle(*im, x1, y1, x2, y2, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Arc) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(7);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, begin);
      REQ_INT_ARG(5, end);
      REQ_INT_ARG(6, color);

      gdImageArc(*im, cx, cy, width, height, begin, end, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FilledArc) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(8);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, begin);
      REQ_INT_ARG(5, end);
      REQ_INT_ARG(6, color);
      REQ_INT_ARG(7, style);

      gdImageFilledArc(*im, cx, cy, width, height, begin, end, color, style);

      info.GetReturnValue().Set(info.This());
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(Ellipse) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, color);

      gdImageEllipse(*im, cx, cy, width, height, color);

      info.GetReturnValue().Set(info.This());
    }
#endif

    static NAN_METHOD(FilledEllipse) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, color);

      gdImageFilledEllipse(*im, cx, cy, width, height, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FillToBorder) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(4);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, border);
      REQ_INT_ARG(3, color);

      gdImageFillToBorder(*im, x, y, border, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Fill) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageFill(*im, x, y, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetAntiAliased) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);

      gdImageSetAntiAliased(*im, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetAntiAliasedDontBlend) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);
      REQ_INT_ARG(1, dont_blend); // what does this mean?

      gdImageSetAntiAliasedDontBlend(*im, color, dont_blend);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetBrush) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_IMG_ARG(0, brush)
      gdImageSetBrush(*im, *brush);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetTile) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_IMG_ARG(0, tile)
      gdImageSetTile(*im, *tile);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetStyle) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      if (info.Length() < 1 || !info[0]->IsArray())
        return Nan::ThrowError("Arguments 0 must be an array");

      Local<Array> array = Local<Array>::Cast(info[0]);
      int len  = array->Length(), _len = 0;
      int *sty =  new int[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Nan::New<Integer>(i));
        if (!v->IsNumber()) continue;

        sty[i] = v->Int32Value();
        _len++;
      }

      gdImageSetStyle(*im, sty, _len);

      delete[] sty;

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetThickness) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, thickness)

      gdImageSetThickness(*im, thickness);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(AlphaBlending) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, blending)
      gdImageAlphaBlending(*im, blending);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SaveAlpha) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, saveFlag)
      gdImageSaveAlpha(*im, saveFlag);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SetClip) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(4);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);

      gdImageSetClip(*im, x1, y1, x2, y2);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(GetClip) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      int x1, y1, x2, y2;
      gdImageGetClip(*im, &x1, &y1, &x2, &y2);

      Local<Object> result = Nan::New<Object>();
      result->Set(Nan::New<String>("x1").ToLocalChecked(), Nan::New<Integer>(x1));
      result->Set(Nan::New<String>("y1").ToLocalChecked(), Nan::New<Integer>(y1));
      result->Set(Nan::New<String>("x2").ToLocalChecked(), Nan::New<Integer>(x2));
      result->Set(Nan::New<String>("y2").ToLocalChecked(), Nan::New<Integer>(y2));

      info.GetReturnValue().Set(result);
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(SetResolution) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, res_x);
      REQ_INT_ARG(1, res_y);

      gdImageSetResolution(*im, res_x, res_y);

      info.GetReturnValue().Set(info.This());
    }
#endif

    /**
     * Query Functions
     */
    static NAN_METHOD(Alpha) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Nan::New<Integer>(gdImageAlpha(im->operator gdImagePtr(), color));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(GetPixel) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = Nan::New<Integer>(gdImageGetPixel(*im, x, y));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(GetTrueColorPixel) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = Nan::New<Integer>(gdImageGetTrueColorPixel(*im, x, y));
      info.GetReturnValue().Set(result);
    }

    // This is implementation of the PHP-GD specific method imagecolorat
    static NAN_METHOD(ImageColorAt) {
      Image *img = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result;
      gdImagePtr im = *img;
      if (gdImageTrueColor(im)) {
        if(im->tpixels && gdImageBoundsSafe(im, x, y)){
          result = Nan::New<Integer>(gdImageTrueColorPixel(im, x, y));
        } else {
          return Nan::ThrowError("[imageColorAt] Invalid pixel");
        }
      } else {
        if (im->pixels && gdImageBoundsSafe(im, x, y)) {
          result = Nan::New<Integer>(im->pixels[y][x]);
        } else {
          return Nan::ThrowError("[imageColorAt] Invalid pixel");
        }
      }
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(GetBoundsSafe) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = Nan::New<Integer>(gdImageBoundsSafe(*im, x, y));
      info.GetReturnValue().Set(result);
    }

    static NAN_PROPERTY_GETTER(WidthGetter) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImageSX(im->operator gdImagePtr()));
      info.GetReturnValue().Set(result);
    }

    static NAN_PROPERTY_GETTER(HeightGetter) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImageSY(im->operator gdImagePtr()));
      info.GetReturnValue().Set(result);
    }

    static NAN_PROPERTY_GETTER(TrueColorGetter) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImageTrueColor(im->operator gdImagePtr()));
      info.GetReturnValue().Set(result);
    }

    /**
     * Font and Text Handling Funcitons
     */
    static NAN_METHOD(StringFTBBox) {
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


      err = gdImageStringFT(NULL, &brect[0], color, *font, size, angle, x, y, *str);
      if (err) return Nan::ThrowError(err);

      Local<Array> result = Nan::New<Array>();

      for (int i = 0; i < 8; i++) {
        result->Set(Nan::New<Integer>(i), Nan::New<Number>(brect[i]));
      }

      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(StringFT) {
      REQ_ARGS(7);
      REQ_INT_ARG(0, color);
      REQ_STR_ARG(1, font);
      REQ_DOUBLE_ARG(2, size);
      REQ_DOUBLE_ARG(3, angle);
      REQ_INT_ARG(4, x);
      REQ_INT_ARG(5, y);
      REQ_STR_ARG(6, str);

      /* return rectangle? */
      bool return_rectangle = false;
      if (info.Length() > 7 && info[7]->IsBoolean()) {
        return_rectangle = info[7]->BooleanValue();
      }

      int brect[8];
      char *err;


      if (return_rectangle) {
        err = gdImageStringFT(NULL, &brect[0], color, *font, size, angle, x, y, *str);
        if (err) return Nan::ThrowError(err);

        Local<Array> result = Nan::New<Array>();
        for(int i=0; i < 8; i++) {
          result->Set( Nan::New<Integer>(i), Nan::New<Number>(brect[i]) );
        }

        info.GetReturnValue().Set(result);
      }

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      err = gdImageStringFT(*im, &brect[0], color, *font, size, angle, x, y, *str);
      if (err) return Nan::ThrowError(err);

      info.GetReturnValue().Set(info.This());
    }

    /**
     * Color Handling Functions
     */
    static NAN_METHOD(ColorAllocate) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(3);
      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Nan::New<Integer>(gdImageColorAllocate(*im, r, g, b));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorAllocateAlpha) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = Nan::New<Integer>(gdImageColorAllocateAlpha(*im, r, g, b, a));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorClosest) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Nan::New<Integer>(gdImageColorClosest(*im, r, g, b));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorClosestAlpha) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = Nan::New<Integer>(gdImageColorClosestAlpha(*im, r, g, b, a));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorClosestHWB) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Nan::New<Integer>(gdImageColorClosestHWB(*im, r, g, b));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorExact) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Nan::New<Integer>(gdImageColorExact(*im, r, g, b));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorResolve) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Nan::New<Integer>(gdImageColorResolve(*im, r, g, b));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorResolveAlpha) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = Nan::New<Integer>(gdImageColorResolveAlpha(*im, r, g, b, a));
      info.GetReturnValue().Set(result);
    }

    static NAN_PROPERTY_GETTER(ColorsTotalGetter) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImageColorsTotal(im->operator gdImagePtr()));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(Red) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Nan::New<Integer>(gdImageRed(im->operator gdImagePtr(), color));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(Blue) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Nan::New<Integer>(gdImageBlue(im->operator gdImagePtr(), color));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(Green) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Nan::New<Integer>(gdImageGreen(im->operator gdImagePtr(), color));
      info.GetReturnValue().Set(result);
    }

    static NAN_PROPERTY_GETTER(InterlaceGetter) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      bool interlaced = (gdImageGetInterlaced(im->operator gdImagePtr()) != 0);
      Local<Boolean> result;
      if (interlaced) {
        result = Nan::True();
      } else {
        result = Nan::False();
      }

      info.GetReturnValue().Set(result);
    }

    static NAN_SETTER(InterlaceSetter) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      if (value->IsBoolean()) {
        bool interlace = value->BooleanValue();

        gdImageInterlace(*im, interlace?1:0);
      }
    }

    static NAN_METHOD(GetTransparent) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImageGetTransparent(im->operator gdImagePtr()));
      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorDeallocate) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);
      gdImageColorDeallocate(*im, color);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(ColorTransparent) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);
      gdImageColorTransparent(*im, color);

      info.GetReturnValue().Set(info.This());
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(ColorReplace) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, fromColor);
      REQ_INT_ARG(1, toColor);

      Local<Number> result = Nan::New<Integer>(gdImageColorReplace(*im, fromColor, toColor));

      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorReplaceThreshold) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, fromColor);
      REQ_INT_ARG(1, toColor);
      REQ_DOUBLE_ARG(2, threshold);

      Local<Number> result =
        Nan::New<Integer>(gdImageColorReplaceThreshold(*im, fromColor, toColor, threshold));

      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorReplaceArray) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(2);

      Local<Array> fromArray = Local<Array>::Cast(info[0]);
      int flen = fromArray->Length(), _flen = 0;
      int *fromColors =  new int[flen];

      for(int i = 0; i < flen; i++) {
        Local<Value> v = fromArray->Get(Nan::New<Integer>(i));
        fromColors[i] = v->Int32Value();
        _flen++;
      }

      Local<Array> toArray = Local<Array>::Cast(info[1]);
      int tlen = toArray->Length(), _tlen = 0;
      int *toColors =  new int[tlen];

      for(int j = 0; j < tlen; j++) {
        Local<Value> v = toArray->Get(Nan::New<Integer>(j));
        toColors[j] = v->Int32Value();
        _tlen++;
      }

      if (_flen != _tlen) {
        return Nan::ThrowError("Color arrays should have same length.");
      }

      Local<Number> result =
        Nan::New<Integer>(gdImageColorReplaceArray(*im, _flen, fromColors, toColors));

      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(GrayScale) {

      Image *im = ObjectWrap::Unwrap<Image>(info.This());
      gdImageGrayScale(*im);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(GaussianBlur) {

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageGaussianBlur(*im);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Negate) {

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageNegate(*im);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Brightness) {
      REQ_INT_ARG(0, brightness);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageBrightness(*im, brightness);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Contrast) {
      REQ_DOUBLE_ARG(0, contrast);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageContrast(*im, contrast);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SelectiveBlur) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageSelectiveBlur(*im);
      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FlipHorizontal) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageFlipHorizontal(*im);
      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FlipVertical) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageFlipVertical(*im);
      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(FlipBoth) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageFlipBoth(*im);
      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Crop) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);

      gdRect *rect = new gdRect;

      rect->x = x;
      rect->y = y;
      rect->width = (width == 0) ? 100 : width;
      rect->height = (height == 0) ? 100 : height;

      gdImagePtr newImage = gdImageCrop(*im, rect);

      RETURN_IMAGE(newImage);
    }

    static NAN_METHOD(CropAuto) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, mode);

      if (mode > 4) {
        return Nan::ThrowError("Crop mode should be between 0 and 5");
      }

      gdImagePtr newImage = gdImageCropAuto(*im, mode);

      RETURN_IMAGE(newImage);
    }

    static NAN_METHOD(CropThreshold) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_INT_ARG(0, color);
      REQ_DOUBLE_ARG(1, threshold);

      gdImagePtr newImage = gdImageCropThreshold(*im, color, threshold);
      RETURN_IMAGE(newImage);
    }
#endif

#if SUPPORTS_GD_2_1_1
    static NAN_METHOD(Emboss) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageEmboss(*im);
      info.GetReturnValue().Set(info.This());
    }
#endif

    /**
     * Copying and Resizing Functions
     */
    static NAN_METHOD(Copy) {
      REQ_ARGS(7);
      REQ_IMG_ARG(0, dest);
      REQ_INT_ARG(1, dstX);
      REQ_INT_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, width);
      REQ_INT_ARG(6, height);

      Image *im   = ObjectWrap::Unwrap<Image>(info.This());

      gdImageCopy(*dest, *im, dstX, dstY, srcX, srcY, width, height);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(CopyResized) {
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

      Image *im   = ObjectWrap::Unwrap<Image>(info.This());

      gdImageCopyResized(*dest, *im, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(CopyResampled) {
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

      Image *im   = ObjectWrap::Unwrap<Image>(info.This());

      gdImageCopyResampled(*dest, *im, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(CopyRotated) {
      REQ_ARGS(8);
      REQ_IMG_ARG(0, dest);
      REQ_DOUBLE_ARG(1, dstX);
      REQ_DOUBLE_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, srcW);
      REQ_INT_ARG(6, srcH);
      REQ_INT_ARG(7, angle);

      Image *im   = ObjectWrap::Unwrap<Image>(info.This());

      gdImageCopyRotated(*dest, *im, dstX, dstY, srcX, srcY, srcW, srcH, angle);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(CopyMerge) {
      REQ_ARGS(8);
      REQ_IMG_ARG(0, dest);
      REQ_INT_ARG(1, dstX);
      REQ_INT_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, width);
      REQ_INT_ARG(6, height);
      REQ_INT_ARG(7, pct);

      Image *im   = ObjectWrap::Unwrap<Image>(info.This());

      gdImageCopyMerge(*dest, *im, dstX, dstY, srcX, srcY, width, height, pct);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(CopyMergeGray) {
      REQ_ARGS(8);
      REQ_IMG_ARG(0, dest);
      REQ_INT_ARG(1, dstX);
      REQ_INT_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, width);
      REQ_INT_ARG(6, height);
      REQ_INT_ARG(7, pct);

      Image *im   = ObjectWrap::Unwrap<Image>(info.This());

      gdImageCopyMergeGray(*dest, *im, dstX, dstY, srcX, srcY, width, height, pct);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(PaletteCopy) {
      REQ_IMG_ARG(0, dest);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImagePaletteCopy(*dest, *im);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(SquareToCircle) {
      REQ_INT_ARG(0, radius);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageSquareToCircle(*im, radius);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(Sharpen) {
      REQ_INT_ARG(0, pct);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageSharpen(*im, pct);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(CreatePaletteFromTrueColor) {
      OPT_INT_ARG(0, ditherFlag, 0);
      OPT_INT_ARG(1, colorsWanted, 256);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImagePtr newImage = gdImageCreatePaletteFromTrueColor(*im, ditherFlag, colorsWanted);

      RETURN_IMAGE(newImage);
    }

    static NAN_METHOD(TrueColorToPalette) {
      OPT_INT_ARG(0, ditherFlag, 0);
      OPT_INT_ARG(1, colorsWanted, 256);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

#if SUPPORTS_GD_2_1_0
      Local<Number> result = Nan::New<Integer>(gdImageTrueColorToPalette(*im, ditherFlag, colorsWanted));
      info.GetReturnValue().Set(result);
#endif
#if SUPPORTS_UNTIL_GD_2_0_36
      gdImageTrueColorToPalette(*im, ditherFlag, colorsWanted);
      info.GetReturnValue().SetUndefined();
#endif
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(PaletteToTrueColor) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImagePaletteToTrueColor(*im));

      info.GetReturnValue().Set(result);
    }

    static NAN_METHOD(ColorMatch) {
      REQ_IMG_ARG(0, palette);

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Nan::New<Integer>(gdImageColorMatch(*im, *palette));

      info.GetReturnValue().Set(result);
    }
#endif

    static NAN_METHOD(GifAnimBegin) {
      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, GlobalCM);
      REQ_INT_ARG(2, Loops);

      FILE *out = fopen(*path, "wb");

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      gdImageGifAnimBegin(*im, out, GlobalCM, Loops);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(GifAnimAdd) {
      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, LocalCM);
      REQ_INT_ARG(2, LeftOfs);
      REQ_INT_ARG(3, TopOfs);
      REQ_INT_ARG(4, Delay);
      REQ_INT_ARG(5, Disposal);

      FILE *out = fopen(*path, "ab");

      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      if (info.Length() <= 6) {
        return Nan::ThrowError("Argument 6 must be an object");
      } else if (info[6]->IsObject()) {
        Image *prevFrame;
        Local<Object> _obj_ = Local<Object>::Cast(info[6]);
        prevFrame = ObjectWrap::Unwrap<Image>(_obj_);
        gdImageGifAnimAdd(*im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, *prevFrame);
      } else {
        gdImageGifAnimAdd(*im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, NULL);
      }

      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    static NAN_METHOD(GifAnimEnd) {
      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "ab");
      gdImageGifAnimEnd(out);
      fclose(out);

      info.GetReturnValue().Set(info.This());
    }

    /**
     * Miscellaneous Functions
     */
    static NAN_METHOD(Compare) {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      REQ_ARGS(1);
      if (!info[0]->IsObject())
        return Nan::ThrowError("Argument 0  must be an image");

      Local<Object> obj = Local<Object>::Cast(info[0]);
      Image *im2 = ObjectWrap::Unwrap<Image>(obj);

      Local<Number> result = Nan::New<Integer>(gdImageCompare(*im, *im2));

      info.GetReturnValue().Set(result);
    }
  };
};

Nan::Persistent<FunctionTemplate> Gd::Image::constructor;
