FROM docker/debian:latest
RUN apt-get update && apt-get install -y g++ git make libqt4-core libqt4-dbus libqt4-test libqt4-dev
CMD cd /opt && git clone 
