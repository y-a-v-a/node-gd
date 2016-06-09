#!/bin/bash
set -ev

brew update
brew outdated gd pkg-config || brew upgrade gd pkg-config
npm install
