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
- Node.js `^22.22.2 || ^24.15.0 || >=26.0.0`, the same range as `node-gyp` 13.
- `node-gyp` is a dev dependency. Users building from source get it from their package manager.
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
- Linux prebuilds are built for glibc (Debian bookworm) and musl (Alpine) on x64 and arm64 by `.github/scripts/build-linux.sh`, in containers started with `docker run`. JavaScript actions don't run in Alpine containers on arm64 runners, so don't move these builds to a job `container:`.
- Linux prebuild files are tagged with their libc (`node-gd.glibc.node`, `node-gd.musl.node`), macOS files are `node-gd.node`.
- The `test-package` job installs the packed package on systems with only the libgd runtime library (`.github/scripts/test-package.sh`). `.github/scripts/verify-prebuild.mjs` checks that the prebuild, not `build/Release`, is loaded and works.
- Local prebuild test:
  1. `npm run prebuild`
  2. `npm pack`
  3. In a dummy project, `npm install /path/to/node-gd-*.tgz`

## Release checklist
- Ensure prebuild workflow ran on release publish.
- Verify `prebuilds/` exists in the npm tarball (`npm pack` + `tar -tf`).
- Publishing to npm uses trusted publishing (OIDC) from the `collect-prebuilds` job in `prebuild.yml`. There is no npm token.
- The release tag must be `v` plus the `package.json` version, for example `v4.0.0`.
- The trusted publisher on npmjs.com is configured for `y-a-v-a/node-gd` and the workflow file `prebuild.yml`. Renaming the file breaks publishing.

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
