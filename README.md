# hello-port-audio

A simple C++ project demonstrating audio synthesis and playback using [PortAudio](http://www.portaudio.com/).

## Features

- Generates and plays a sine wave using a wavetable.
- Selects MIDI notes and converts them to frequency.
- Cross-platform (tested on macOS, should work on Linux/Windows with PortAudio).

## Requirements

- C++23 compiler
- [PortAudio](http://www.portaudio.com/) development libraries
- CMake 3.16+

## Building

```sh
cmake -S . -B build
cmake --build build
```

## Running

```sh
./build/hello-port-audio
```

## Project Structure

- `src/` — Source files
- `include/` — Header files
- `CMakeLists.txt` — Build configuration
- `format.sh` — Code formatting script
- `.clang-format` — Formatting rules

## License

MIT License