FROM ubuntu:20.10

RUN apt-get update && DEBIAN_FRONTEND="noninteractive" apt-get install -yq uuid-dev build-essential cmake nodejs git npm golang

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]