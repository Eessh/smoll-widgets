# Smoll Widgets

Super simple backend agnostic widgets writen in C.

### Features
- Lightweight.
- You can build your own custom widgets.
- Provides incremental render updates (can be used by both Stateful Rendering & Immediate Mode Rendering architectures).
- Simple backend API (just write 5 functions `init_backend()`, `deinit_backend()`, `translate_events()`, `render_rect()`, `render_text()`).
- Software Rendering/Hardware(GPU) Accelerated Rendering, depends upon backend.

### Implemented Backends
- [SDL2 + Cairo](backends/sdl2_cairo)
  - SDL2 - for cross-platform windowing & events
  - Cairo - for rendering
- [Win32 + Cairo](backends/win32_cairo)
  - Win32 - for windowing & events
  - Cairo - for rendering

### Building Examples
- [SDL2 + Cairo example](backends/sdl2_cairo/sdl2_cairo_example.c)
  - Windows
    ```powershell
    .\scripts\windows\gen_makefiles.bat
    .\scripts\windows\build_sdl2_cairo_example_release.bat
    ```
  - Linux - 🚧 WIP
  - MacOS - 🚧 WIP
- [Win32 + Cairo example](backends/win32_cairo/win32_cairo_example.c)
  ```powershell
  .\scripts\windows\gen_makefiles.bat
  .\scripts\windows\build_win32_cairo_example_release.bat
  ```
