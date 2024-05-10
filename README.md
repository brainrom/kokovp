# KokoVP

KokoVP is a video player (mpv frontend), primarly for Linux and Windows.
The project's starting point, is to reimplement SMPlayer's functionality
from scratch, using modular architecture and modern technologies, sacrifying side functions.

## User fetatures
 - Play videos
 - Playlist with drag'n drop appending and reordering
 - Auto discover external audio and subtitles tracks, even in nested folders (with configurable depth)
 - Configurable mouse and keyboard actions
 - Remember video position and state (volume, selected tracks, etc.)
 - Video screenshots

## Code features
 - Wayland, HiDPI, MultiDPI aware
 - Using libmpv OpenGL Render API
 - Built on top of Qt6

## Building
Dependencies:
- Qt6 (Core, Widgets, OpenGLWidgets, Network)
- libmpv

You can build KokoVP from source using CMake.  Here are the steps using the command line:

```console
mkdir build
cd build
cmake ..
cmake --build .
```

## TODO
 - Beauty icon
 - Windows Build
 - Web interface for remote control
 - Lua scripting for selecting tracks
 - Configuration GUI for GLSL (and possible automatic Anime4k configuration)
 - Toolbar configuration GUI
 - Custom timeline with chapters

---

Currently, the project in active state of development, so the help is welcomed.

Copyright (C) 2024 Ilya Chelyadin <ilya77105@gmail.com>
