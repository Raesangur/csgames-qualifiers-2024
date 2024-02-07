FROM debian:bookworm

RUN apt update && apt install -y make g++

WORKDIR /src

COPY . .

ENTRYPOINT [ "/bin/sh" ]
