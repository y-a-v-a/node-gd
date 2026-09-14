# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# 3.2.0 - 2026-09-14 (current)

### Added
- Test coverage for drawing primitives, color functions, transformations and filters, copying and comparing images, encoding and decoding all supported formats, and `gd.GifAnim`.
- TypeScript types for `gd.GifAnim`.
- Documentation of `gd.GifAnim`.

### Changed
- `gd.Image#gifAnimEnd()` returns a `Buffer` with the end of the GIF data instead of `true`.
- `gd.Image#setBrush()` and `gd.Image#setTile()` copy the given image. Changes made to it afterwards no longer affect drawing, and it can be destroyed right away.
- `gd.Image#getPixel()` and `gd.Image#getTrueColorPixel()` return `0` for coordinates at or beyond the width or height. Previously `getTrueColorPixel()` on a palette image returned the color of palette index `0`.
- Methods taking an image argument throw a `TypeError` for anything that is not a `gd.Image`, and an `Error` for a destroyed image. This includes the previous frame of `gd.Image#gifAnimAdd()`.
- `gd.Image#colorReplaceArray()` and the polygon methods throw a `TypeError` for invalid arrays or values instead of a generic error.
- `gd.Image#cropAuto()` reports the valid mode range as 0 to 4 and rejects negative modes. Use `gd.Image#cropThreshold()` to crop by threshold.

### Fixed
- Crash when calling `red()`, `green()`, `blue()` or `alpha()` on a palette image with a value that is not a palette index. A `RangeError` is thrown instead.
- Crash when setting `interlace` on a destroyed image. Setting `interpolationId` above 30 no longer applies the value after throwing.
- Crash when passing a destroyed image to `copy()`, `compare()`, `colorMatch()` or `paletteCopy()`.
- Use after free when drawing with a brush or tile whose image was destroyed or garbage collected.
- Animated GIFs created with `gd.GifAnim` missing the trailing `0x3B` byte.
- `gd.GifAnim#end()` continuing after rejecting when called twice or when the file could not be written.
- Stray lines and style pixels when polygon points or style arrays contain invalid entries.
- Memory leaks in `crop()`, `colorReplaceArray()` and the polygon methods.
- Wrong argument index in the `stringFTEx()` error and missing argument count check in `stringFTCircle()`.
- Tests that passed without asserting anything when the expected error was not thrown.
- Outdated GIF animation documentation and TypeScript declarations.
- Release workflow flattening prebuilds into a single directory, which left only one unusable prebuild in the npm package.

# 3.1.0 - 2026-01-30

### Added
- Prebuilt binary packaging via prebuildify, with prebuilds bundled in the npm package.
- HEIF/AVIF/WebP TypeScript API surface (open/create/save methods) and test coverage.
- Docker test helper script and expanded documentation.

### Changed
- Dropped support for Node.js 18; now requires Node.js 20+ (CI targets 20/22/24).
- Install script uses `node-gyp-build` with fallback to `node-gyp rebuild`.
- CI workflows updated for prebuilds and modern macOS runners; Linux prebuilds moved to Debian.
- Dependency updates (node-addon-api, node-gyp, mocha/chai, and security-related transitive bumps).

### Fixed
- Avoid overriding existing image `save*` helpers (issue #84).
- More robust error handling for image decoding and HEIF/AVIF save paths.

# 3.0.0 - 2023-06-05

- Only support libgd 2.3.0 and up

### Added

- A lot of error messages with a sane message
- Added gdImageColorExactAlpha
- Added gdImagePixelate
- Added getter for interpolation_id
- Added gdImageScale
- Added gdImageSetInterpolationMethod
- Added resX and resY getters
- Added gdImageRotateInterpolated
- Added homebrew paths e.g. `/opt/homebrew/include`

### Updated

- Updated dependencies to latest versions
- Updated Github actions dependencies
- C++ code formatting

### Removed

- Removed gd and gd2 image formats as libgd turned them off by default since 2.3.0
- Removed many libgd version conditions since decision is made to support libgd 2.3.0 and up only per node-gd 3.x.x

# 2.1.1 - 2020-09-10

### Added

- TypeScript types file, as mentioned in #81 (thanks to [vladislav805](https://github.com/vladislav805))

### Fixed

- Package size of eventual npm package tgz file by being more specific about what it should contain in `package.json`'s `file` property.

# 2.1.0 - 2020-06-19

### Added

- Support for libgd 2.3.0

### Fixed

- Tests with regard to font boundary coordinates

# 2.0.1 - 2020-05-26

### Added

- Added `files` property in `package.json`.
- Added test files to `files` property

### Changed

- Upgraded dependencies in package.json
- Typo fixed in documentation (thanks [gabrieledarrigo](https://github.com/gabrieledarrigo))
- Updated test to create `output` directory if not present

### Removed

- Remove `.npmignore` in favour of `files` property in `package.json`.

# 2.0.0 - 2020-01-19

### Added

- Added multiple `AsyncWorker` classes.
- Added a license file.
- Added a changelog file.
- Added a lot of new tests.

### Changed

- Changed the workging of Gif animation creation.
- Moved from Nan to Napi.
- Changed `gd.create` and `gd.createTruecolor` and let them return a `Promise`.
- Moved macros to header file.
- Updated documentation
- Changed custom prototype functions unwritable functions with a [proper name](https://stackoverflow.com/questions/9479046/is-there-any-non-eval-way-to-create-a-function-with-a-runtime-determined-name/9479081#9479081)

### Removed

- No longer supports image creation from `String`, only from `Buffer` from now on.

### Breaking

- Dropped support for Node <6.x

# 1.5.4 - 2018-02-06

### Fixed

- Fixed creating image from `String` or `Buffer`.

### Added

- Extended documentation for `gd.Image#crop()`

# 1.5.3 - 2018-01-21

### Fixed

- Fixed #59 where a value of `0` was considered out of range.
