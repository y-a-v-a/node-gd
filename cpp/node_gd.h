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
#ifndef NODE_GD_H
#define NODE_GD_H

#include <nan.h>
#include <gd.h>

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

class Gd : public Nan::ObjectWrap {
public:
  static void Init(v8::Local<v8::Object> exports);

  class Image : public Nan::ObjectWrap {
  public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;

    static void Init(v8::Local<v8::Object> exports);
  private:
    static NAN_METHOD(New);

    Image(gdImagePtr image);
    ~Image();

    gdImagePtr _image;

    operator gdImagePtr () const { return _image; }

    /**
     * Destruction, Loading and Saving Functions
     */
    static NAN_METHOD(Destroy);
    static NAN_METHOD(Jpeg);
    static NAN_METHOD(JpegPtr);
    static NAN_METHOD(Gif);
    static NAN_METHOD(GifPtr);
    static NAN_METHOD(Png);
    static NAN_METHOD(PngPtr);
    static NAN_METHOD(WBMP);
    static NAN_METHOD(WBMPPtr);
#if HAS_LIBWEBP
    static NAN_METHOD(Webp);
    static NAN_METHOD(WebpPtr);
#endif
    static NAN_METHOD(Gd);
    static NAN_METHOD(GdPtr);
    static NAN_METHOD(Gd2);
    static NAN_METHOD(Gd2Ptr);
#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(Bmp);
    static NAN_METHOD(BmpPtr);
#endif

#if HAS_LIBTIFF
    static NAN_METHOD(Tiff);
    static NAN_METHOD(TiffPtr);
#endif

#if SUPPORTS_GD_2_1_1
    static NAN_METHOD(File);
    static NAN_METHOD(FileCallback);
#endif
    /**
     * Drawing Functions
     */
    static NAN_METHOD(SetPixel);
    static NAN_METHOD(Line);
    static NAN_METHOD(DashedLine);
    static NAN_METHOD(Polygon);
    static NAN_METHOD(OpenPolygon);
    static NAN_METHOD(FilledPolygon);
    static NAN_METHOD(Rectangle);
    static NAN_METHOD(FilledRectangle);
    static NAN_METHOD(Arc);
    static NAN_METHOD(FilledArc);
#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(Ellipse);
#endif

    static NAN_METHOD(FilledEllipse);
    static NAN_METHOD(FillToBorder);
    static NAN_METHOD(Fill);
    static NAN_METHOD(SetAntiAliased);
    static NAN_METHOD(SetAntiAliasedDontBlend);
    static NAN_METHOD(SetBrush);
    static NAN_METHOD(SetTile);
    static NAN_METHOD(SetStyle);
    static NAN_METHOD(SetThickness);
    static NAN_METHOD(AlphaBlending);
    static NAN_METHOD(SaveAlpha);
    static NAN_METHOD(SetClip);
    static NAN_METHOD(GetClip);
#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(SetResolution);
#endif

    /**
     * Query Functions
     */
    static NAN_METHOD(GetPixel);
    static NAN_METHOD(GetTrueColorPixel);
    // This is implementation of the PHP-GD specific method imagecolorat
    static NAN_METHOD(ImageColorAt);
    static NAN_METHOD(GetBoundsSafe);
    static NAN_PROPERTY_GETTER(WidthGetter);
    static NAN_PROPERTY_GETTER(HeightGetter);
    static NAN_PROPERTY_GETTER(TrueColorGetter);
    /**
     * Font and Text Handling Funcitons
     */
    static NAN_METHOD(StringFTBBox);
    static NAN_METHOD(StringFT);
    static NAN_METHOD(StringFTEx);
    static NAN_METHOD(StringFTCircle);
    /**
     * Color Handling Functions
     */
    static NAN_METHOD(ColorAllocate);
    static NAN_METHOD(ColorAllocateAlpha);
    static NAN_METHOD(ColorClosest);
    static NAN_METHOD(ColorClosestAlpha);
    static NAN_METHOD(ColorClosestHWB);
    static NAN_METHOD(ColorExact);
    static NAN_METHOD(ColorResolve);
    static NAN_METHOD(ColorResolveAlpha);
    static NAN_PROPERTY_GETTER(ColorsTotalGetter);
    static NAN_METHOD(Red);
    static NAN_METHOD(Blue);
    static NAN_METHOD(Green);
    static NAN_METHOD(Alpha);
    static NAN_PROPERTY_GETTER(InterlaceGetter);
    static NAN_SETTER(InterlaceSetter);
    static NAN_METHOD(GetTransparent);
    static NAN_METHOD(ColorDeallocate);
    static NAN_METHOD(ColorTransparent);
#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(ColorReplace);
    static NAN_METHOD(ColorReplaceThreshold);
    static NAN_METHOD(ColorReplaceArray);
    static NAN_METHOD(GrayScale);
    static NAN_METHOD(GaussianBlur);
    static NAN_METHOD(Negate);
    static NAN_METHOD(Brightness);
    static NAN_METHOD(Contrast);
    static NAN_METHOD(SelectiveBlur);
    static NAN_METHOD(FlipHorizontal);
    static NAN_METHOD(FlipVertical);
    static NAN_METHOD(FlipBoth);
    static NAN_METHOD(Crop);
    static NAN_METHOD(CropAuto);
    static NAN_METHOD(CropThreshold);
#endif

#if SUPPORTS_GD_2_1_1
    static NAN_METHOD(Emboss);
#endif

    /**
     * Copying and Resizing Functions
     */
    static NAN_METHOD(Copy);
    static NAN_METHOD(CopyResized);
    static NAN_METHOD(CopyResampled);
    static NAN_METHOD(CopyRotated);
    static NAN_METHOD(CopyMerge);
    static NAN_METHOD(CopyMergeGray);
    static NAN_METHOD(PaletteCopy);
    static NAN_METHOD(SquareToCircle);
    static NAN_METHOD(Sharpen);
    static NAN_METHOD(CreatePaletteFromTrueColor);
    static NAN_METHOD(TrueColorToPalette);

#if SUPPORTS_GD_2_1_0
    static NAN_METHOD(PaletteToTrueColor);
    static NAN_METHOD(ColorMatch);
#endif

    static NAN_METHOD(GifAnimBegin);
    static NAN_METHOD(GifAnimAdd);
    static NAN_METHOD(GifAnimEnd);
    /**
     * Miscellaneous Functions
     */
    static NAN_METHOD(Compare);
  };

private:
  static NAN_METHOD(ImageCreate);
  static NAN_METHOD(ImageCreateSync);
  static NAN_METHOD(ImageCreateTrueColor);
  static NAN_METHOD(ImageCreateTrueColorSync);

  static NAN_METHOD(CreateFromJpeg);
  static NAN_METHOD(CreateFromJpegPtr);
  static NAN_METHOD(CreateFromPng);
  static NAN_METHOD(CreateFromPngPtr);
  static NAN_METHOD(CreateFromGif);
  static NAN_METHOD(CreateFromGifPtr);
  static NAN_METHOD(CreateFromGd2);
  static NAN_METHOD(CreateFromGd2Ptr);
  static NAN_METHOD(CreateFromWBMP);
  static NAN_METHOD(CreateFromWBMPPtr);
#if HAS_LIBWEBP
  static NAN_METHOD(CreateFromWebp);
  static NAN_METHOD(CreateFromWebpPtr);
#endif

#if SUPPORTS_GD_2_1_0
  static NAN_METHOD(CreateFromBmp);
  static NAN_METHOD(CreateFromBmpPtr);
#endif
#if HAS_LIBTIFF
  static NAN_METHOD(CreateFromTiff);
  static NAN_METHOD(CreateFromTiffPtr);
#endif

#if SUPPORTS_GD_2_1_1
  static NAN_METHOD(CreateFromFile);
#endif

  static NAN_METHOD(CreateFromGd2Part);
  static NAN_METHOD(CreateFromGd2PartPtr);
  static NAN_METHOD(TrueColor);
  static NAN_METHOD(TrueColorAlpha);
  static NAN_METHOD(GdVersionGetter);
};

#endif
