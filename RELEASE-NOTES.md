# Spek-Xtra 1.0.0 Release Notes

Spek-Xtra 1.0.0 is the first release of the friendly fork of Spek-X, bringing a polished UI and many quality-of-life improvements for audio visualisation.

## What's New

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

## Bug Fixes

- Fixed empty spectrogram render when the audio info panel is hidden on startup.
- Fixed version label truncation in the top-right corner of the spectrogram.

## Assets

- `Spek-Xtra.zip` — Windows x64 portable build
- `spek-xtra_1.0.0_amd64.deb` — Debian x86_64 package
- `Spek-Xtra.tgz` — macOS universal bundle (built via CI)
