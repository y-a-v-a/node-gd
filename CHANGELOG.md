# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# Unreleased

This will be released as 5.0.0.

### Breaking
- Promises returned by node-gd are rejected with an `Error` instead of a string (issue #50). The message is the string that was used before, so use `error.message`. This applies to opening and reading files (`gd.openPng()`, `gd.createFromPng()`, `gd.openFile()` and the other formats), saving files (`gd.Image#png()`, `gd.Image#file()` and the other formats) and `gd.GifAnim#end()`.
- `gd.createFromJpegPtr()`, `gd.createFromPngPtr()` and the other `gd.createFrom*Ptr()` functions throw an `Error` when the data can't be read in that format, for example "Cannot read PNG data", instead of returning `null` (issue #50).

### Fixed
- The TypeScript declarations of `gd.createFrom*Ptr()` return `gd.Image` instead of `Promise<gd.Image>`. These functions have always been synchronous.
- `gd.GD_XPM` not being set when libgd supports XPM. The build looked for the library `xpm`, but libgd links `Xpm`.
- Importing node-gd when its native addon can't be loaded, for example because libgd is missing or there is no binary for the platform, throws an error with the reason. Previously it printed unrelated "Cannot find module" messages and ended the process.
- Animated GIFs made with `gd.GifAnim` from true color images showing wrong colors, mostly black (issue #85). libgd converts each true color frame to its own palette but only writes it with a local color map, so true color frames now use a local color map by default. Pass `localColorMap: 0` to get the previous behavior.
- Node.js crashing with a segmentation fault when libgd can't encode an image, for example `webpPtr()`, `avifPtr()` or `heifPtr()` on a palette image, or `heifPtr()` without a HEIF encoder. The `*Ptr()` methods now throw an `Error` with the message "Cannot encode image in this format", and the `save*()` methods reject with it.

# 4.1.0 - 2026-11-10 (current)

### Added
- Prebuilt binaries for Linux arm64, and for Linux x64 and arm64 with musl (Alpine). The musl binaries are built on Alpine 3.24 and need its `libgd` package.

### Changed
- Linux prebuilt binaries are named after their C library, `node-gd.glibc.node` and `node-gd.musl.node`, so `node-gyp-build` loads the right one.
- The release workflow installs the npm package on Debian and Alpine, x64 and arm64, with only the libgd runtime library and no build tools, and only publishes when that works.

# 4.0.1 - 2026-11-09

### Fixed
- Node.js being killed by macOS when loading node-gd on Apple Silicon. The `darwin-arm64` prebuilt binary had an invalid code signature, because the binaries are stripped after the linker signs them. The macOS binaries are now signed again after stripping.
- The release workflow loading the freshly compiled binary instead of the prebuilt one when testing prebuilds, which hid the problem above.
- README claiming prebuilt binaries for Linux arm64 and that no system dependencies are needed. The prebuilt binaries need the libgd shared library.

# 4.0.0 - 2026-11-09

This is the first release on npm since 3.0.0. Versions 3.1.0 and 3.2.0 were only released on GitHub, so when you upgrade from 3.0.0 on npm, also read the changes of 3.1.0 and 3.2.0 below. Among them, 3.2.0 changes the return value of `gd.Image#gifAnimEnd()` and makes methods taking an image argument throw a `TypeError` for anything that is not a `gd.Image`.

### Changed
- `node-gyp` is no longer installed with node-gd. When no prebuilt binary is available, the build uses the `node-gyp` your package manager provides. Development builds use `node-gyp` 13.1.0.
- Supported Node.js versions changed from `>=20` to `^22.22.2 || ^24.15.0 || >=26.0.0`, the range `node-gyp` 13 requires. Yarn 4 installs the latest `node-gyp` for packages that don't list it, and that version no longer supports these older Node.js versions. Node.js 20, 21, 23 and 25 are no longer supported, nor are versions of 22 before 22.22.2 and of 24 before 24.15.0.
- Prebuilt binaries are no longer built and tested on Node.js 20.
- `mocha` updated to 12.0.3 for the test suite.

### Security
- Installing node-gd no longer installs `node-gyp`, and with it the vulnerable `glob` and `minimatch` versions `node-gyp` 11 depended on (GHSA-3g3m-2fjr-6pw4, GHSA-g3hv-75wg-m249).
- `mocha` 12.0.3 still requires the affected `glob` 13 and `minimatch` 10, so an npm override makes it use `glob` 14 and `minimatch` 11 instead. The override only applies to `mocha`, which is only installed for development. Remove it once `mocha` depends on these versions itself.
- Releases are published to npm from GitHub Actions with npm trusted publishing instead of an npm token, and get a provenance statement.

# 3.2.0 - 2026-09-14

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
