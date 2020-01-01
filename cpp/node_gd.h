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
    gdImagePtr _image;

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
    Napi::Value FileCallback(const Napi::CallbackInfo& info);
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
  static Napi::Value ImageCreate(const Napi::CallbackInfo& info);
  static Napi::Value ImageCreateTrueColor(const Napi::CallbackInfo& info);

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

#if SUPPORTS_GD_2_1_1
  static Napi::Value CreateFromFile(const Napi::CallbackInfo& info);
#endif

  static Napi::Value CreateFromGd2Part(const Napi::CallbackInfo& info);
  static Napi::Value CreateFromGd2PartPtr(const Napi::CallbackInfo& info);
  static Napi::Value TrueColor(const Napi::CallbackInfo& info);
  static Napi::Value TrueColorAlpha(const Napi::CallbackInfo& info);
  static Napi::Value GdVersionGetter(const Napi::CallbackInfo& info);
};

#endif
