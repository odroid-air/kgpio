# KArchive

Reading, creating, and manipulating gpio pins through Linux' sysfs interface

## Introduction

KGpio provides classes for easy reading, creation and manipulation of
GPIO pins through the Linux kernel's sysfs interface found in /sys/class/gpio.

KGpio implements the protocol described at
https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
as a Qt-style library.


## Usage

* Create and receive a GpioPin through KGpio::pin(id)
* ...


To build the code on the ODROID C1 with Netrunner Core running:

edit /etc/apt/source.list to contain the following lines:

deb http://snapshot.debian.org/archive/debian/20161211/ testing main contrib non-free
deb http://security.debian.org/ testing/updates main contrib non-free
deb http://snapshot.debian.org/archive/debian/20161211/ testing-updates main contrib non-free

deb-src http://snapshot.debian.org/archive/debian/20161211/ testing main contrib non-free
deb-src http://security.debian.org/ testing/updates main contrib non-free
deb-src http://snapshot.debian.org/archive/debian/20161211/ testing-updates main contrib non-free

then install the build dependencies for a tier1 library, installs a bit too much, but in any way
sufficient for our purposes:
 sudo apt-get build-dep libkf5archive5

make the gpio pins writable (TODO: use group permissions here):

  vim /etc/udev/rules.d/91-gpio.rules

SUBSYSTEM=="subsystem", KERNEL=="gpio*",  ACTION=="add", PROGRAM="/bin/sh -c 'chown -R sebas:users /sys%p'"
SUBSYSTEM=="gpio", KERNEL=="gpio*",  ACTION=="add", PROGRAM="/bin/sh -c 'chown -R sebas:users /sys%p/*'"
