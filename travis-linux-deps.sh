#!/bin/bash
set -ev
lsb_release -a

sudo apt-get -qy --no-install-suggests --no-install-recommends --force-yes install libgd2-xpm-dev g++-4.8
npm install
