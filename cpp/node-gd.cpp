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
  return NanThrowError(STR);

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N))                                              \
    return NanThrowError("Expected " #N " arguments");

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return NanThrowError("Argument " #I " must be a string");           \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (args.Length() <= (I) || !args[I]->IsInt32())                      \
    return NanThrowError("Argument " #I " must be an integer");         \
  VAR = args[I]->Int32Value();

#define REQ_FN_ARG(I, VAR)                                              \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    return NanThrowError("Argument " #I " must be a function");         \
  Local<Function> VAR = args[I].As<Function>();

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (args.Length() <= (I) || !args[I]->IsNumber())                     \
    return NanThrowError("Argument " #I " must be a number");           \
  VAR = args[I]->NumberValue();

#define REQ_EXT_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsExternal())                   \
    return NanThrowError("Argument " #I " invalid");                    \
  Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_IMG_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsObject())                     \
    return NanThrowError("Argument " #I " must be an object");          \
  Local<Object> _obj_ = Local<Object>::Cast(args[I]);                   \
  Image *VAR = ObjectWrap::Unwrap<Image>(_obj_);

#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (args.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (args[I]->IsInt32()) {                                      \
    VAR = args[I]->Int32Value();                                        \
  } else {                                                              \
    return NanThrowError("Argument " #I " must be an integer");         \
  }

#define RETURN_IMAGE(IMG)                                               \
  if (!IMG) NanReturnNull();                                            \
  Local<Value> _arg_ = NanNew<External>(IMG);                           \
  Persistent<Object> handle;                                            \
  Local<FunctionTemplate> func = NanNew(Image::constructor);           \
  Local<Object> _image_ = func->GetFunction()->NewInstance(1, &_arg_); \
  NanAssignPersistent(handle, _image_);                                 \
  NanReturnValue(_image_);

#define DECLARE_CREATE_FROM(TYPE)                                       \
  static NAN_METHOD(CreateFrom##TYPE) {                                 \
    NanScope();                                                         \
    REQ_STR_ARG(0, path);                                               \
    FILE *in; in = fopen(*path, "rb");                                  \
    gdImagePtr im = gdImageCreateFrom##TYPE(in);                        \
    fclose(in);                                                         \
    RETURN_IMAGE(im)                                                    \
  }                                                                     \
  static NAN_METHOD(CreateFrom##TYPE##Ptr) {                            \
    NanScope();                                                         \
    REQ_ARGS(1);                                                        \
    ASSERT_IS_STRING_OR_BUFFER(args[0]);                                \
    gdImagePtr im;                                                      \
    if(Buffer::HasInstance(args[0])) {                                  \
      Local<Value> obj = args[0]->ToObject();                           \
      char *buffer_data = Buffer::Data(obj);                            \
      size_t buffer_length = Buffer::Length(obj);                       \
      im = gdImageCreateFrom##TYPE##Ptr(buffer_length, buffer_data);    \
    } else {                                                            \
      ssize_t len = DecodeBytes(args[0], BINARY);                       \
      char* buf = new char[len];                                        \
      ssize_t written = DecodeWrite(buf, len, args[0]);                 \
      assert(written == len);                                           \
      im = gdImageCreateFrom##TYPE##Ptr(len, buf);                      \
      delete[] buf;                                                     \
    }                                                                   \
    RETURN_IMAGE(im)                                                    \
  }

#define ASSERT_IS_STRING_OR_BUFFER(val)                                 \
  if (!val->IsString() && !Buffer::HasInstance(val)) {                  \
    return NanThrowError("Not a string or buffer");                     \
  }

#define RETURN_DATA()                                                   \
  NanScope();                                                           \
  Local<Value> result = NanEncode(data, size, Nan::BINARY);             \
  delete[] data;                                                        \
  NanReturnValue(result);

#define COLOR_ANTIALIASED    gdAntiAliased
#define COLOR_BRUSHED        gdBrushed
#define COLOR_STYLED         gdStyled
#define COLOR_STYLEDBRUSHED  gdStyledBrushed
#define COLOR_TITLED         gdTiled
#define COLOR_TRANSPARENT    gdTransparent

class Gd : public node::ObjectWrap {
public:
  static void Init(Local<Object> exports) {
    NanScope();

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

    NODE_SET_METHOD(exports, "create", ImageCreate);
    NODE_SET_METHOD(exports, "createTrueColor", ImageCreateTrueColor);
    NODE_SET_METHOD(exports, "createFromJpeg", CreateFromJpeg);
    NODE_SET_METHOD(exports, "createFromJpegPtr", CreateFromJpegPtr);
    NODE_SET_METHOD(exports, "createFromPng", CreateFromPng);
    NODE_SET_METHOD(exports, "createFromPngPtr", CreateFromPngPtr);
    NODE_SET_METHOD(exports, "createFromGif", CreateFromGif);
    NODE_SET_METHOD(exports, "createFromGifPtr", CreateFromGifPtr);
    NODE_SET_METHOD(exports, "createFromGd2", CreateFromGd2);
    NODE_SET_METHOD(exports, "createFromGd2Ptr", CreateFromGd2Ptr);
    NODE_SET_METHOD(exports, "createFromGd2Part", CreateFromGd2Part);
    NODE_SET_METHOD(exports, "createFromGd2PartPtr", CreateFromGd2PartPtr);
    NODE_SET_METHOD(exports, "createFromWBMP", CreateFromWBMP);
    NODE_SET_METHOD(exports, "createFromWBMPPtr", CreateFromWBMPPtr);
#if HAS_LIBWEBP
    NODE_SET_METHOD(exports, "createFromWebp", CreateFromWebp);
    NODE_SET_METHOD(exports, "createFromWebpPtr", CreateFromWebpPtr);
#endif

#if SUPPORTS_GD_2_1_0
    NODE_SET_METHOD(exports, "createFromBmp", CreateFromBmp);
    NODE_SET_METHOD(exports, "createFromBmpPtr", CreateFromBmpPtr);
    // NODE_SET_METHOD(exports, "createFromTiff", CreateFromTiff);
    // NODE_SET_METHOD(exports, "createFromTiffPtr", CreateFromTiffPtr);
#endif

#if SUPPORTS_GD_2_1_1
    NODE_SET_METHOD(exports, "createFromFile", CreateFromFile);
#endif
    NODE_SET_METHOD(exports, "trueColor", TrueColor);
    NODE_SET_METHOD(exports, "trueColorAlpha", TrueColorAlpha);
    NODE_SET_METHOD(exports, "getGDVersion", GdVersionGetter);

    Image::Init(exports);
  }

private:
  static NAN_METHOD(ImageCreate) {
    NanScope();
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreate(width, height);

    RETURN_IMAGE(im)
  }

  static NAN_METHOD(ImageCreateTrueColor) {
    NanScope();
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreateTrueColor(width, height);

    RETURN_IMAGE(im)
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
    NanScope();
    REQ_STR_ARG(0, path);

    gdImagePtr im = gdImageCreateFromFile(*path);

    RETURN_IMAGE(im)
  }
#endif

  static NAN_METHOD(CreateFromGd2Part) {
    NanScope();
    REQ_STR_ARG(0, path);
    REQ_INT_ARG(1, srcX);
    REQ_INT_ARG(2, srcY);
    REQ_INT_ARG(3, width);
    REQ_INT_ARG(4, height);

    FILE *in;
    in = fopen(*path, "rb");
    gdImagePtr im = gdImageCreateFromGd2Part(in, srcX, srcY, width, height);
    fclose(in);

    RETURN_IMAGE(im)
  }

  static NAN_METHOD(CreateFromGd2PartPtr) {
    NanScope();
    REQ_ARGS(5);
    REQ_INT_ARG(1, srcX);
    REQ_INT_ARG(2, srcY);
    REQ_INT_ARG(3, width);
    REQ_INT_ARG(4, height);

    ssize_t len = DecodeBytes(args[0]);
    char *buf   = new char[len];
    ssize_t written = DecodeWrite(buf, len, args[0]);
    assert(written == len);

    gdImagePtr im = gdImageCreateFromGd2PartPtr(len, buf, srcX, srcY, width, height);
    delete[] buf;

    RETURN_IMAGE(im)
  }

  static NAN_METHOD(TrueColor) {
    NanScope();

    REQ_ARGS(3);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);

    uint32_t result = NanNew<Uint32>(gdTrueColor(r, g, b))->Uint32Value();

    NanReturnValue(result);
  }

  static NAN_METHOD(TrueColorAlpha) {
    NanScope();

    REQ_ARGS(4);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);
    REQ_INT_ARG(3, a);

    uint32_t result = NanNew<Uint32>(gdTrueColorAlpha(r, g, b, a))->Uint32Value();

    NanReturnValue(result);
  }

  static NAN_METHOD(GdVersionGetter) {
    NanScope();
    std::stringstream version_string;
    version_string << GD_MAJOR_VERSION << "." << GD_MINOR_VERSION << "." << GD_RELEASE_VERSION;
    NanReturnValue(version_string.str());
  }


  class Image : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor;

    static void Init(v8::Local<Object> exports) {
      NanScope();

      Local<FunctionTemplate> t = NanNew<FunctionTemplate>(New);
      t->SetClassName(NanNew("Image"));
      t->InstanceTemplate()->SetInternalFieldCount(1);

      NODE_SET_PROTOTYPE_METHOD(t, "jpeg", Jpeg);
      NODE_SET_PROTOTYPE_METHOD(t, "jpegPtr", JpegPtr);
      NODE_SET_PROTOTYPE_METHOD(t, "png", Png);
      NODE_SET_PROTOTYPE_METHOD(t, "pngPtr", PngPtr);
      NODE_SET_PROTOTYPE_METHOD(t, "gif", Gif);
      NODE_SET_PROTOTYPE_METHOD(t, "gifPtr", GifPtr);
      NODE_SET_PROTOTYPE_METHOD(t, "gd", Gd);
      NODE_SET_PROTOTYPE_METHOD(t, "gdPtr", GdPtr);
      NODE_SET_PROTOTYPE_METHOD(t, "gd2", Gd2);
      NODE_SET_PROTOTYPE_METHOD(t, "gd2Ptr", Gd2Ptr);
      NODE_SET_PROTOTYPE_METHOD(t, "wbmp", WBMP);
      NODE_SET_PROTOTYPE_METHOD(t, "wbmpPtr", WBMPPtr);
#if HAS_LIBWEBP
      NODE_SET_PROTOTYPE_METHOD(t, "webp", Webp);
      NODE_SET_PROTOTYPE_METHOD(t, "webpPtr", WebpPtr);
#endif

#if SUPPORTS_GD_2_1_0
      NODE_SET_PROTOTYPE_METHOD(t, "bmp", Bmp);
      NODE_SET_PROTOTYPE_METHOD(t, "bmpPtr", BmpPtr);
#endif

#if HAS_LIBTIFF
      NODE_SET_PROTOTYPE_METHOD(t, "tiff", Tiff);
      NODE_SET_PROTOTYPE_METHOD(t, "tiffPtr", TiffPtr);
#endif

#if SUPPORTS_GD_2_1_1
      NODE_SET_PROTOTYPE_METHOD(t, "file", File);
      NODE_SET_PROTOTYPE_METHOD(t, "fileCallback", FileCallback);
#endif

      NODE_SET_PROTOTYPE_METHOD(t, "destroy", Destroy);

      /**
       * Drawing Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "setPixel", SetPixel);
      NODE_SET_PROTOTYPE_METHOD(t, "line", Line);
      NODE_SET_PROTOTYPE_METHOD(t, "dashedLine", DashedLine);
      NODE_SET_PROTOTYPE_METHOD(t, "polygon", Polygon);
      NODE_SET_PROTOTYPE_METHOD(t, "openPolygon", OpenPolygon);
      NODE_SET_PROTOTYPE_METHOD(t, "filledPolygon", FilledPolygon);
      NODE_SET_PROTOTYPE_METHOD(t, "rectangle", Rectangle);
      NODE_SET_PROTOTYPE_METHOD(t, "filledRectangle", FilledRectangle);
      NODE_SET_PROTOTYPE_METHOD(t, "arc", Arc);
      NODE_SET_PROTOTYPE_METHOD(t, "filledArc", FilledArc);
#if SUPPORTS_GD_2_1_0
      NODE_SET_PROTOTYPE_METHOD(t, "ellipse", Ellipse);
#endif
      NODE_SET_PROTOTYPE_METHOD(t, "filledEllipse", FilledEllipse);
      NODE_SET_PROTOTYPE_METHOD(t, "fillToBorder", FillToBorder);
      NODE_SET_PROTOTYPE_METHOD(t, "fill", Fill);
      NODE_SET_PROTOTYPE_METHOD(t, "setAntiAliased", SetAntiAliased);
      NODE_SET_PROTOTYPE_METHOD(t, "setAntiAliasedDontBlend", SetAntiAliasedDontBlend);
      NODE_SET_PROTOTYPE_METHOD(t, "setBrush", SetBrush);
      NODE_SET_PROTOTYPE_METHOD(t, "setTile", SetTile);
      NODE_SET_PROTOTYPE_METHOD(t, "setStyle", SetStyle);
      NODE_SET_PROTOTYPE_METHOD(t, "setThickness", SetThickness);
      NODE_SET_PROTOTYPE_METHOD(t, "alphaBlending", AlphaBlending);
      NODE_SET_PROTOTYPE_METHOD(t, "saveAlpha", SaveAlpha);
      NODE_SET_PROTOTYPE_METHOD(t, "setClip", SetClip);
      NODE_SET_PROTOTYPE_METHOD(t, "getClip", GetClip);
#if SUPPORTS_GD_2_1_0
      NODE_SET_PROTOTYPE_METHOD(t, "setResolution", SetResolution);
#endif

      /**
       * Query Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "Alpha", Alpha);
      NODE_SET_PROTOTYPE_METHOD(t, "getPixel", GetPixel);
      NODE_SET_PROTOTYPE_METHOD(t, "getTrueColorPixel", GetTrueColorPixel);
      NODE_SET_PROTOTYPE_METHOD(t, "imageColorAt", ImageColorAt);
      NODE_SET_PROTOTYPE_METHOD(t, "getBoundsSafe", GetBoundsSafe);
      // trueColor
      t->InstanceTemplate()->SetAccessor(NanNew("trueColor"),
        TrueColorGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      // width, height
      t->InstanceTemplate()->SetAccessor(NanNew("width"),
        WidthGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      t->InstanceTemplate()->SetAccessor(NanNew("height"),
        HeightGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

      /**
       * Font and Text Handling Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "stringFTBBox", StringFTBBox);
      NODE_SET_PROTOTYPE_METHOD(t, "stringFT", StringFT);

      /**
       * Color Handling Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "colorAllocate", ColorAllocate);
      NODE_SET_PROTOTYPE_METHOD(t, "colorAllocateAlpha", ColorAllocateAlpha);
      NODE_SET_PROTOTYPE_METHOD(t, "colorClosest", ColorClosest);
      NODE_SET_PROTOTYPE_METHOD(t, "colorClosestAlpha", ColorClosestAlpha);
      NODE_SET_PROTOTYPE_METHOD(t, "colorClosestHWB", ColorClosestHWB);
      NODE_SET_PROTOTYPE_METHOD(t, "colorExact", ColorExact);
      NODE_SET_PROTOTYPE_METHOD(t, "colorResolve", ColorResolve);
      NODE_SET_PROTOTYPE_METHOD(t, "colorResolveAlpha", ColorResolveAlpha);
      NODE_SET_PROTOTYPE_METHOD(t, "red", Red);
      NODE_SET_PROTOTYPE_METHOD(t, "green", Green);
      NODE_SET_PROTOTYPE_METHOD(t, "blue", Blue);
      NODE_SET_PROTOTYPE_METHOD(t, "getTransparent", GetTransparent);
      NODE_SET_PROTOTYPE_METHOD(t, "colorDeallocate", ColorDeallocate);

      /**
       * Color Manipulation Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "colorTransparent", ColorTransparent);
#if SUPPORTS_GD_2_1_0
      NODE_SET_PROTOTYPE_METHOD(t, "colorReplace", ColorReplace);
      NODE_SET_PROTOTYPE_METHOD(t, "colorReplaceThreshold", ColorReplaceThreshold);
      NODE_SET_PROTOTYPE_METHOD(t, "colorReplaceArray", ColorReplaceArray);

      /**
       * Effects
       */
      NODE_SET_PROTOTYPE_METHOD(t, "grayscale", GrayScale);
      NODE_SET_PROTOTYPE_METHOD(t, "gaussianBlur", GaussianBlur);
      NODE_SET_PROTOTYPE_METHOD(t, "negate", Negate);
      NODE_SET_PROTOTYPE_METHOD(t, "brightness", Brightness);
      NODE_SET_PROTOTYPE_METHOD(t, "contrast", Contrast);
      NODE_SET_PROTOTYPE_METHOD(t, "selectiveBlur", SelectiveBlur);
      NODE_SET_PROTOTYPE_METHOD(t, "flipHorizontal", FlipHorizontal);
      NODE_SET_PROTOTYPE_METHOD(t, "flipVertical", FlipVertical);
      NODE_SET_PROTOTYPE_METHOD(t, "flipBoth", FlipBoth);
      NODE_SET_PROTOTYPE_METHOD(t, "crop", Crop);
      NODE_SET_PROTOTYPE_METHOD(t, "cropAuto", CropAuto);
      NODE_SET_PROTOTYPE_METHOD(t, "cropThreshold", CropThreshold);
#endif

#if SUPPORTS_GD_2_1_1
      NODE_SET_PROTOTYPE_METHOD(t, "emboss", Emboss);
#endif
      NODE_SET_PROTOTYPE_METHOD(t, "sharpen", Sharpen);
      NODE_SET_PROTOTYPE_METHOD(t, "createPaletteFromTrueColor", CreatePaletteFromTrueColor);
      NODE_SET_PROTOTYPE_METHOD(t, "trueColorToPalette", TrueColorToPalette);
#if SUPPORTS_GD_2_1_0
      NODE_SET_PROTOTYPE_METHOD(t, "paletteToTrueColor", PaletteToTrueColor);
      NODE_SET_PROTOTYPE_METHOD(t, "colorMatch", ColorMatch);
#endif
      NODE_SET_PROTOTYPE_METHOD(t, "gifAnimBegin", GifAnimBegin);
      NODE_SET_PROTOTYPE_METHOD(t, "gifAnimAdd", GifAnimAdd);
      NODE_SET_PROTOTYPE_METHOD(t, "gifAnimEnd", GifAnimEnd);

      // interlace
      t->InstanceTemplate()->SetAccessor(NanNew("interlace"),
        InterlaceGetter, InterlaceSetter, Local<Value>(), PROHIBITS_OVERWRITING);
      t->InstanceTemplate()->SetAccessor(NanNew("colorsTotal"),
        ColorsTotalGetter, 0, Local<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

      /**
       * Copying and Resizing Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "copy", Copy);
      NODE_SET_PROTOTYPE_METHOD(t, "copyResized", CopyResized);
      NODE_SET_PROTOTYPE_METHOD(t, "copyResampled", CopyResampled);
      NODE_SET_PROTOTYPE_METHOD(t, "copyRotated", CopyRotated);
      NODE_SET_PROTOTYPE_METHOD(t, "copyMerge", CopyMerge);
      NODE_SET_PROTOTYPE_METHOD(t, "copyMergeGray", CopyMergeGray);
      NODE_SET_PROTOTYPE_METHOD(t, "paletteCopy", PaletteCopy);
      NODE_SET_PROTOTYPE_METHOD(t, "squareToCircle", SquareToCircle);

      /**
       * Miscellaneous Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "compare", Compare);

      NanAssignPersistent(constructor, t);
      exports->Set(NanNew<String>("Image"), t->GetFunction());
    }

    static NAN_METHOD(New) {
      NanScope();
      REQ_EXT_ARG(0, image);

      gdImagePtr imgPtr = (gdImagePtr)image->Value();
      (new Image(imgPtr))->Wrap(args.This());

      NanReturnThis();
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
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());
      if(im->_image){
        gdImageDestroy(*im);
        im->_image = NULL;
      }

      NanReturnUndefined();
    }

    static NAN_METHOD(Jpeg) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, quality, -1);

      FILE *out = fopen(*path, "wb");
      gdImageJpeg(*im, out, quality);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(JpegPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, quality, -1);

      int size;
      char *data = (char*)gdImageJpegPtr(*im, &size, quality);

      RETURN_DATA()
    }

    static NAN_METHOD(Gif) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGif(*im, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(GifPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int size;
      char *data = (char*)gdImageGifPtr(*im, &size);

      RETURN_DATA()
    }

    static NAN_METHOD(Png) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      FILE *out = fopen(*path, "wb");
      gdImagePngEx(*im, out, level);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(PngPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, level, -1);

      int size;
      char *data = (char*)gdImagePngPtrEx(*im, &size, level);

      RETURN_DATA()
    }

    static NAN_METHOD(WBMP) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      FILE *out = fopen(*path, "wb");
      gdImageWBMP(*im, foreground, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(WBMPPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, foreground);

      int size;
      char *data = (char*)gdImageWBMPPtr(*im, &size, foreground);

      RETURN_DATA()
    }

#if HAS_LIBWEBP
    static NAN_METHOD(Webp) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      FILE *out = fopen(*path, "wb");
      gdImageWebpEx(*im, out, level);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(WebpPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, level, -1);

      int size;
      char *data = (char*)gdImageWebpPtrEx(*im, &size, level);

      RETURN_DATA()
    }
#endif

    static NAN_METHOD(Gd) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGd(*im, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(GdPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int size;
      char *data = (char*)gdImageGdPtr(*im, &size);

      RETURN_DATA()
    }

    static NAN_METHOD(Gd2) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, chunkSize);
      OPT_INT_ARG(2, format, GD2_FMT_RAW);

      FILE *out = fopen(*path, "wb");
      gdImageGd2(*im, out, chunkSize, format);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(Gd2Ptr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, chunkSize);
      OPT_INT_ARG(1, format, GD2_FMT_RAW);

      int size;
      char *data = (char*)gdImageGd2Ptr(*im, chunkSize, format, &size);

      RETURN_DATA()
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(Bmp) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, compression);

      FILE *out = fopen(*path, "wb");
      gdImageBmp(*im, out, compression);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(BmpPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, compression, 0);

      int size;
      char *data = (char*)gdImageBmpPtr(*im, &size, compression);

      RETURN_DATA()
    }
#endif

#if HAS_LIBTIFF
    static NAN_METHOD(Tiff) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageTiff(*im, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(TiffPtr) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int size;
      char *data = (char*)gdImageTiffPtr(*im, &size);

      RETURN_DATA()
    }
#endif

#if SUPPORTS_GD_2_1_1
    static NAN_METHOD(File) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      gdImageFile(*im, *path);

      NanReturnThis();
    }

    static NAN_METHOD(FileCallback) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_FN_ARG(1, cb);

      NanCallback *callback = new NanCallback(cb);

      bool result = gdImageFile(*im, *path);
      if (result == false) {
        Local<Value> err = Exception::Error(NanNew<String>("Unable to write file."));
        Local<Value> argv[] = {
          // NanError(NanNew<String>("fail!"))
          err
        };
        callback->Call(1, argv);
      } else {
        Local<Value> argv[] = {
          NanNull()
        };
        callback->Call(1, argv);
      }

      delete callback;
      NanReturnThis();
    }
#endif

    /**
     * Drawing Functions
     */
    static NAN_METHOD(SetPixel) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageSetPixel(*im, x, y, color);

      NanReturnThis();
    }

    static NAN_METHOD(Line) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageLine(*im, x1, y1, x2, y2, color);

      NanReturnThis();
    }

    static NAN_METHOD(DashedLine) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageDashedLine(*im, x1, y1, x2, y2, color);

      NanReturnThis();
    }

    static NAN_METHOD(Polygon) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!args[0]->IsArray()) {
        return NanThrowError("Arguments 0 must be an array");
      }

      Local<String> x = NanNew("x");
      Local<String> y = NanNew("y");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(NanNew<Integer>(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImagePolygon(*im, points, _len, color);

      delete[] points;

      NanReturnThis();
    }

    static NAN_METHOD(OpenPolygon) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!args[0]->IsArray()) {
        return NanThrowError("Arguments 0 must be an array");
      }

      Local<String> x = NanNew("x");
      Local<String> y = NanNew("y");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(NanNew<Integer>(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImageOpenPolygon(*im, points, _len, color);

      delete[] points;

      NanReturnThis();
    }

    static NAN_METHOD(FilledPolygon) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!args[0]->IsArray()) {
        return NanThrowError("Arguments 0 must be an array");
      }

      Local<String> x = NanNew("x");
      Local<String> y = NanNew("y");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(NanNew<Integer>(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImageFilledPolygon(*im, points, _len, color);

      delete[] points;

      NanReturnThis();
    }

    static NAN_METHOD(Rectangle) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageRectangle(*im, x1, y1, x2, y2, color);

      NanReturnThis();
    }

    static NAN_METHOD(FilledRectangle) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageFilledRectangle(*im, x1, y1, x2, y2, color);

      NanReturnThis();
    }

    static NAN_METHOD(Arc) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(7);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, begin);
      REQ_INT_ARG(5, end);
      REQ_INT_ARG(6, color);

      gdImageArc(*im, cx, cy, width, height, begin, end, color);

      NanReturnThis();
    }

    static NAN_METHOD(FilledArc) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

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

      NanReturnThis();
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(Ellipse) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, color);

      gdImageEllipse(*im, cx, cy, width, height, color);

      NanReturnThis();
    }
#endif

    static NAN_METHOD(FilledEllipse) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, color);

      gdImageFilledEllipse(*im, cx, cy, width, height, color);

      NanReturnThis();
    }

    static NAN_METHOD(FillToBorder) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(4);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, border);
      REQ_INT_ARG(3, color);

      gdImageFillToBorder(*im, x, y, border, color);

      NanReturnThis();
    }

    static NAN_METHOD(Fill) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageFill(*im, x, y, color);

      NanReturnThis();
    }

    static NAN_METHOD(SetAntiAliased) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      gdImageSetAntiAliased(*im, color);

      NanReturnThis();
    }

    static NAN_METHOD(SetAntiAliasedDontBlend) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      REQ_INT_ARG(1, dont_blend); // what does this mean?

      gdImageSetAntiAliasedDontBlend(*im, color, dont_blend);

      NanReturnThis();
    }

    static NAN_METHOD(SetBrush) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_IMG_ARG(0, brush)
      gdImageSetBrush(*im, *brush);

      NanReturnThis();
    }

    static NAN_METHOD(SetTile) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_IMG_ARG(0, tile)
      gdImageSetTile(*im, *tile);

      NanReturnThis();
    }

    static NAN_METHOD(SetStyle) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      if (args.Length() < 1 || !args[0]->IsArray())
        return NanThrowError("Arguments 0 must be an array");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len  = array->Length(), _len = 0;
      int *sty =  new int[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(NanNew<Integer>(i));
        if (!v->IsNumber()) continue;

        sty[i] = v->Int32Value();
        _len++;
      }

      gdImageSetStyle(*im, sty, _len);

      delete[] sty;

      NanReturnThis();
    }

    static NAN_METHOD(SetThickness) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, thickness)

      gdImageSetThickness(*im, thickness);

      NanReturnThis();
    }

    static NAN_METHOD(AlphaBlending) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, blending)
      gdImageAlphaBlending(*im, blending);

      NanReturnThis();
    }

    static NAN_METHOD(SaveAlpha) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, saveFlag)
      gdImageSaveAlpha(*im, saveFlag);

      NanReturnThis();
    }

    static NAN_METHOD(SetClip) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(4);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);

      gdImageSetClip(*im, x1, y1, x2, y2);

      NanReturnThis();
    }

    static NAN_METHOD(GetClip) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int x1, y1, x2, y2;
      gdImageGetClip(*im, &x1, &y1, &x2, &y2);

      Local<Object> result = NanNew<Object>();
      result->Set(NanNew<String>("x1"), NanNew<Integer>(x1));
      result->Set(NanNew<String>("y1"), NanNew<Integer>(y1));
      result->Set(NanNew<String>("x2"), NanNew<Integer>(x2));
      result->Set(NanNew<String>("y2"), NanNew<Integer>(y2));

      NanReturnValue(result);
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(SetResolution) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, res_x);
      REQ_INT_ARG(1, res_y);

      gdImageSetResolution(*im, res_x, res_y);

      NanReturnThis();
    }
#endif

    /**
     * Query Functions
     */
    static NAN_METHOD(Alpha) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageAlpha(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_METHOD(GetPixel) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = NanNew<Integer>(gdImageGetPixel(*im, x, y));
      NanReturnValue(result);
    }

    static NAN_METHOD(GetTrueColorPixel) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = NanNew<Integer>(gdImageGetTrueColorPixel(*im, x, y));
      NanReturnValue(result);
    }

    // This is implementation of the PHP-GD specific method imagecolorat
    static NAN_METHOD(ImageColorAt) {
      NanScope();
      Image *img = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result;
      gdImagePtr im = *img;
      if (gdImageTrueColor(im)) {
        if(im->tpixels && gdImageBoundsSafe(im, x, y)){
          result = NanNew<Integer>(gdImageTrueColorPixel(im, x, y));
        } else {
          return NanThrowError("[imageColorAt] Invalid pixel");
        }
      } else {
        if (im->pixels && gdImageBoundsSafe(im, x, y)) {
          result = NanNew<Integer>(im->pixels[y][x]);
        } else {
          return NanThrowError("[imageColorAt] Invalid pixel");
        }
      }
      NanReturnValue(result);
    }

    static NAN_METHOD(GetBoundsSafe) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = NanNew<Integer>(gdImageBoundsSafe(*im, x, y));
      NanReturnValue(result);
    }

    static NAN_GETTER(WidthGetter) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageSX(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_GETTER(HeightGetter) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageSY(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_GETTER(TrueColorGetter) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageTrueColor(im->operator gdImagePtr()));
      NanReturnValue(result);
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
      NanScope();

      err = gdImageStringFT(NULL, &brect[0], color, *font, size, angle, x, y, *str);
      if (err) return NanThrowError(err);

      Local<Array> result = NanNew<Array>();

      for (int i = 0; i < 8; i++) {
        result->Set(NanNew<Integer>(i), NanNew<Number>(brect[i]));
      }

      NanReturnValue(result);
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
      if (args.Length() > 7 && args[7]->IsBoolean()) {
        return_rectangle = args[7]->BooleanValue();
      }

      int brect[8];
      char *err;
      NanScope();

      if (return_rectangle) {
        err = gdImageStringFT(NULL, &brect[0], color, *font, size, angle, x, y, *str);
        if (err) return NanThrowError(err);

        Local<Array> result = NanNew<Array>();
        for(int i=0; i < 8; i++) {
          result->Set( NanNew<Integer>(i), NanNew<Number>(brect[i]) );
        }

        NanReturnValue(result);
      }

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      err = gdImageStringFT(*im, &brect[0], color, *font, size, angle, x, y, *str);
      if (err) return NanThrowError(err);

      NanReturnThis();
    }

    /**
     * Color Handling Functions
     */
    static NAN_METHOD(ColorAllocate) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorAllocate(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorAllocateAlpha) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = NanNew<Integer>(gdImageColorAllocateAlpha(*im, r, g, b, a));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorClosest) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorClosest(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorClosestAlpha) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = NanNew<Integer>(gdImageColorClosestAlpha(*im, r, g, b, a));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorClosestHWB) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorClosestHWB(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorExact) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorExact(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorResolve) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorResolve(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorResolveAlpha) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = NanNew<Integer>(gdImageColorResolveAlpha(*im, r, g, b, a));
      NanReturnValue(result);
    }

    static NAN_GETTER(ColorsTotalGetter) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageColorsTotal(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_METHOD(Red) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageRed(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_METHOD(Blue) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageBlue(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_METHOD(Green) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageGreen(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_GETTER(InterlaceGetter) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      bool interlaced = (gdImageGetInterlaced(im->operator gdImagePtr()) != 0);
      Local<Boolean> result;
      if (interlaced) {
        result = NanTrue();
      } else {
        result = NanFalse();
      }

      NanReturnValue(result);
    }

    static NAN_SETTER(InterlaceSetter) {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      if (value->IsBoolean()) {
        bool interlace = value->BooleanValue();

        gdImageInterlace(*im, interlace?1:0);
      }
    }

    static NAN_METHOD(GetTransparent) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageGetTransparent(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorDeallocate) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      gdImageColorDeallocate(*im, color);

      NanReturnThis();
    }

    static NAN_METHOD(ColorTransparent) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      gdImageColorTransparent(*im, color);

      NanReturnThis();
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(ColorReplace) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, fromColor);
      REQ_INT_ARG(1, toColor);

      Local<Number> result = NanNew<Integer>(gdImageColorReplace(*im, fromColor, toColor));

      NanReturnValue(result);
    }

    static NAN_METHOD(ColorReplaceThreshold) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, fromColor);
      REQ_INT_ARG(1, toColor);
      REQ_DOUBLE_ARG(2, threshold);

      Local<Number> result =
        NanNew<Integer>(gdImageColorReplaceThreshold(*im, fromColor, toColor, threshold));

      NanReturnValue(result);
    }

    static NAN_METHOD(ColorReplaceArray) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);

      Local<Array> fromArray = Local<Array>::Cast(args[0]);
      int flen = fromArray->Length(), _flen = 0;
      int *fromColors =  new int[flen];

      for(int i = 0; i < flen; i++) {
        Local<Value> v = fromArray->Get(NanNew<Integer>(i));
        fromColors[i] = v->Int32Value();
        _flen++;
      }

      Local<Array> toArray = Local<Array>::Cast(args[1]);
      int tlen = toArray->Length(), _tlen = 0;
      int *toColors =  new int[tlen];

      for(int j = 0; j < tlen; j++) {
        Local<Value> v = toArray->Get(NanNew<Integer>(j));
        toColors[j] = v->Int32Value();
        _tlen++;
      }

      if (_flen != _tlen) {
        return NanThrowError("Color arrays should have same length.");
      }

      Local<Number> result =
        NanNew<Integer>(gdImageColorReplaceArray(*im, _flen, fromColors, toColors));

      NanReturnValue(result);
    }

    static NAN_METHOD(GrayScale) {
      NanScope();

      Image *im = ObjectWrap::Unwrap<Image>(args.This());
      gdImageGrayScale(*im);

      NanReturnThis();
    }

    static NAN_METHOD(GaussianBlur) {
      NanScope();

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageGaussianBlur(*im);

      NanReturnThis();
    }

    static NAN_METHOD(Negate) {
      NanScope();

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageNegate(*im);

      NanReturnThis();
    }

    static NAN_METHOD(Brightness) {
      NanScope();
      REQ_INT_ARG(0, brightness);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageBrightness(*im, brightness);

      NanReturnThis();
    }

    static NAN_METHOD(Contrast) {
      NanScope();
      REQ_DOUBLE_ARG(0, contrast);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageContrast(*im, contrast);

      NanReturnThis();
    }

    static NAN_METHOD(SelectiveBlur) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSelectiveBlur(*im);
      NanReturnThis();
    }

    static NAN_METHOD(FlipHorizontal) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageFlipHorizontal(*im);
      NanReturnThis();
    }

    static NAN_METHOD(FlipVertical) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageFlipVertical(*im);
      NanReturnThis();
    }

    static NAN_METHOD(FlipBoth) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageFlipBoth(*im);
      NanReturnThis();
    }

    static NAN_METHOD(Crop) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

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
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, mode);

      if (mode > 4) {
        return NanThrowError("Crop mode should be between 0 and 5");
      }

      gdImagePtr newImage = gdImageCropAuto(*im, mode);

      RETURN_IMAGE(newImage);
    }

    static NAN_METHOD(CropThreshold) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      REQ_DOUBLE_ARG(1, threshold);

      gdImagePtr newImage = gdImageCropThreshold(*im, color, threshold);
      RETURN_IMAGE(newImage);
    }
#endif

#if SUPPORTS_GD_2_1_1
    static NAN_METHOD(Emboss) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageEmboss(*im);
      NanReturnThis();
    }
#endif

    /**
     * Copying and Resizing Functions
     */
    static NAN_METHOD(Copy) {
      NanScope();
      REQ_ARGS(7);
      REQ_IMG_ARG(0, dest);
      REQ_INT_ARG(1, dstX);
      REQ_INT_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, width);
      REQ_INT_ARG(6, height);

      Image *im   = ObjectWrap::Unwrap<Image>(args.This());

      gdImageCopy(*dest, *im, dstX, dstY, srcX, srcY, width, height);

      NanReturnThis();
    }

    static NAN_METHOD(CopyResized) {
      NanScope();
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

      Image *im   = ObjectWrap::Unwrap<Image>(args.This());

      gdImageCopyResized(*dest, *im, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

      NanReturnThis();
    }

    static NAN_METHOD(CopyResampled) {
      NanScope();
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

      Image *im   = ObjectWrap::Unwrap<Image>(args.This());

      gdImageCopyResampled(*dest, *im, dstX, dstY, srcX, srcY, destW, destH, srcW, srcH);

      NanReturnThis();
    }

    static NAN_METHOD(CopyRotated) {
      NanScope();
      REQ_ARGS(8);
      REQ_IMG_ARG(0, dest);
      REQ_DOUBLE_ARG(1, dstX);
      REQ_DOUBLE_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, srcW);
      REQ_INT_ARG(6, srcH);
      REQ_INT_ARG(7, angle);

      Image *im   = ObjectWrap::Unwrap<Image>(args.This());

      gdImageCopyRotated(*dest, *im, dstX, dstY, srcX, srcY, srcW, srcH, angle);

      NanReturnThis();
    }

    static NAN_METHOD(CopyMerge) {
      NanScope();
      REQ_ARGS(8);
      REQ_IMG_ARG(0, dest);
      REQ_INT_ARG(1, dstX);
      REQ_INT_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, width);
      REQ_INT_ARG(6, height);
      REQ_INT_ARG(7, pct);

      Image *im   = ObjectWrap::Unwrap<Image>(args.This());

      gdImageCopyMerge(*dest, *im, dstX, dstY, srcX, srcY, width, height, pct);

      NanReturnThis();
    }

    static NAN_METHOD(CopyMergeGray) {
      NanScope();
      REQ_ARGS(8);
      REQ_IMG_ARG(0, dest);
      REQ_INT_ARG(1, dstX);
      REQ_INT_ARG(2, dstY);
      REQ_INT_ARG(3, srcX);
      REQ_INT_ARG(4, srcY);
      REQ_INT_ARG(5, width);
      REQ_INT_ARG(6, height);
      REQ_INT_ARG(7, pct);

      Image *im   = ObjectWrap::Unwrap<Image>(args.This());

      gdImageCopyMergeGray(*dest, *im, dstX, dstY, srcX, srcY, width, height, pct);

      NanReturnThis();
    }

    static NAN_METHOD(PaletteCopy) {
      NanScope();
      REQ_IMG_ARG(0, dest);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImagePaletteCopy(*dest, *im);

      NanReturnThis();
    }

    static NAN_METHOD(SquareToCircle) {
      NanScope();
      REQ_INT_ARG(0, radius);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSquareToCircle(*im, radius);

      NanReturnThis();
    }

    static NAN_METHOD(Sharpen) {
      NanScope();
      REQ_INT_ARG(0, pct);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSharpen(*im, pct);

      NanReturnThis();
    }

    static NAN_METHOD(CreatePaletteFromTrueColor) {
      NanScope();

      OPT_INT_ARG(0, ditherFlag, 0);
      OPT_INT_ARG(1, colorsWanted, 256);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImagePtr newImage = gdImageCreatePaletteFromTrueColor(*im, ditherFlag, colorsWanted);

      RETURN_IMAGE(newImage);
    }

    static NAN_METHOD(TrueColorToPalette) {
      NanScope();

      OPT_INT_ARG(0, ditherFlag, 0);
      OPT_INT_ARG(1, colorsWanted, 256);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

#if SUPPORTS_GD_2_1_0
      Local<Number> result = NanNew<Integer>(gdImageTrueColorToPalette(*im, ditherFlag, colorsWanted));
      NanReturnValue(result);
#endif
#if SUPPORTS_UNTIL_GD_2_0_36
      gdImageTrueColorToPalette(*im, ditherFlag, colorsWanted);
      NanReturnUndefined();
#endif
    }

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(PaletteToTrueColor) {
      NanScope();

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImagePaletteToTrueColor(*im));

      NanReturnValue(result);
    }

    static NAN_METHOD(ColorMatch) {
      NanScope();

      REQ_IMG_ARG(0, palette);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageColorMatch(*im, *palette));

      NanReturnValue(result);
    }
#endif

    static NAN_METHOD(GifAnimBegin) {
      NanScope();

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, GlobalCM);
      REQ_INT_ARG(2, Loops);

      FILE *out = fopen(*path, "wb");

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageGifAnimBegin(*im, out, GlobalCM, Loops);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(GifAnimAdd) {
      NanScope();

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, LocalCM);
      REQ_INT_ARG(2, LeftOfs);
      REQ_INT_ARG(3, TopOfs);
      REQ_INT_ARG(4, Delay);
      REQ_INT_ARG(5, Disposal);

      FILE *out = fopen(*path, "ab");

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      if (args.Length() <= 6) {
        return NanThrowError("Argument 6 must be an object");
      } else if (args[6]->IsObject()) {
        Image *prevFrame;
        Local<Object> _obj_ = Local<Object>::Cast(args[6]);
        prevFrame = ObjectWrap::Unwrap<Image>(_obj_);
        gdImageGifAnimAdd(*im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, *prevFrame);
      } else {
        gdImageGifAnimAdd(*im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, NULL);
      }

      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(GifAnimEnd) {
      NanScope();

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "ab");
      gdImageGifAnimEnd(out);
      fclose(out);

      NanReturnThis();
    }

    /**
     * Miscellaneous Functions
     */
    static NAN_METHOD(Compare) {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(1);
      if (!args[0]->IsObject())
        return NanThrowError("Argument 0  must be an image");

      Local<Object> obj = Local<Object>::Cast(args[0]);
      Image *im2 = ObjectWrap::Unwrap<Image>(obj);

      Local<Number> result = NanNew<Integer>(gdImageCompare(*im, *im2));

      NanReturnValue(result);
    }
  };
};

Persistent<FunctionTemplate> Gd::Image::constructor;

extern "C"
{
  void init (Local<Object> exports) {
    NanScope();

    Gd::Init(exports);
  }

  NODE_MODULE(node_gd, init);
}
