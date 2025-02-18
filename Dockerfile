# 20.04
FROM --platform=linux/amd64 ubuntu:20.04

WORKDIR /app

ARG DEBIAN_FRONTEND=noninteractive


RUN apt update && \
    apt install -y wget build-essential

# Install GCC 13.2 rel1 (recommended by Himax/Seeed)
RUN mkdir -p /opt/gcc-13.2 && \
    cd /opt/gcc-13.2 && \
    wget --progress=bar:force:noscroll https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz && \
    tar xf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz && \
    rm -rf arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz

# Add to PATH
ENV PATH="${PATH}:/opt/gcc-13.2/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/"
