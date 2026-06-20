# Spek-X Agent Guide

This file is for AI coding agents working on the Spek-X project. Spek-X is an acoustic spectrum analyser, a fork of [Spek-alternative](https://github.com/withmorten/spek-alternative), originally derived from [Spek](https://github.com/alexkay/spek).

- Version: 0.9.4
- Repository: <https://github.com/MikeWang000000/spek-X>
- License: GNU General Public License v3 (see `lic/GPL`)

## Technology Stack

| Layer | Technology |
|-------|------------|
| Languages | C++11, C |
| GUI toolkit | wxWidgets >= 3.0 |
| Audio decoding | FFmpeg / libav (`libavformat`, `libavcodec`, `libavutil`) |
| Threading | POSIX threads (`pthread`) |
| FFT | FFmpeg `libavutil/tx` (with legacy `avfft` fallback) |
| Build system | GNU autotools (`autoconf`, `automake`, `libtool`, `intltool`) |
| Internationalisation | gettext + intltool |
| Packaging | Debian `.deb`, Windows ZIP/MSI, macOS `.app` bundle |

## Project Structure

```
spek-X-main/
├── configure.ac              # Autotools project definition
├── Makefile.am               # Top-level Automake file
├── autogen.sh                # Bootstrap script
├── build-mingw.sh            # Local MinGW helper
├── src/                      # Application source
├── tests/                    # Unit/performance tests and sample audio files
├── data/                     # Desktop file, icons, screenshot
├── dist/                     # Platform packaging scripts
├── man/                      # Man page
├── po/                       # Translations
├── lic/                      # Third-party licence texts
├── .github/workflows/         # CI/CD
└── .tx/config                # Transifex configuration
```

### Source Organisation (`src/`)

| File(s) | Responsibility |
|---------|----------------|
| `spek.cc`, `spek-window.cc/h` | `wxApp`, main frame, menus, toolbar, recent-files, queue, drag-and-drop |
| `spek-spectrogram.cc/h` | Custom `wxWindow` that renders the spectrogram and drives analysis |
| `spek-audio.cc/h` | FFmpeg-based audio file opener/reader |
| `spek-fft.cc/h` | FFT wrapper (`FFT`, `FFTPlan`) |
| `spek-pipeline.cc/h` | Producer/consumer pipeline using two pthreads (reader + worker) |
| `spek-events.cc/h` | Custom `wxEvent` for delivering FFT columns to the GUI thread |
| `spek-palette.cc/h` | Eight colour palettes (Spectrum, SoX, Green, Rainbow, Teal, Heat, Ice, Grayscale) |
| `spek-ruler.cc/h` | Axis/ruler drawing helper |
| `spek-info-panel.cc/h` | Side panel showing file metadata |
| `spek-preferences.cc/h` | Singleton settings (INI file via `wxFileConfig`) |
| `spek-preferences-dialog.cc/h` | Preferences UI including language list |
| `spek-export-dialog.cc/h` | Export-image dialog with presets/custom resolution |
| `spek-platform.cc/h` | Platform-specific config path, DPI/font scale, init |
| `spek-artwork.cc/h` | `wxArtProvider` for toolbar/frame icons |
| `spek-utils.cc/h` | Version compare and min/max helpers |

`src/Makefile.am` builds:

- `libspek.a` — core engine (audio, FFT, palette, pipeline, utils), linked against FFmpeg.
- `spek` — GUI binary, built from the remaining sources plus `libspek.a` and wxWidgets.

## Build and Test Commands

### Dependencies

- wxWidgets >= 3.0
- FFmpeg >= 5.0 (current CI uses FFmpeg 7.1)
- Build tools: `g++`, `make`, `pkg-config`, `libtool`, `intltool`

On Debian/Ubuntu:

```bash
sudo apt install g++ make pkg-config libtool intltool \
    libwxgtk3.2-dev wx-common libavcodec-dev libavformat-dev
```

### Standard Build

```bash
./autogen.sh
make
src/spek                    # run uninstalled
sudo make install           # system install
```

`autogen.sh` runs `intltoolize --automake --copy` and `autoreconf -fiv`, then executes `./configure` unless `NOCONFIGURE` is set.

### Testing

```bash
make check
```

This builds and runs the `test` and `perf` programs in `tests/`.

To run tests under Valgrind:

```bash
./autogen.sh --enable-valgrind
make check
```

### Headless Export

Spek-X supports a command-line export mode:

```bash
spek input.wav output.png [width] [height]
```

## Testing Strategy

Tests live in `tests/` and use a small custom framework (`test.h`/`test.cc`). There is no external test library dependency.

- `test-audio.cc` — opens sample files in `tests/samples/` and verifies codec info, bit rate, sample rate, channels, duration, sample count.
- `test-fft.cc` — checks FFT plan sizes, zero/DC inputs, and sine-wave peaks.
- `test-utils.cc` — version-comparison tests.
- `perf.cc` — currently mostly stubs; auto-generates `tests/samples/perf.wav` for future performance regression tests.

`tests/Makefile.am` builds both `test` and `perf` as `check_PROGRAMS`; `make check` runs them.

## Code Style Guidelines

- **Language:** C++11 with GNU extensions (`-std=gnu++11`).
- **File extensions:** `.cc` for source, `.h` for headers.
- **Header guards:** Use `#pragma once`.
- **Class naming:** `SpekWindow`, `SpekSpectrogram`, `AudioFileImpl`.
- **Member access:** `this->` is used pervasively.
- **Indentation:** 4 spaces, no tabs.
- **wxWidgets style:** `BEGIN_EVENT_TABLE` / `EVT_*` macros, `wxID_HIGHEST` offsets for custom IDs.
- **Strings:** Use `_()` / `wxGetTranslation()` for translatable UI text, `wxT("...")` for literals, and `N_()` for translatable static strings.
- **Error handling:** `enum class AudioError` with an `operator!` overload.
- **Platform conditionals:** Use `#ifdef OS_WIN`, `OS_OSX`, `OS_UNIX` from the generated `config.h`.
- The old `#ifdef SPEK_CHECK_VERSION` update-check code remains but is not enabled by configure.

## Runtime Architecture

1. `SpekWindow` opens one or more files (queue/recent-files).
2. `SpekSpectrogram::start()` creates an `AudioFile` and an `FFTPlan`, then starts the pipeline.
3. The reader thread decodes audio via FFmpeg; the worker thread windows the samples and applies FFT per pixel column.
4. Each completed column is posted as `SpekHaveSampleEvent` to the GUI thread.
5. The spectrogram redraws from the cached `wxImage`.

Constants of note:

- `MAX_QUEUE_FILES = 100`
- `MAX_RECENT_FILES = 10`

## Internationalisation

- gettext package name: `spek`
- 32 languages in `po/LINGUAS` and `po/*.po`.
- Source strings are extracted from files listed in `po/POTFILES.in`.
- Transifex integration is configured in `.tx/config`.
- Platform-specific loading:
  - Unix: standard filesystem `.mo` files.
  - Windows: `wxResourceTranslationsLoader` loads MOs embedded as resources.
  - macOS: `po/<lang>.gmo` is copied into `Spek.app/Contents/Resources/<lang>.lproj/spek.mo`.

## Deployment and Packaging

### Linux / Unix

- `data/spek.desktop.in.in` is processed via intltool into `data/spek.desktop`.
- Hicolor icons are installed from `data/icons/` (16×16 to 48×48 plus scalable).
- Man page: `man/spek.1`.
- Debian packaging files are under `dist/debian/`; CI builds a `.deb` with `dpkg-deb --build dist/debian/spek-x`.

### Windows

- `dist/win/install_deps.sh` builds static wxWidgets 3.2.6 and FFmpeg 7.1 from source in MSYS2.
- `dist/win/bundle.sh` produces a single statically-linked `spek.exe` and a ZIP archive at `dist/win/Spek/Spek.zip`.
- `dist/win/spek.rc` embeds icons and manifest; `dist/win/compile-rc.py` embeds `.gmo` translations as resources.
- `dist/win/spek.wxs.in` is a WiX source for an MSI installer but is not built in CI.

### macOS

- `dist/osx/install_deps.sh` and `dist/osx/bundle.sh` build wxWidgets with CMake and FFmpeg as shared libraries per architecture.
- `dist/osx/bundle_universal.sh` merges `x86_64` and `arm64` binaries with `lipo`.
- Output archives: `Spek.x86_64.tgz`, `Spek.arm64.tgz`, `Spek.tgz`.

## CI/CD

`.github/workflows/ci.yml` runs on push, pull request, and workflow dispatch:

1. **Build-Debian-x86_64** — builds and packages a `.deb` in a Debian `sid` container.
2. **Build-Windows-x64** — restores a cached MSYS2 tree, runs `dist/win/bundle.sh`.
3. **Build-macOS-Intel** — `ARCH=x86_64` build and bundle.
4. **Build-macOS-AppleSilicon** — `ARCH=arm64` build and bundle.
5. **Build-macOS-Universal** — downloads Intel and ARM artifacts, merges them.
6. **Unit-Test** — `./autogen.sh && make check` in Debian `sid`.

`.github/workflows/caching.yml` refreshes the cached MSYS2 dependency tree weekly.

## Security Considerations

- Spek-X decodes arbitrary audio files via FFmpeg. Ensure FFmpeg is kept up to date to benefit from security fixes in bundled dependencies.
- The Windows build creates a single statically-linked executable; verify that all statically linked libraries are from trusted sources and are current.
- The macOS bundle copies shared libraries into the app; use `install_name_tool` and codesigning consistently.
- Do not enable the legacy `SPEK_CHECK_VERSION` update-check feature without reviewing its network behaviour.

## Useful References

- `README.md` — project overview and changelog.
- `INSTALL.md` — installation and build instructions.
- `MANUAL.md` — user manual, command-line usage, and keybindings.
- `LICENCE.md` — authors, translators, and third-party licence notes.
- `dist/win/README.md`, `dist/osx/README.md` — platform-specific build details.
