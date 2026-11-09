#!/bin/sh
set -eu

if [ -f /etc/alpine-release ]; then
  echo "Alpine Linux $(cat /etc/alpine-release)"
  apk add --no-cache build-base python3 pkgconf gd-dev \
    libpng-dev libjpeg-turbo-dev libwebp-dev tiff-dev libheif-dev libavif-dev \
    libxpm-dev freetype-dev fontconfig-dev
  libc=musl
else
  . /etc/os-release
  echo "$PRETTY_NAME"
  apt-get update
  apt-get install -y build-essential pkg-config libgd-dev \
    libheif-dev libavif-dev libtiff-dev libwebp-dev \
    libfontconfig1-dev libfreetype6-dev libpng-dev libjpeg-dev \
    libaom-dev libdav1d-dev
  libc=glibc
fi

echo "Node.js $(node -v), $(uname -m)"
echo "gdlib libraries: $(pkg-config --static --libs-only-l gdlib)"

npm ci
npm run rebuild
npm test

npx prebuildify --napi --strip --tag-libc

PREBUILDS_ONLY=1 node .github/scripts/verify-prebuild.mjs . "$libc"
