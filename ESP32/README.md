HikeBuddy Firmware
=====================

Debian prerequisites
--------------------

```
sudo apt-get install libncurses5-dev flex bison gperf python-serial libffi-dev libsdl2-dev libmbedtls-dev perl
```

Compiling and flashing
----------------------

```
git submodule update --init --recursive
source set_env.sh
make defconfig
make -j5
```

Optional configuration
-------------
```
make menuconfig
```

Interacting via serial
----------------------
```
make monitor
```
