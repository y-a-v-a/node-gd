declare namespace gd {

    type Ptr = Buffer | BufferSource;

    // Creating and opening graphic images

    function create(width: number, height: number): Promise<gd.Image>;

    function createTrueColor(width: number, height: number): Promise<gd.Image>;

    function createSync(width: number, height: number): gd.Image;

    function createTrueColorSync(width: number, height: number): gd.Image;

    function openJpeg(path: string): Promise<gd.Image>;

    function createFromJpeg(path: string): Promise<gd.Image>;

    function createFromJpegPtr(data: Ptr): Promise<gd.Image>;

    function openPng(path: string): Promise<gd.Image>;

    function createFromPng(path: string): Promise<gd.Image>;

    function createFromPngPtr(data: Ptr): Promise<gd.Image>;

    function openGif(path: string): Promise<gd.Image>;

    function createFromGif(path: string): Promise<gd.Image>;

    function createFromGifPtr(data: Ptr): Promise<gd.Image>;

    function openGd2(path: string): Promise<gd.Image>;

    function createFromGd2(path: string): Promise<gd.Image>;

    function createFromGd2Ptr(data: Ptr): Promise<gd.Image>;

    function openGd2Part(path: string, srcX: number, srcY: number, width: number, height: number): Promise<gd.Image>;

    function createFromGd2Part(path: string, srcX: number, srcY: number, width: number, height: number): Promise<gd.Image>;

    function createFromGd2PartPtr(data: Ptr, srcX: number, srcY: number, width: number, height: number): Promise<gd.Image>;

    function openWBMP(path: string): Promise<gd.Image>;

    function createFromWBMP(path: string): Promise<gd.Image>;

    function createFromWBMPPtr(data: Ptr): Promise<gd.Image>;

    function openBmp(path: string): Promise<gd.Image>;

    function createFromBmp(path: string): Promise<gd.Image>;

    function createFromBmpPtr(data: Ptr): Promise<gd.Image>;

    function openTiff(path: string): Promise<gd.Image>;

    function createFromTiff(path: string): Promise<gd.Image>;

    function createFromTiffPtr(data: Ptr): Promise<gd.Image>;

    function openFile(path: string): Promise<gd.Image>;

    function createFromFile(path: string): Promise<gd.Image>;

    type Color = number;

    function trueColor(red: number, green: number, blue: number): Color;

    function trueColorAlpha(red: number, green: number, blue: number, alpha: number): Color;

    function getGDVersion(): string;

    type Point = {
        x: number;
        y: number;
    };

    // Manipulating graphic images

    interface Image {
        // Image properties

        readonly width: number;

        readonly height: number;

        readonly trueColor: 0 | 1;

        readonly colorsTotal: number;

        interlace: boolean;

        destroy(): void;

        // Drawing

        setPixel(x: number, y: number, color: Color): gd.Image;

        line(x1: number, y1: number, x2: number, y2: number, color: Color): gd.Image;

        dashedLine(x1: number, y1: number, x2: number, y2: number, color: Color): gd.Image;

        polygon(array: Point[], color: Color): gd.Image;

        openPolygon(array: Point[], color: Color): gd.Image;

        filledPolygon(array: Point[], color: Color): gd.Image;

        rectangle(x1: number, y1: number, x2: number, y2: number, color: Color): gd.Image;

        filledRectangle(x1: number, y1: number, x2: number, y2: number, color: Color): gd.Image;

        arc(cx: number, cy: number, width: number, height: number, begin: number, end: number, color: Color): gd.Image;

        filledArc(cx: number, cy: number, width: number, height: number, begin: number, end: number, color: Color, style?: number): gd.Image;

        ellipse(cx: number, cy: number, width: number, height: number, color: Color): gd.Image;

        filledEllipse(cx: number, cy: number, width: number, height: number, color: Color): gd.Image;

        fillToBorder(x: number, y: number, border: number, color: Color): gd.Image;

        fill(x: number, y: number, color: Color): gd.Image;

        setAntiAliased(color: Color): gd.Image;

        setAntiAliasedDontBlend(color: Color, dontblend: boolean): gd.Image;

        setBrush(image): gd.Image;

        setTile(image): gd.Image;

        setStyle(array): gd.Image;

        setThickness(thickness: number): gd.Image;

        alphaBlending(blending: 0 | 1): gd.Image;

        saveAlpha(saveFlag: 0 | 1): gd.Image;

        setClip(x1: number, y1: number, x2: number, y2: number): gd.Image;

        getClip(): {
            x1: number;
            y1: number;
            x2: number;
            y2: number;
        };

        setResolution(res_x: number, res_y: number): gd.Image;

        // Query image information

        getPixel(x: number, y: number): Color;

        getTrueColorPixel(x: number, y: number): Color;

        imageColorAt(x: number, y: number): Color;

        getBoundsSafe(x: number, y: number): 0 | 1;

        // Font and text

        stringFTBBox(color: Color, font: string, size: number, angle: number, x: number, y: number, text: string): [number, number, number, number, number, number, number, number];

        stringFT(color: Color, font: string, size: number, angle: number, x: number, y: number, text: string): void;
        stringFT(color: Color, font: string, size: number, angle: number, x: number, y: number, text: string, boundingbox: boolean): [number, number, number, number, number, number, number, number];

        // Color handling

        colorAllocate(r: number, g: number, b: number): Color;

        colorAllocateAlpha(r: number, g: number, b: number, a: number): Color;

        colorClosest(r: number, g: number, b: number): Color;

        colorClosestAlpha(r: number, g: number, b: number, a: number): Color;

        colorClosestHWB(r: number, g: number, b: number): Color;

        colorExact(r: number, g: number, b: number): Color;

        colorResolve(r: number, g: number, b: number): Color;

        colorResolveAlpha(r: number, g: number, b: number, a: number): Color;

        red(r: number): number;

        green(g: number): number;

        blue(b: number): number;

        alpha(color: Color): number;

        getTransparent(): number;

        colorDeallocate(color: Color): gd.Image;

        // Color Manipulation

        colorTransparent(color: Color): gd.Image;

        colorReplace(fromColor: Color, toColor: Color): number;

        colorReplaceThreshold(fromColor: Color, toColor: Color, threshold: number): number;

        colorReplaceArray(fromColors: Color[], toColors: Color[]): number;

        // Effects

        grayscale(): gd.Image;

        gaussianBlur(): gd.Image;

        negate(): gd.Image;

        brightness(brightness: number): gd.Image;

        contrast(contrast: number): gd.Image;

        selectiveBlur(): gd.Image;

        emboss(): gd.Image;

        flipHorizontal(): gd.Image;

        flipVertical(): gd.Image;

        flipBoth(): gd.Image;

        crop(x: number, y: number, width: number, height: number): gd.Image;

        cropAuto(mode: AutoCrop): gd.Image;

        cropThreshold(color: Color, threshold: number): gd.Image;

        sharpen(pct: number): gd.Image;

        createPaletteFromTrueColor(ditherFlag: 0 | 1, colorsWanted: number): gd.Image;

        trueColorToPalette(ditherFlag: 0 | 1, colorsWanted: number): number;

        paletteToTrueColor(): 0 | 1;

        colorMatch(image: gd.Image): number;

        gifAnimBegin(anim: string, useGlobalColorMap: -1 | 0 | 1, loops: number): Uint8Array;

        gifAnimAdd(anim: string, localColorMap: number, leftOffset: number, topOffset: number, delay: number, disposal: number, prevFrame: gd.Image | null): boolean;

        gifAnimEnd(anim: string): boolean;

        // Copying and resizing

        copy(dest: gd.Image, dx: number, dy: number, sx: number, sy: number, width: number, height: number): gd.Image;

        copyResized(dest: gd.Image, dx: number, dy: number, sx: number, sy: number, dw: number, dh: number, sw: number, sh: number): gd.Image;

        copyResampled(dest: gd.Image, dx: number, dy: number, sx: number, sy: number, dw: number, dh: number, sw: number, sh: number): gd.Image;

        copyRotated(dest: gd.Image, dx: number, dy: number, sx: number, sy: number, sw: number, sh: number, angle: number): gd.Image;

        copyMerge(dest: gd.Image, dx: number, dy: number, sx: number, sy: number, width: number, height: number, pct: number): gd.Image;

        copyMergeGray(dest: gd.Image, dx: number, dy: number, sx: number, sy: number, width: number, height: number, pct: number): gd.Image;

        paletteCopy(dest: gd.Image): gd.Image;

        squareToCircle(radius: number): gd.Image;

        // Misc

        // Returns a bitmask of image comparsion flags
        // https://libgd.github.io/manuals/2.3.0/files/gd-c.html#gdImageCompare
        compare(image: gd.Image): number;

        // Saving graphic images

        savePng(path: string, level: number): Promise<boolean>;
        saveJpeg(path: string, quality: number): Promise<boolean>;
        saveGif(path: string): Promise<boolean>;
        saveGd(path: string): Promise<boolean>;
        saveGd2(path: string, chunkSize: number, format?: string): Promise<boolean>;
        saveWBMP(path: string, foreground: 0x000000 | 0xffffff | number): Promise<boolean>;
        saveBmp(path: string, compression: 0 | 1): Promise<boolean>;
        saveTiff(path: string): Promise<boolean>;

        png(path: string, level: number): Promise<boolean>;
        jpeg(path: string, quality: number): Promise<boolean>;
        gif(path: string): Promise<boolean>;
        gd(path: string): Promise<boolean>;
        gd2(path: string, chunkSize: number, format?: string): Promise<boolean>;
        wbmp(path: string, foreground: 0x000000 | 0xffffff | number): Promise<boolean>;
        bmp(path: string, compression: 0 | 1): Promise<boolean>;
        tiff(path: string): Promise<boolean>;

        file(path: string): Promise<boolean>;
    }

    export const enum AutoCrop {
        DEFAULT = 0,
        TRANSPARENT,
        BLACK,
        WHITE,
        SIDES,
        THRESHOLD,
    }

    export const enum Cmp {
        IMAGE = 1,
        NUM_COLORS = 2,
        COLOR = 4,
        SIZE_X = 8,
        SIZE_Y = 16,
        TRANSPARENT = 32,
        BACKGROUND = 64,
        INTERLACE = 128,
        TRUECOLOR = 256,
    }
}

export = gd;
