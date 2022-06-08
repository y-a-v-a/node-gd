FROM node:14-bullseye

USER root

ENV HOME=/usr/src

RUN apt-get update && \
  apt-get install build-essential pkg-config python3 libgd-dev -y && \
  npm i -g npm && \
  npm i -g node-gyp && \
  mkdir $HOME/.cache && \
  chown -R node:node $HOME

USER node

WORKDIR $HOME

CMD  [ "sh" ]
