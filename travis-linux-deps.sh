#!/bin/bash
set -ev
lsb_release -a

sudo -E apt-get -qy --no-install-suggests --no-install-recommends --force-yes install libgd2-dev
npm install
