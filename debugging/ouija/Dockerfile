FROM debian:bookworm

RUN apt update && apt install -y make gcc

WORKDIR /src

COPY . .

ENTRYPOINT [ "/bin/sh" ]
