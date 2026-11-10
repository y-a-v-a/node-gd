#!/bin/sh
set -eu

if [ -f /etc/alpine-release ]; then
  echo "Alpine Linux $(cat /etc/alpine-release)"
  apk add --no-cache libgd
  libc=musl
else
  . /etc/os-release
  echo "$PRETTY_NAME"
  apt-get update
  apt-get install -y --no-install-recommends libgd3
  libc=glibc
fi

echo "Node.js $(node -v), npm $(npm -v), $(uname -m)"
if command -v cc >/dev/null 2>&1 || command -v gcc >/dev/null 2>&1; then
  echo "A C compiler is installed, the package test needs a system without build tools"
  exit 1
fi

find prebuilds -type f
npm pack --pack-destination /tmp

mkdir /tmp/app
cd /tmp/app
npm init -y >/dev/null
npm install /tmp/node-gd-*.tgz

node /work/.github/scripts/verify-prebuild.mjs /tmp/app/node_modules/node-gd "$libc"
