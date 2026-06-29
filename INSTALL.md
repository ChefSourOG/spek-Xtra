# Installation instructions

[[简体中文 Simplified Chinese]](./INSTALL-zh_CN.md)

## Windows

Download section offers two packages: a ZIP archive for x64 processors and a
ZIP archive for Arm64 processors. 32-bit Windows is not supported.

Download the ZIP archive, unpack it somewhere on your disk and run `spek-xtra.exe`.

## macOS

Download section offers two packages: a TGZ archive for Intel processors and a
TGZ archive for Apple Silicon. Spek requires macOS 10.5+.

Download and unpack the TGZ archive, then drag the Spek icon to Applications.

## Linux and other Unix-like systems

### Binary packages

Spek-Xtra
 * Debian (deb-multimedia): [spek-x](https://deb-multimedia.org/pool/main/s/spek-x-dmo/), Tsinghua mirror: [spek-x](https://mirrors.tuna.tsinghua.edu.cn/debian-multimedia/pool/main/s/spek-x-dmo/)

Original Spek (Outdated)
 * Arch: [spek](https://aur.archlinux.org/packages/spek/) and
   [spek-git](https://aur.archlinux.org/packages/spek-git/)
 * Debian: [spek](https://packages.debian.org/search?keywords=spek)
 * Fedora: [RPMFusion package](https://bugzilla.rpmfusion.org/show_bug.cgi?id=1718)
 * FreeBSD: [audio/spek](https://www.freshports.org/audio/spek/)
 * Gentoo: [media-sound/spek](https://packages.gentoo.org/packages/media-sound/spek)
 * Ubuntu: [spek](http://packages.ubuntu.com/search?keywords=spek)

### Building from the git repository

    git clone https://github.com/ChefSourOG/spek-Xtra.git
    cd spek-Xtra
    ./autogen.sh
    make

To build from git you need the GNU autotools plus wxWidgets and FFmpeg
packages. On Debian/Ubuntu install:

```bash
sudo apt install -y g++ make pkg-config autoconf automake libtool \
    intltool gettext autopoint libwxgtk3.2-dev wx-common \
    libavcodec-dev libavformat-dev
```

(On older distributions you may need `libwxgtk3.0-dev` instead of
`libwxgtk3.2-dev`.)

To start Spek, run:

    src/spek-xtra

Or install it with:

    sudo make install

