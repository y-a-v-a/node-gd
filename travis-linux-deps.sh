#!/bin/bash
set -ev
lsb_release -a

sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"
sudo -E apt-get -qy --no-install-suggests --no-install-recommends --force-yes install libgd2-xpm-dev g++-4.8
npm install
