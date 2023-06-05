# Map /usr/src to /Users/vincentb/Projects/node-gd
# docker run -it -v $(pwd):/usr/src y-a-v-a:node-gd bash
FROM node:18

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

ENTRYPOINT []

CMD ["bash"]
