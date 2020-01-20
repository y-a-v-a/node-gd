# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# Unreleased - 2020

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


# 1.5.4 - 2018-02-06 (current)

### Fixed

- Fixed creating image from `String` or `Buffer`.

### Added

- Extended documentation for `gd.Image#crop()`

# 1.5.3 - 2018-01-21

### Fixed

- Fixed #59 where a value of `0` was considered out of range.
