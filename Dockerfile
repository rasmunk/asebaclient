FROM debian:latest
## If the update failed -> try again
RUN update=1; while test $update -ne 0; do apt-get update --fix-missing; update=$? ; done \
    && update=1; while test $update -ne 0; do apt-get install -y \
    g++ \
    git \
    make \
    libqt4-core \
    libqt4-dbus \
    libqt4-test \
    libqt4-dev \
    curl \
    ca-certificates \
    ssh-client \
    rsync \
    nano; \
    update=$? ; done \
    && apt-get clean \
    && rm -r /var/lib/apt/lists/*

ENV CMAKE_NAME "cmake-3.7.2-Linux-x86_64"
ENV CMAKE_FILE "${CMAKE_NAME}.sh"
ENV CMAKE_URL "https://cmake.org/files/v3.7/${CMAKE_FILE}"

## Get and install cmake
RUN cd /opt \
    && curl -O "$CMAKE_URL" \
    && chmod +x "$CMAKE_FILE" \
    && (echo y) | ./"$CMAKE_FILE" \
    && rsync -a "/opt/${CMAKE_NAME}/" /usr/local/ \
    && rm -r /opt/*

## Download and install asebaclient
RUN cd /opt \
    && git clone -b development --single-branch https://github.com/rasmunk/asebaclient.git \
    && cd asebaclient/build-script \
    && chmod +x generate-debug-release \
    && ./generate-debug-release \
    && rm -r /opt/*