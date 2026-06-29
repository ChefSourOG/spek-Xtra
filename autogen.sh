#!/bin/sh
# Run this to generate all the initial makefiles, etc.

test -n "$srcdir" || srcdir=$(dirname "$0")
test -n "$srcdir" || srcdir=.
command -v autoreconf >/dev/null 2>&1 || {
  echo "autoreconf not found. Please install autoconf, automake, libtool, intltool, gettext and autopoint." >&2
  exit 1
}
(
  cd "$srcdir" &&
  touch config.rpath &&
  mkdir -p m4 &&
  # gettext >= 0.25 installs its m4 macros under share/gettext/m4 instead of
  # share/aclocal. Make sure aclocal can find them on modern distributions.
  GETTEXT_M4_DIRS=""
  for d in /usr/share/gettext/m4 \
           /usr/local/share/gettext/m4 \
           /opt/homebrew/opt/gettext/share/gettext/m4 \
           /opt/homebrew/opt/gettext/share/aclocal \
           /usr/local/opt/gettext/share/gettext/m4 \
           /usr/local/opt/gettext/share/aclocal \
           /mingw64/share/gettext/m4 \
           /usr/share/aclocal; do
    if [ -d "$d" ]; then
      GETTEXT_M4_DIRS="${GETTEXT_M4_DIRS:+$GETTEXT_M4_DIRS:}$d"
    fi
  done
  if [ -n "$GETTEXT_M4_DIRS" ]; then
    ACLOCAL_PATH="$GETTEXT_M4_DIRS${ACLOCAL_PATH:+:$ACLOCAL_PATH}"
    export ACLOCAL_PATH
  fi &&
  AUTOPOINT='intltoolize --automake --copy --force' autoreconf -fiv
) || exit
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
