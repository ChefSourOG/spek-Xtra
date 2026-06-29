#!/usr/bin/env bash
set -e
cd /c/Users/jpisa/Desktop/spek-x_fork/spek-Xtra
export PATH=/mingw64/bin:/usr/bin:$PATH
make -j4 "$@"
