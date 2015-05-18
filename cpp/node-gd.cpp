/**
 * Copyright (c) 2009-2011, Taegon Kim <gonom9@gmail.com>
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

using namespace v8;
using namespace node;

#define THROW_ERROR(TYPE, STR)                                          \
  return NanThrowError(STR);

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N))                                              \
    return NanThrowError("Expected " #N "arguments");

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return NanThrowError("Argument " #I " must be a string");           \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (args.Length() <= (I) || !args[I]->IsInt32())                      \
    return NanThrowError("Argument " #I " must be an integer");         \
  VAR = args[I]->Int32Value();

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
  NanScope();                                                           \
  Local<Value> _arg_ = NanNew<External>(IMG);                           \
  Persistent<Object> handle;                                            \
  Handle<Object> _image_ = NanNew(Image::constructor)->GetFunction()->NewInstance(1, &_arg_); \
  NanAssignPersistent(handle, _image_);                                 \
  NanReturnValue(_image_);

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
    ssize_t len = DecodeBytes(args[0]);                                 \
    char *buf   = new char[len];                                        \
    ssize_t written = DecodeWrite(buf, len, args[0]);                   \
    assert(written == len);                                             \
    gdImagePtr im = gdImageCreateFrom##TYPE##Ptr(len, buf);             \
    delete[] buf;                                                       \
    RETURN_IMAGE(im)                                                    \
  }

#define RETURN_DATA()                                                   \
  NanScope();                                                           \
  Local<Value> result = Encode(data, size);                             \
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
  static void Init(Handle<Object> exports)
  {
    NanScope();

    NODE_DEFINE_CONSTANT(exports, COLOR_ANTIALIASED);
    NODE_DEFINE_CONSTANT(exports, COLOR_BRUSHED);
    NODE_DEFINE_CONSTANT(exports, COLOR_STYLED);
    NODE_DEFINE_CONSTANT(exports, COLOR_STYLEDBRUSHED);
    NODE_DEFINE_CONSTANT(exports, COLOR_TITLED);
    NODE_DEFINE_CONSTANT(exports, COLOR_TRANSPARENT);

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
    NODE_SET_METHOD(exports, "trueColor", TrueColor);
    NODE_SET_METHOD(exports, "trueColorAlpha", TrueColorAlpha);

    Image::Init(exports);
  }

private:
  static NAN_METHOD(ImageCreate)
  {
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreate(width, height);

    RETURN_IMAGE(im)
  }

  static NAN_METHOD(ImageCreateTrueColor)
  {
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreateTrueColor(width, height);

    RETURN_IMAGE(im)
  }

  DECLARE_CREATE_FROM(Jpeg)
  DECLARE_CREATE_FROM(Png)
  DECLARE_CREATE_FROM(WBMP)
  DECLARE_CREATE_FROM(Gif)
  DECLARE_CREATE_FROM(Gd2)

  static NAN_METHOD(CreateFromGd2Part)
  {
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

  static NAN_METHOD(CreateFromGd2PartPtr)
  {
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

  static NAN_METHOD(TrueColor)
  {
    NanScope();

    REQ_ARGS(3);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);

    uint32_t result = NanNew<Uint32>(gdTrueColor(r, g, b))->Uint32Value();

    NanReturnValue(result);
  }

  static NAN_METHOD(TrueColorAlpha)
  {
    NanScope();

    REQ_ARGS(4);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);
    REQ_INT_ARG(3, a);

    uint32_t result = NanNew<Uint32>(gdTrueColorAlpha(r, g, b, a))->Uint32Value();

    NanReturnValue(result);
  }

  class Image : public node::ObjectWrap {
  public:
    static Persistent<FunctionTemplate> constructor;

    static void Init(v8::Handle<Object> exports) {
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
      NODE_SET_PROTOTYPE_METHOD(t, "filledRectangle", FilledRectangle);
      NODE_SET_PROTOTYPE_METHOD(t, "arc", Arc);
      NODE_SET_PROTOTYPE_METHOD(t, "filledArc", FilledArc);
      NODE_SET_PROTOTYPE_METHOD(t, "filledEllipse", FilledEllipse);
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

      /**
       * Query Functions
       */
      NODE_SET_PROTOTYPE_METHOD(t, "Alpha", Alpha);
      NODE_SET_PROTOTYPE_METHOD(t, "getPixel", GetPixel);
      NODE_SET_PROTOTYPE_METHOD(t, "getTrueColorPixel", GetTrueColorPixel);
      NODE_SET_PROTOTYPE_METHOD(t, "imageColorAt", ImageColorAt);
      NODE_SET_PROTOTYPE_METHOD(t, "getBoundsSafe", GetBoundsSafe);
      // trueColor
      t->PrototypeTemplate()->SetAccessor(NanNew("trueColor"), TrueColorGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      // width, height
      t->PrototypeTemplate()->SetAccessor(NanNew("width"), WidthGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      t->PrototypeTemplate()->SetAccessor(NanNew("height"), HeightGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

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
      NODE_SET_PROTOTYPE_METHOD(t, "colorTransparent", ColorTransparent);


      /**
       * Effects
       */
      NODE_SET_PROTOTYPE_METHOD(t, "toGrayscale",GrayScale);
      NODE_SET_PROTOTYPE_METHOD(t, "gaussianBlur", GaussianBlur);
      NODE_SET_PROTOTYPE_METHOD(t, "negate", Negate);
      NODE_SET_PROTOTYPE_METHOD(t, "brightness", Brightness);

      // interlace
      t->PrototypeTemplate()->SetAccessor(NanNew("interlace"), InterlaceGetter, InterlaceSetter, Handle<Value>(), PROHIBITS_OVERWRITING);
      t->PrototypeTemplate()->SetAccessor(NanNew("colorsTotal"), ColorsTotalGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);



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
      NODE_SET_PROTOTYPE_METHOD(t, "sharpen", Sharpen);

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
      (new Image((gdImagePtr)image->Value()))->Wrap(args.This());
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
    static NAN_METHOD(Destroy)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());
      if(im->_image){
        gdImageDestroy(*im);
        im->_image = NULL;
      }

      NanReturnUndefined();
    }

    static NAN_METHOD(Jpeg)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, quality, -1);

      FILE *out = fopen(*path, "wb");
      gdImageJpeg(*im, out, quality);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(JpegPtr)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, quality, -1);

      int size;
      char *data = (char*)gdImageJpegPtr(*im, &size, quality);

      RETURN_DATA()
    }

    static NAN_METHOD(Gif)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGif(*im, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(GifPtr)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      //REQ_STR_ARG(0, path);

      int size;
      char *data = (char*)gdImageGifPtr(*im, &size);

      RETURN_DATA()
    }

    static NAN_METHOD(Png)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      FILE *out = fopen(*path, "wb");
      gdImagePngEx(*im, out, level);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(PngPtr)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, level, -1);

      int size;
      char *data = (char*)gdImagePngPtrEx(*im, &size, level);

      RETURN_DATA()
    }

    static NAN_METHOD(WBMP)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      FILE *out = fopen(*path, "wb");
      gdImageWBMP(*im, foreground, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(WBMPPtr)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      int size;
      char *data = (char*)gdImageWBMPPtr(*im, &size, foreground);

      RETURN_DATA()
    }

    static NAN_METHOD(Gd)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGd(*im, out);
      fclose(out);

      NanReturnThis();
    }

    static NAN_METHOD(GdPtr)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int size;
      char *data = (char*)gdImageGdPtr(*im, &size);

      RETURN_DATA()
    }

    static NAN_METHOD(Gd2)
    {
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

    static NAN_METHOD(Gd2Ptr)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, chunkSize);
      OPT_INT_ARG(1, format, GD2_FMT_RAW);

      int size;
      char *data = (char*)gdImageGd2Ptr(*im, chunkSize, format, &size);

      RETURN_DATA()
    }

    /**
     * Drawing Functions
     */
    static NAN_METHOD(SetPixel)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageSetPixel(*im, x, y, color);

      NanReturnThis();
    }

    static NAN_METHOD(Line)
    {
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

    static NAN_METHOD(DashedLine)
    {
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

    static NAN_METHOD(Polygon)
    {
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

    static NAN_METHOD(OpenPolygon)
    {
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

    static NAN_METHOD(FilledPolygon)
    {
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

    static NAN_METHOD(FilledRectangle)
    {
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

    static NAN_METHOD(Arc)
    {
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

    static NAN_METHOD(FilledArc)
    {
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

    static NAN_METHOD(FilledEllipse)
    {
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

    static NAN_METHOD(FillToBorder)
    {
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

    static NAN_METHOD(Fill)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageFill(*im, x, y, color);

      NanReturnThis();
    }

    static NAN_METHOD(SetAntiAliased)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      gdImageSetAntiAliased(*im, color);

      NanReturnThis();
    }

    static NAN_METHOD(SetAntiAliasedDontBlend)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      REQ_INT_ARG(1, dont_blend); // what does this mean?

      gdImageSetAntiAliasedDontBlend(*im, color, dont_blend);

      NanReturnThis();
    }

    static NAN_METHOD(SetBrush)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_IMG_ARG(0, brush)
      gdImageSetBrush(*im, *brush);

      NanReturnThis();
    }

    static NAN_METHOD(SetTile)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_IMG_ARG(0, tile)
      gdImageSetTile(*im, *tile);

      NanReturnThis();
    }

    static NAN_METHOD(SetStyle)
    {
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

    static NAN_METHOD(SetThickness)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, thickness)

      gdImageSetThickness(*im, thickness);

      NanReturnThis();
    }

    static NAN_METHOD(AlphaBlending)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, blending)
      gdImageAlphaBlending(*im, blending);

      NanReturnThis();
    }

    static NAN_METHOD(SaveAlpha)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, saveFlag)
      gdImageSaveAlpha(*im, saveFlag);

      NanReturnThis();
    }

    static NAN_METHOD(SetClip)
    {
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

    static NAN_METHOD(GetClip)
    {
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

    /**
     * Query Functions
     */
    static NAN_METHOD(Alpha)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageAlpha(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_METHOD(GetPixel)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = NanNew<Integer>(gdImageGetPixel(*im, x, y));
      NanReturnValue(result);
    }

    static NAN_METHOD(GetTrueColorPixel)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = NanNew<Integer>(gdImageGetTrueColorPixel(*im, x, y));
      NanReturnValue(result);
    }

    // This is implementation of the PHP-GD specific method imagecolorat
    static NAN_METHOD(ImageColorAt)
    {
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
          return NanThrowError("[imageColorAt]Invalid pixel");
        }
      } else {
        if (im->pixels && gdImageBoundsSafe(im, x, y)) {
          result = NanNew<Integer>(im->pixels[y][x]);
        } else {
          return NanThrowError("[imageColorAt]Invalid pixel");
        }
      }
      NanReturnValue(result);
    }

    static NAN_METHOD(GetBoundsSafe)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = NanNew<Integer>(gdImageBoundsSafe(*im, x, y));
      NanReturnValue(result);
    }

    static NAN_GETTER(WidthGetter)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageSX(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_GETTER(HeightGetter)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageSY(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_GETTER(TrueColorGetter)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageTrueColor(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    /**
     * Font and Text Handling Funcitons
     */
    static NAN_METHOD(StringFTBBox)
    {
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

    static NAN_METHOD(StringFT)
    {
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
    static NAN_METHOD(ColorAllocate)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorAllocate(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorAllocateAlpha)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = NanNew<Integer>(gdImageColorAllocateAlpha(*im, r, g, b, a));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorClosest)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorClosest(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorClosestAlpha)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = NanNew<Integer>(gdImageColorClosestAlpha(*im, r, g, b, a));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorClosestHWB)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorClosestHWB(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorExact)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorExact(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorResolve)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = NanNew<Integer>(gdImageColorResolve(*im, r, g, b));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorResolveAlpha)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = NanNew<Integer>(gdImageColorResolveAlpha(*im, r, g, b, a));
      NanReturnValue(result);
    }

    static NAN_GETTER(ColorsTotalGetter)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageColorsTotal(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_METHOD(Red)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageRed(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_METHOD(Blue)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageBlue(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_METHOD(Green)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = NanNew<Integer>(gdImageGreen(im->operator gdImagePtr(), color));
      NanReturnValue(result);
    }

    static NAN_GETTER(InterlaceGetter)
    {
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

    static NAN_SETTER(InterlaceSetter)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      if (value->IsBoolean()) {
        bool interlace = value->BooleanValue();

        gdImageInterlace(*im, interlace?1:0);
      }
    }

    static NAN_METHOD(GetTransparent)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = NanNew<Integer>(gdImageGetTransparent(im->operator gdImagePtr()));
      NanReturnValue(result);
    }

    static NAN_METHOD(ColorDeallocate)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      gdImageColorDeallocate(*im, color);

      NanReturnThis();
    }

    static NAN_METHOD(ColorTransparent)
    {
      NanScope();
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      gdImageColorTransparent(*im, color);

      NanReturnThis();
    }

    static NAN_METHOD(GrayScale)
    {
      NanScope();
      // Silently fail on missing gdImageGrayScale
#if (defined(GD_MAJOR_VERSION) && defined(GD_MINOR_VERSION) && GD_MAJOR_VERSION >= 2 && GD_MINOR_VERSION >= 1)
      Image *im = ObjectWrap::Unwrap<Image>(args.This());
      gdImageGrayScale(*im);
#endif
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
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, brightness);
      gdImageBrightness(*im, brightness);

      NanReturnThis();
    }

    /**
     * Copying and Resizing Functions
     */
    static NAN_METHOD(Copy)
    {
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

    static NAN_METHOD(CopyResized)
    {
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

    static NAN_METHOD(CopyResampled)
    {
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

    static NAN_METHOD(CopyRotated)
    {
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

    static NAN_METHOD(CopyMerge)
    {
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

    static NAN_METHOD(CopyMergeGray)
    {
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

    static NAN_METHOD(PaletteCopy)
    {
      NanScope();
      REQ_IMG_ARG(0, dest);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImagePaletteCopy(*dest, *im);

      NanReturnThis();
    }

    static NAN_METHOD(SquareToCircle)
    {
      NanScope();
      REQ_INT_ARG(0, radius);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSquareToCircle(*im, radius);

      NanReturnThis();
    }

    static NAN_METHOD(Sharpen)
    {
      NanScope();
      REQ_INT_ARG(0, pct);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSharpen(*im, pct);

      NanReturnThis();
    }

    /**
     * Miscellaneous Functions
     */
    static NAN_METHOD(Compare)
    {
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
  void init (Handle<Object> exports)
  {
    NanScope();

    Gd::Init(exports);
  }

  NODE_MODULE(node_gd, init);
}
