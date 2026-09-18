FROM ubuntu:noble
LABEL org.opencontainers.image.title="marinholab_sas_core build environment"
LABEL authors="murilomarinho"
SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND=noninteractive

# Build tooling, C++ dependencies and Debian packaging tools.
# libdqrobotics (headers + shared library) is not in the Ubuntu repositories;
# it is provided by the DQ Robotics PPA (noble, amd64/arm64).
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential g++ cmake make pkg-config \
        libeigen3-dev \
        software-properties-common ca-certificates \
        dpkg-dev debhelper devscripts \
    && rm -rf /var/lib/apt/lists/*

RUN add-apt-repository -y ppa:dqrobotics-dev/development \
    && apt-get update \
    && apt-get install -y --no-install-recommends libdqrobotics \
    && rm -rf /var/lib/apt/lists/*

# The dqrobotics headers use <Eigen/Dense> (without the eigen3/ prefix);
# provide the conventional include path for them.
RUN ln -s /usr/include/eigen3/Eigen /usr/include/Eigen

WORKDIR /opt/sas_cpp
COPY . /opt/sas_cpp

CMD ["bash", "/opt/sas_cpp/docker/build.sh"]
