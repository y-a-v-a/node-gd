# AGENTS.md

Short, consolidated instructions for working on this repo.

## Repo overview
- Node.js native addon for libgd (Node-API).
- Prebuilds are bundled into the npm package under `prebuilds/`.
- Bindings load via `node-gyp-build` in `lib/bindings.js`.

## Architecture
- **Native C++ binding** (`src/`)
  - `node_gd.cc` main implementation
  - `node_gd.h` class definitions and macros
  - `addon.cc` module initialization
  - `node_gd_workers.cc` async workers for file I/O
- **JavaScript wrapper** (`lib/`)
  - `node-gd.js` convenience methods and Promise support
  - `bindings.js` native binding loader
  - `GifAnim.js` animated GIF support
- **Entry point** (`index.js`) exports the main API
- **Build system**: `node-gyp`, `binding.gyp`, and `util.sh` feature detection

## Prereqs
- Node.js 20+.
- System libgd and build tools.
  - macOS: `brew install pkg-config gd`
  - Debian/Ubuntu: `apt-get install build-essential pkg-config libgd-dev`

## Install
- `npm ci` (or `npm install`) in repo root.

## Build
- `npm run rebuild` (node-gyp rebuild).
- `npm run prebuild` (creates `prebuilds/` for current platform).

## Tests
- `npm test`

## Prebuilds (CI model)
- CI builds per OS/arch and bundles `prebuilds/` into npm publish.
- Local prebuild test:
  1. `npm run prebuild`
  2. `npm pack`
  3. In a dummy project, `npm install /path/to/node-gd-*.tgz`

## Release checklist
- Ensure prebuild workflow ran on release publish.
- Verify `prebuilds/` exists in the npm tarball (`npm pack` + `tar -tf`).
- Publish uses `NPM_TOKEN` secret.

## Development notes
- **Image types**:
  - Palette images via `gd.create()` (max 256 colors, white background).
  - True color images via `gd.createTrueColor()` (millions of colors, black background).
- **Memory**: Always call `image.destroy()` to free C-level memory.
- **Async vs sync**: File I/O is async (Promises); drawing is sync.
- **Platform**: Windows is not supported; requires libgd headers.
- **Testing**: Mocha with ES modules; tests in `test/*.test.mjs`, fixtures in `test/fixtures/`.
- **Conditional features**: AVIF/HEIF/TIFF/WebP/FreeType/Fontconfig via `util.sh` detection.
  - Check runtime support with `gd.getGDVersion()` and feature flags like `gd.GD_TIFF`.
