# Spek-Xtra

[![CI](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml/badge.svg?branch=main&event=push)](https://github.com/ChefSourOG/spek-Xtra/actions/workflows/ci.yml)
[![Latest Release](https://img.shields.io/github/v/release/ChefSourOG/spek-Xtra)](https://github.com/ChefSourOG/spek-Xtra/releases/latest)
[![License](https://img.shields.io/github/license/ChefSourOG/spek-Xtra)](./LICENSE.md)
[![Platforms](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-blue)](./INSTALL.md)
[![C++](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](./src)
[![Downloads](https://img.shields.io/github/downloads/ChefSourOG/spek-Xtra/total)](https://github.com/ChefSourOG/spek-Xtra/releases)

[[简体中文 Simplified Chinese]](./README-zh_CN.md)

**Spek-Xtra** is a modern, cross-platform acoustic spectrum analyser for audio files. Originally derived from [Spek](https://github.com/alexkay/spek) and built on top of [Spek-X](https://github.com/MikeWang000000/spek-X/), Spek-Xtra makes it easy to visualise the frequency content of your audio with a clean, responsive spectrogram.

Whether you are a musician, audio engineer, podcaster, or hobbyist, Spek-Xtra gives you the tools to inspect codecs, compare takes, export publication-ready spectrograms, and navigate large files with intuitive zoom and pan controls.

## What Spek-Xtra Does

Spek-Xtra reads virtually any audio format supported by FFmpeg and renders a detailed, colour-coded spectrogram. You can:

- Inspect audio metadata at a glance (codec, bit rate, sample rate, channels, duration).
- Switch between eight colour palettes to suit your taste or workflow.
- Zoom and pan through the spectrogram with the mouse.
- Compare two audio files side-by-side in split-view mode.
- Export high-resolution spectrogram images as PNG.
- Manage multiple files with a sidebar queue and a Recent Files list.
- Toggle between linear and logarithmic frequency scales.

## What's New in Spek-Xtra 1.0.0

Spek-Xtra 1.0.0 adds a host of features and quality-of-life improvements:

- **Audio metadata panel** — see codec, bit rate, sample rate, channels, and duration at a glance.
- **Eight colour palettes** — Spectrum, SoX, Green, Rainbow, Teal, Heat, Ice, and Grayscale.
- **Export spectrogram to PNG** — save high-resolution images for reports and sharing.
- **Recent Files** — quickly reopen one of the last 10 audio files.
- **Sidebar file queue** — load and manage multiple files without leaving the app.
- **Split-view compare mode** — view two spectrograms side-by-side.
- **Mouse-wheel zoom & click-drag pan** — inspect details with precision.
- **Interactive status bar** — real-time readout of frequency, time, and dB under the cursor.
- **Linear / logarithmic frequency scale toggle**.
- **Refreshed logo and toolbar icons** with high-DPI support.
- **Debian package renamed** to `spek-xtra`.
- Bug fixes for empty spectrogram rendering and version-label truncation.

[See the full changelog below](#spek-xtra-new-features-and-enhancements) for more details.

## Downloads

Platform | Package
---------|---------
Windows (x64) | [spek-xtra-1.0.0-windows-x86_64.zip](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.zip) / [spek-xtra-1.0.0-windows-x86_64.exe](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-windows-x86_64.exe)
Debian (x86_64) | [spek-xtra_1.0.0_amd64.deb](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra_1.0.0_amd64.deb)
macOS (Universal) | [spek-xtra-1.0.0-macos-universal.tgz](https://github.com/ChefSourOG/spek-Xtra/releases/download/v1.0.0/spek-xtra-1.0.0-macos-universal.tgz)
Source | [v1.0.0.tar.gz](https://github.com/ChefSourOG/spek-Xtra/archive/v1.0.0.tar.gz)

[Spek-Xtra](https://github.com/ChefSourOG/spek-Xtra) (IPA: /spɛks/) is a fork of [spek-X](https://github.com/MikeWang000000/spek-X/) which is a fork of [Spek-alternative](https://github.com/withmorten/spek-alternative), which is originally derived from [Spek](https://github.com/alexkay/spek).

Spek is an acoustic spectrum analyser written in C and C++. It uses FFmpeg
libraries for audio decoding and wxWidgets for the GUI.

Spek is available on *BSD, GNU/Linux, Windows and macOS.

Find out more about Spek on its website: <http://spek.cc/>

<img src="./data/example_image_1.png" height="500">
<img src="./data/example_image_2.png" height="500">
<img src="./data/example_image_3.png" height="500">

## Spek-Xtra New Features And Enhancements

Spek-Xtra 1.0.0 Updates:
 * New info panel showing audio file metadata (codec, bit rate, sample rate, channels, duration).
 * New colour palette selection with eight presets (Spectrum, SoX, Green, Rainbow, Teal, Heat, Ice, Grayscale).
 * Export spectrogram to PNG with custom resolution.
 * Recent Files submenu that lists the last 10 opened audio files.
 * Sidebar file queue for opening and managing multiple audio files.
 * Split-view comparison mode for two audio files side-by-side.
 * Mouse-wheel zoom and click-drag panning on the spectrogram.
 * Interactive status-bar readout showing frequency, time, and dB under the cursor.
 * Toggle between linear and logarithmic frequency scales.
 * Refreshed application logo with a transparent background.
 * Updated toolbar icons using custom-selected PNG assets, with high-DPI support.
 * Debian package renamed to `spek-xtra`.
 * Fixed empty spectrogram render when the audio info panel is hidden on startup.
 * Fixed version label truncation in the top-right corner of the spectrogram.

### Build Instructions

[Windows](./dist/win/README.md) | [macOS](./dist/osx/README.md) | [Linux and other Unix-like systems](./INSTALL.md#building-from-the-git-repository)

### Dependencies

 * wxWidgets >= 3.0
 * FFmpeg >= 5.0
