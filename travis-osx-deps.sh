#!/bin/bash
set -ev

brew update
brew outdated gd || brew upgrade gd
npm install
