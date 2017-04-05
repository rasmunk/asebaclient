FROM debian:latest
## If the update failed -> try again
RUN update=1; while test $update -ne 0; do apt-get update --fix-missing; update=$? ; done \
    && update=1; while test $update -ne 0; do apt-get install --no-install-recommends -y \
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
    cmake \
    nano; \
    update=$? ; done \
    && apt-get clean \
    && rm -r /var/lib/apt/lists/*

## Download and install asebaclient
RUN cd /opt \
    && git clone -b development --single-branch https://github.com/rasmunk/asebaclient.git \
    && cd asebaclient/build-script \
    && chmod +x generate-debug-release \
    && ./generate-debug-release \
    && rm -r /opt/*