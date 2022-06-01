FROM node:14-bullseye

USER root

ENV HOME=/usr/src

RUN apt-get update && \
  apt-get install build-essential libgd-dev pkg-config python3 -y && \
  npm i -g npm && \
  npm i -g node-gyp && \
  mkdir $HOME/.cache && \
  chown -R node:node $HOME

USER node

WORKDIR $HOME

CMD  [ "sh" ]
