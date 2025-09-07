FROM ubuntu:22.04

RUN apt-get update && apt-get install -y build-essential cmake gdb

WORKDIR /app
COPY . /app

CMD ["/bin/bash"]