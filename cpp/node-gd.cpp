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
#include <string.h>
#include <assert.h>

using namespace v8;
using namespace node;

#define THROW_ERROR(TYPE, STR)                                          \
  return ThrowException(Exception::TYPE(String::New(STR)));

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N))                                              \
    return ThrowException(Exception::TypeError(                         \
                             String::New("Expected " #N "arguments")));

#define REQ_STR_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));    \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (args.Length() <= (I) || !args[I]->IsInt32())                      \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be an integer")));  \
  VAR = args[I]->Int32Value();

#define REQ_DOUBLE_ARG(I, VAR)                                          \
  double VAR;                                                           \
  if (args.Length() <= (I) || !args[I]->IsNumber())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a number")));    \
  VAR = args[I]->NumberValue();

#define REQ_EXT_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsExternal())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " invalid")));             \
  Local<External> VAR = Local<External>::Cast(args[I]);

#define REQ_IMG_ARG(I, VAR)                                             \
  if (args.Length() <= (I) || !args[I]->IsObject())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be an object")));   \
  Local<Object> _obj_ = Local<Object>::Cast(args[I]);                   \
  Image *VAR = ObjectWrap::Unwrap<Image>(_obj_);


#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (args.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (args[I]->IsInt32()) {                                      \
    VAR = args[I]->Int32Value();                                        \
  } else {                                                              \
    return ThrowException(Exception::TypeError(                         \
              String::New("Argument " #I " must be an integer")));      \
  }

#define RETURN_IMAGE(IMG)                                               \
  if (!IMG) return Null();                                              \
  HandleScope scope;                                                    \
  Local<Value> _arg_ = External::New(IMG);                              \
  Persistent<Object> _image_(Image::constructor_template->GetFunction()->NewInstance(1, &_arg_)); \
  return scope.Close(_image_);

#define DECLARE_CREATE_FROM(TYPE)                                       \
  static Handle<Value> CreateFrom##TYPE (const Arguments &args) {       \
    REQ_STR_ARG(0, path);                                               \
    FILE *in; in = fopen(*path, "rb");                                  \
    gdImagePtr im = gdImageCreateFrom##TYPE(in);                        \
    fclose(in);                                                         \
    RETURN_IMAGE(im)                                                    \
  }                                                                     \
  static Handle<Value> CreateFrom##TYPE##Ptr (const Arguments &args){   \
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
  HandleScope scope;                                                    \
  Local<Value> result = Encode(data, size);                             \
  delete[] data;                                                        \
  return scope.Close(result);

#define COLOR_ANTIALIASED    gdAntiAliased
#define COLOR_BRUSHED        gdBrushed
#define COLOR_STYLED         gdStyled
#define COLOR_STYLEDBRUSHED  gdStyledBrushed
#define COLOR_TITLED         gdTiled
#define COLOR_TRANSPARENT    gdTransparent

class Gd
{
public:
  static void Init(Handle<Object> target)
  {
    HandleScope scope;

    NODE_DEFINE_CONSTANT(target, COLOR_ANTIALIASED);
    NODE_DEFINE_CONSTANT(target, COLOR_BRUSHED);
    NODE_DEFINE_CONSTANT(target, COLOR_STYLED);
    NODE_DEFINE_CONSTANT(target, COLOR_STYLEDBRUSHED);
    NODE_DEFINE_CONSTANT(target, COLOR_TITLED);
    NODE_DEFINE_CONSTANT(target, COLOR_TRANSPARENT);

    NODE_SET_METHOD(target, "create", ImageCreate);
    NODE_SET_METHOD(target, "createTrueColor", ImageCreateTrueColor);
    NODE_SET_METHOD(target, "createFromJpeg", CreateFromJpeg);
    NODE_SET_METHOD(target, "createFromJpegPtr", CreateFromJpegPtr);
    NODE_SET_METHOD(target, "createFromPng", CreateFromPng);
    NODE_SET_METHOD(target, "createFromPngPtr", CreateFromPngPtr);
    NODE_SET_METHOD(target, "createFromGif", CreateFromGif);
    NODE_SET_METHOD(target, "createFromGifPtr", CreateFromGifPtr);
    NODE_SET_METHOD(target, "createFromGd2", CreateFromGd2);
    NODE_SET_METHOD(target, "createFromGd2Ptr", CreateFromGd2Ptr);
    NODE_SET_METHOD(target, "createFromGd2Part", CreateFromGd2Part);
    NODE_SET_METHOD(target, "createFromGd2PartPtr", CreateFromGd2PartPtr);
    NODE_SET_METHOD(target, "createFromWBMP", CreateFromWBMP);
    NODE_SET_METHOD(target, "createFromWBMPPtr", CreateFromWBMPPtr);
    NODE_SET_METHOD(target, "trueColor", TrueColor);
    NODE_SET_METHOD(target, "trueColorAlpha", TrueColorAlpha);


    Image::Init(target);
  }

protected:
  static Handle<Value> ImageCreate (const Arguments& args)
  {
    REQ_INT_ARG(0, width);
    REQ_INT_ARG(1, height);

    gdImagePtr im = gdImageCreate(width, height);

    RETURN_IMAGE(im)
  }

  static Handle<Value> ImageCreateTrueColor (const Arguments &args)
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

  static Handle<Value> CreateFromGd2Part (const Arguments &args)
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

  static Handle<Value> CreateFromGd2PartPtr (const Arguments &args)
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

  static Handle<Value> TrueColor (const Arguments &args)
  {
    HandleScope scope;

    REQ_ARGS(3);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);

    Local<Number> result = Integer::New(gdTrueColor(r, g, b));

    return scope.Close(result);
  }

  static Handle<Value> TrueColorAlpha (const Arguments &args)
  {
    HandleScope scope;

    REQ_ARGS(4);
    REQ_INT_ARG(0, r);
    REQ_INT_ARG(1, g);
    REQ_INT_ARG(2, b);
    REQ_INT_ARG(3, a);

    Local<Number> result = Integer::New(gdTrueColorAlpha(r, g, b, a));

    return scope.Close(result);
  }

  class Image : public node::ObjectWrap
  {
  public:
    static Persistent<FunctionTemplate> constructor_template;

    static void Init(v8::Handle<Object> target) {
      HandleScope scope;

      Local<FunctionTemplate> t = FunctionTemplate::New(New);
      constructor_template = Persistent<FunctionTemplate>::New(t);

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
      NODE_SET_PROTOTYPE_METHOD(t, "ellipse", Ellipse);
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
      NODE_SET_PROTOTYPE_METHOD(t, "getBoundsSafe", GetBoundsSafe);
      // trueColor
      t->PrototypeTemplate()->SetAccessor(String::NewSymbol("trueColor"), TrueColorGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      // width, height
      t->PrototypeTemplate()->SetAccessor(String::NewSymbol("width"), WidthGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
      t->PrototypeTemplate()->SetAccessor(String::NewSymbol("height"), HeightGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

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
      // interlace
      t->PrototypeTemplate()->SetAccessor(String::NewSymbol("interlace"), InterlaceGetter, InterlaceSetter, Handle<Value>(), PROHIBITS_OVERWRITING);
      t->PrototypeTemplate()->SetAccessor(String::NewSymbol("colorsTotal"), ColorsTotalGetter, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

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

      target->Set(String::NewSymbol("Image"), t->GetFunction());
    }

    static Handle<Value> New(const Arguments& args) {
      HandleScope scope;
      int I = 0;
      REQ_EXT_ARG(0, image);
      (new Image((gdImagePtr)image->Value()))->Wrap(args.This());
      return args.This();
    }
  protected:
    Image(gdImagePtr image) : _image(image) {}
    ~Image() { if(_image) gdImageDestroy(_image);  }

    gdImagePtr _image;

    operator gdImagePtr () const { return _image; }

    /**
     * Destruction, Loading and Saving Functions
     */
    static Handle<Value> Destroy (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageDestroy(*im);

      return Undefined();
    }

    static Handle<Value> Jpeg (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, quality, -1);

      FILE *out = fopen(*path, "wb");
      gdImageJpeg(*im, out, quality);
      fclose(out);

      return args.This();
    }

    static Handle<Value> JpegPtr (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, quality, -1);

      int size;
      char *data = (char*)gdImageJpegPtr(*im, &size, quality);

      RETURN_DATA()
    }

    static Handle<Value> Gif (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGif(*im, out);
      fclose(out);

      return args.This();
    }

    static Handle<Value> GifPtr (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      //REQ_STR_ARG(0, path);

      int size;
      char *data = (char*)gdImageGifPtr(*im, &size);

      RETURN_DATA()
    }

    static Handle<Value> Png (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      OPT_INT_ARG(1, level, -1);

      FILE *out = fopen(*path, "wb");
      gdImagePngEx(*im, out, level);
      fclose(out);

      return args.This();
    }

    static Handle<Value> PngPtr (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, level, -1);

      int size;
      char *data = (char*)gdImagePngPtrEx(*im, &size, level);

      RETURN_DATA()
    }

    static Handle<Value> WBMP (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      FILE *out = fopen(*path, "wb");
      gdImageWBMP(*im, foreground, out);
      fclose(out);

      return args.This();
    }

    static Handle<Value> WBMPPtr (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, foreground);

      int size;
      char *data = (char*)gdImageWBMPPtr(*im, &size, foreground);

      RETURN_DATA()
    }

    static Handle<Value> Gd (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);

      FILE *out = fopen(*path, "wb");
      gdImageGd(*im, out);
      fclose(out);

      return args.This();
    }

    static Handle<Value> GdPtr (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int size;
      char *data = (char*)gdImageGdPtr(*im, &size);

      RETURN_DATA()
    }

    static Handle<Value> Gd2 (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_STR_ARG(0, path);
      REQ_INT_ARG(1, chunkSize);
      OPT_INT_ARG(2, format, GD2_FMT_RAW);

      FILE *out = fopen(*path, "wb");
      gdImageGd2(*im, out, chunkSize, format);
      fclose(out);

      return args.This();
    }

    static Handle<Value> Gd2Ptr (const Arguments &args)
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
    static Handle<Value> SetPixel (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageSetPixel(*im, x, y, color);

      return args.This();
    }

    static Handle<Value> Line (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageLine(*im, x1, y1, x2, y2, color);

      return args.This();
    }

    static Handle<Value> DashedLine (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageDashedLine(*im, x1, y1, x2, y2, color);

      return args.This();
    }

    static Handle<Value> Polygon (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!args[0]->IsArray()) {
        return ThrowException(Exception::TypeError(
              String::New("Arguments 0 must be an array")));
      }

      Local<String> x = String::NewSymbol("x");
      Local<String> y = String::NewSymbol("y");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Integer::New(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImagePolygon(*im, points, _len, color);

      delete[] points;

      return args.This();
    }

    static Handle<Value> OpenPolygon (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!args[0]->IsArray()) {
        return ThrowException(Exception::TypeError(
              String::New("Arguments 0 must be an array")));
      }

      Local<String> x = String::NewSymbol("x");
      Local<String> y = String::NewSymbol("y");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Integer::New(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImageOpenPolygon(*im, points, _len, color);

      delete[] points;

      return args.This();
    }

    static Handle<Value> FilledPolygon (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(1, color);

      if (!args[0]->IsArray()) {
        return ThrowException(Exception::TypeError(
              String::New("Arguments 0 must be an array")));
      }

      Local<String> x = String::NewSymbol("x");
      Local<String> y = String::NewSymbol("y");

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len = array->Length(), _len = 0;
      gdPoint *points =  new gdPoint[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Integer::New(i));
        if (!v->IsObject()) continue;

        Local<Object> o = v->ToObject();
        if ( !o->Has(x) || !o->Has(y)) continue;

        points[i].x = o->Get(x)->Int32Value();
        points[i].y = o->Get(y)->Int32Value();
        _len++;
      }

      gdImageFilledPolygon(*im, points, _len, color);

      delete[] points;

      return args.This();
    }

    static Handle<Value> Rectangle (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageRectangle(*im, x1, y1, x2, y2, color);

      return args.This();
    }

    static Handle<Value> FilledRectangle (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);
      REQ_INT_ARG(4, color);

      gdImageFilledRectangle(*im, x1, y1, x2, y2, color);

      return args.This();
    }

    static Handle<Value> Arc (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> FilledArc (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> Ellipse (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, color);

      gdImageEllipse(*im, cx, cy, width, height, color);

      return args.This();
    }

    static Handle<Value> FilledEllipse (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(5);
      REQ_INT_ARG(0, cx);
      REQ_INT_ARG(1, cy);
      REQ_INT_ARG(2, width);
      REQ_INT_ARG(3, height);
      REQ_INT_ARG(4, color);

      gdImageFilledEllipse(*im, cx, cy, width, height, color);

      return args.This();
    }

    static Handle<Value> FillToBorder (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(4);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, border);
      REQ_INT_ARG(3, color);

      gdImageFillToBorder(*im, x, y, border, color);

      return args.This();
    }

    static Handle<Value> Fill (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);
      REQ_INT_ARG(2, color);

      gdImageFill(*im, x, y, color);

      return args.This();
    }

    static Handle<Value> SetAntiAliased (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      gdImageSetAntiAliased(*im, color);

      return args.This();
    }

    static Handle<Value> SetAntiAliasedDontBlend (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      REQ_INT_ARG(1, dont_blend); // what does this mean?

      gdImageSetAntiAliasedDontBlend(*im, color, dont_blend);

      return args.This();
    }

    static Handle<Value> SetBrush (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_IMG_ARG(0, brush)
      gdImageSetBrush(*im, *brush);

      return args.This();
    }

    static Handle<Value> SetTile (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_IMG_ARG(0, tile)
      gdImageSetTile(*im, *tile);

      return args.This();
    }

    static Handle<Value> SetStyle (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      if (args.Length() < 1 || !args[0]->IsArray())
        return ThrowException(Exception::TypeError(
              String::New("Arguments 0 must be an array")));

      Local<Array> array = Local<Array>::Cast(args[0]);
      int len  = array->Length(), _len = 0;
      int *sty =  new int[len];

      for(int i=0; i < len; i++) {
        Local<Value> v = array->Get(Integer::New(i));
        if (!v->IsNumber()) continue;

        sty[i] = v->Int32Value();
        _len++;
      }

      gdImageSetStyle(*im, sty, _len);

      delete[] sty;


      return args.This();
    }

    static Handle<Value> SetThickness (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, thickness)

      gdImageSetThickness(*im, thickness);

      return args.This();
    }

    static Handle<Value> AlphaBlending (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, blending)
      gdImageAlphaBlending(*im, blending);

      return args.This();
    }

    static Handle<Value> SaveAlpha (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, saveFlag)
      gdImageSaveAlpha(*im, saveFlag);

      return args.This();
    }

    static Handle<Value> SetClip (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(4);
      REQ_INT_ARG(0, x1);
      REQ_INT_ARG(1, y1);
      REQ_INT_ARG(2, x2);
      REQ_INT_ARG(3, y2);

      gdImageSetClip(*im, x1, y1, x2, y2);

      return args.This();
    }

    static Handle<Value> GetClip (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      int x1, y1, x2, y2;
      gdImageGetClip(*im, &x1, &y1, &x2, &y2);

      Local<Object> result = Object::New();
      result->Set(String::New("x1"), Integer::New(x1));
      result->Set(String::New("y1"), Integer::New(y1));
      result->Set(String::New("x2"), Integer::New(x2));
      result->Set(String::New("y2"), Integer::New(y2));

      return scope.Close(result);
    }

    /**
     * Query Functions
     */
    static Handle<Value> Alpha (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Integer::New(gdImageAlpha(im->operator gdImagePtr(), color));
      return scope.Close(result);
    }

    static Handle<Value> GetPixel (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = Integer::New(gdImageGetPixel(*im, x, y));
      return scope.Close(result);
    }

    static Handle<Value> GetBoundsSafe (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(2);
      REQ_INT_ARG(0, x);
      REQ_INT_ARG(1, y);

      Local<Number> result = Integer::New(gdImageBoundsSafe(*im, x, y));
      return scope.Close(result);
    }

    static Handle<Value> WidthGetter (Local<String> property, const AccessorInfo& info)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Integer::New(gdImageSX(im->operator gdImagePtr()));
      return scope.Close(result);
    }

    static Handle<Value> HeightGetter (Local<String> property, const AccessorInfo& info)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Integer::New(gdImageSY(im->operator gdImagePtr()));
      return scope.Close(result);
    }

    static Handle<Value> TrueColorGetter (Local<String> property, const AccessorInfo& info)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Integer::New(gdImageTrueColor(im->operator gdImagePtr()));
      return scope.Close(result);
    }

    /**
     * Font and Text Handling Funcitons
     */
    static Handle<Value> StringFTBBox (const Arguments &args)
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
      HandleScope scope;

      err = gdImageStringFT(NULL, &brect[0], 0, *font, size, angle, x, y, *str);
      if (err) return ThrowException(Exception::Error(String::New(err)));

      Local<Array> result = Array::New();

      for (int i = 0; i < 8; i++) {
        result->Set(Integer::New(i), Number::New(brect[i]));
      }

      return scope.Close(result);
    }

    static Handle<Value> StringFT (const Arguments &args)
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
      HandleScope scope;

      if (return_rectangle) {
        err = gdImageStringFT(NULL, &brect[0], 0, *font, size, angle, x, y, *str);
        if (err) return ThrowException(Exception::Error(String::New(err)));

        Local<Array> result = Array::New();
        for(int i=0; i < 8; i++) {
          result->Set( Integer::New(i), Number::New(brect[i]) );
        }

        return scope.Close(result);
      }

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      err = gdImageStringFT(*im, &brect[0], color, *font, size, angle, x, y, *str);
      if (err) return ThrowException(Exception::Error(String::New(err)));

      return args.This();
    }

    /**
     * Color Handling Functions
     */
    static Handle<Value> ColorAllocate (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(3);
      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Integer::New(gdImageColorAllocate(*im, r, g, b));
      return scope.Close(result);
    }

    static Handle<Value> ColorAllocateAlpha (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = Integer::New(gdImageColorAllocateAlpha(*im, r, g, b, a));
      return scope.Close(result);
    }

    static Handle<Value> ColorClosest (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Integer::New(gdImageColorClosest(*im, r, g, b));
      return scope.Close(result);
    }

    static Handle<Value> ColorClosestAlpha (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = Integer::New(gdImageColorClosestAlpha(*im, r, g, b, a));
      return scope.Close(result);
    }

    static Handle<Value> ColorClosestHWB (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Integer::New(gdImageColorClosestHWB(*im, r, g, b));
      return scope.Close(result);
    }

    static Handle<Value> ColorExact (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Integer::New(gdImageColorExact(*im, r, g, b));
      return scope.Close(result);
    }

    static Handle<Value> ColorResolve (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);

      Local<Number> result = Integer::New(gdImageColorResolve(*im, r, g, b));
      return scope.Close(result);
    }

    static Handle<Value> ColorResolveAlpha (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      OPT_INT_ARG(0, r, 0);
      OPT_INT_ARG(1, g, 0);
      OPT_INT_ARG(2, b, 0);
      OPT_INT_ARG(3, a, 100);

      Local<Number> result = Integer::New(gdImageColorResolveAlpha(*im, r, g, b, a));
      return scope.Close(result);
    }

    static Handle<Value> ColorsTotalGetter (Local<String> property, const AccessorInfo& info)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      Local<Number> result = Integer::New(gdImageColorsTotal(im->operator gdImagePtr()));
      return scope.Close(result);
    }

    static Handle<Value> Red (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Integer::New(gdImageRed(im->operator gdImagePtr(), color));
      return scope.Close(result);
    }

    static Handle<Value> Blue (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Integer::New(gdImageBlue(im->operator gdImagePtr(), color));
      return scope.Close(result);
    }

    static Handle<Value> Green (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);

      Local<Number> result = Integer::New(gdImageGreen(im->operator gdImagePtr(), color));
      return scope.Close(result);
    }

    static Handle<Value> InterlaceGetter (Local<String> property, const AccessorInfo& info)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      bool interlaced = (gdImageGetInterlaced(im->operator gdImagePtr()) != 0);

      return interlaced?True():False();
    }

    static void InterlaceSetter (Local<String> property, Local<Value> value, const AccessorInfo& info)
    {
      Image *im = ObjectWrap::Unwrap<Image>(info.This());

      if (value->IsBoolean()) {
        bool interlace = value->BooleanValue();

        gdImageInterlace(*im, interlace?1:0);
      }
    }

    static Handle<Value> GetTransparent (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      Local<Number> result = Integer::New(gdImageGetTransparent(im->operator gdImagePtr()));
      return scope.Close(result);
    }

    static Handle<Value> ColorDeallocate (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      gdImageColorDeallocate(*im, color);

      return args.This();
    }

    static Handle<Value> ColorTransparent (const Arguments &args)
    {
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_INT_ARG(0, color);
      gdImageColorTransparent(*im, color);

      return args.This();
    }

    /**
     * Copying and Resizing Functions
     */
    static Handle<Value> Copy (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> CopyResized (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> CopyResampled (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> CopyRotated (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> CopyMerge (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> CopyMergeGray (const Arguments &args)
    {
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

      return args.This();
    }

    static Handle<Value> PaletteCopy (const Arguments &args)
    {
      REQ_IMG_ARG(0, dest);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImagePaletteCopy(*dest, *im);

      return args.This();
    }

    static Handle<Value> SquareToCircle (const Arguments &args)
    {
      REQ_INT_ARG(0, radius);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSquareToCircle(*im, radius);

      return args.This();
    }

    static Handle<Value> Sharpen (const Arguments &args)
    {
      REQ_INT_ARG(0, pct);

      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      gdImageSquareToCircle(*im, pct);

      return args.This();
    }

    /**
     * Miscellaneous Functions
     */
    static Handle<Value> Compare (const Arguments &args)
    {
      HandleScope scope;
      Image *im = ObjectWrap::Unwrap<Image>(args.This());

      REQ_ARGS(1);
      if (!args[0]->IsObject())
        return ThrowException(Exception::TypeError(String::New("Argument 0  must be an image")));

      Local<Object> obj = Local<Object>::Cast(args[0]);
      Image *im2 = ObjectWrap::Unwrap<Image>(obj);

      Local<Number> result = Integer::New(gdImageCompare(*im, *im2));

      return scope.Close(result);
    }
  };
};

Persistent<FunctionTemplate> Gd::Image::constructor_template;

extern "C"
void init (Handle<Object> target)
{
  HandleScope scope;
  Gd::Init(target);
}
