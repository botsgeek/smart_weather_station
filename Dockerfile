# Use an official Ubuntu image as the base image
FROM ubuntu:20.04

# Install necessary dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    gcc-arm-none-eabi \
    gdb-multiarch \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/myapp



