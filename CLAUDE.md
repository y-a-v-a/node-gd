# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is node-gd, a Node.js binding for the libgd C graphics library. It provides high-level JavaScript APIs for creating, manipulating, and saving images in various formats (PNG, JPEG, GIF, BMP, TIFF, WebP, etc.).

## Key Architecture

### Core Components

- **Native C++ binding** (`src/`) - Contains the core N-API wrapper around libgd
  - `node_gd.cc` - Main implementation with all image manipulation functions
  - `node_gd.h` - Header file with class definitions and macros
  - `addon.cc` - Module initialization
  - `node_gd_workers.cc` - Async worker implementations for file I/O

- **JavaScript wrapper** (`lib/`) - Adds convenience methods and Promise support
  - `node-gd.js` - Main wrapper that adds format-specific convenience functions
  - `bindings.js` - Handles the native binding loading
  - `GifAnim.js` - Animated GIF functionality

- **Entry point** (`index.js`) - Simple module setup that exports the main API

### Build System

- Uses `node-gyp` for compiling the native C++ code
- `binding.gyp` - Defines build configuration with conditional compilation based on available libraries
- `util.sh` - Shell script that detects available libgd features (AVIF, HEIF, TIFF, etc.)

## Common Commands

### Development
```bash
# Build the native module
npm run rebuild

# Clean build artifacts
npm run clean

# Install (triggers rebuild)
npm install
```

### Testing
```bash
# Run all tests (builds first via pretest)
npm test

# Just build without testing
npm run pretest
```

### Docker Development
```bash
# Build Docker image
npm run docker-build

# Run in Docker
npm run docker-run
```

## Development Notes

### Image Types
- **Palette images** - Created with `gd.create()`, max 256 colors, white background
- **True color images** - Created with `gd.createTrueColor()`, millions of colors, black background

### Memory Management
- All images must be explicitly destroyed with `image.destroy()` to free C-level memory
- Failing to destroy images will cause memory leaks

### Async vs Sync
- Version 2.x removed sync functions (`createSync`, `createTrueColorSync`)
- All file I/O operations return Promises and use N-API AsyncWorkers
- Drawing operations are synchronous

### Platform Support
- **Does not build on Windows** - explicitly excluded in package.json
- Requires libgd development headers to be installed on the system
- Uses pkg-config to find libgd

### Testing
- Uses Mocha with ES modules
- Tests are in `test/` directory with `.test.mjs` extension
- Test fixtures in `test/fixtures/`
- Output images saved to `test/output/`

### Conditional Features
The build system detects available libgd features and compiles accordingly:
- AVIF support (requires libavif)
- HEIF support (requires libheif) 
- TIFF support (requires libtiff)
- WebP support (requires libwebp)
- FreeType font support
- Fontconfig support

Check available features with `gd.getGDVersion()` and feature flags like `gd.GD_TIFF`.