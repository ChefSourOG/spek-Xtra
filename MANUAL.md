# SPEK-XTRA(1) User's Guide | Version 1.0.0

## NAME

Spek-Xtra - Acoustic Spectrum Analyser

## SYNOPSIS

`spek-xtra` [*OPTION* *...*] \[*FILE*] \[*PNG*] \[*WIDTH*] \[*HEIGHT*]

## DESCRIPTION

*Spek-Xtra* generates a spectrogram for the input audio file.

## ARGUMENTS

`FILE`
:   Audio file to be analyzed.

`PNG`
:   Write spectrogram to this PNG file then exit.

`WIDTH`
:   Initial width of the spectrum in pixels.

`HEIGHT`
:   Initial height of the spectrum in pixels.

## OPTIONS

`-h`, `--help`
:   Output the help message then quit.

`-V`, `--version`
:   Output version information then quit.

## KEYBINDINGS

### Notes

On macOS use the Command key instead of Ctrl.

### Menu

`Ctrl-O`
:   Open a new file.

`Ctrl-S`
:   Save the spectrogram as an image file.

`Ctrl-E`
:   Show the preferences dialog.

`F1`
:   Open online manual in the browser.

`Shift-F1`
:   Show the about dialog.

### Spectrogram

`c`, `C`
:   Change the audio channel.

`f`, `F`
:   Change the DFT window function.

`l`, `L`
:   Change the lower limit of the dynamic range in dBFS.

`p`, `P`
:   Change the palette.

`s`, `S`
:   Change the audio stream.

`u`, `U`
:   Change the upper limit of the dynamic range in dBFS.

`w`, `W`
:   Change the DFT window size.

## FILES

*~/.config/spek-xtra/preferences*
:   The configuration file for *Spek-Xtra*, stored in a simple INI format.

## AUTHORS

Alexander Kojevnikov <alexander@kojevnikov.com>. Other contributors are listed
in the LICENCE.md file distributed with the source code.

## DISTRIBUTION

The latest version of *Spek-Xtra* may be downloaded from <https://github.com/MikeWang000000/spek-X>.
