# virtual-textures

A minimal prototype implementing virtual texturing without hardware sparse textures, put together while writing [How Virtual Textures Really Work](https://www.shlom.dev/articles/how-virtual-textures-work/).

This repository contains a small prototype that implements the core ideas behind virtual texturing. It focuses on correctness and clarity rather than completeness or performance. The project demonstrates how large textures can be split into pages, mapped through a GPU page table, and streamed into a physical atlas on demand based on what the GPU actually samples.

Virtual texturing is usually explained at a high level and implemented behind layers of engine code. This prototype strips it down to the essentials so the system can be understood and reasoned about end to end.

![virtual texture prototype](https://github.com/user-attachments/assets/983ee7d7-70d2-4652-ae4e-8dc583d55501)

### Features

- Virtual texture sampling via page table indirection
- Physical texture atlas with page residency
- GPU feedback pass to detect page usage and mip selection
- CPU-side page cache and simple eviction
- Page padding for bilinear filtering

### Project Overview

The demo renders a simple terrain using a virtual texture. A minimap overlay displays currently resident pages and feedback results, making page streaming behavior visible in real time. Camera movement and zooming trigger higher-resolution tiles to be requested and loaded incrementally.

- Rendering: Powered by [VGLX](https://www.vglx.org/) which handles internal dependencies.
- Tile Generation: Textures are generated offline using [vtile](https://github.com/shlomnissan/vtile).
- Assets: The UV grid texture is by Maurus Löffel. You can download the [source image here](https://drive.google.com/drive/folders/1K_G_hbFyohR8-xCCAlYx8xhsd_a7Ir7G).

### Building the Project

The project uses [CMake](http://cmake.org/) and [VGLX](https://vglx.org) for rendering. The easiest way to install VGLX is with the Python installer included in the repository. It guides the process and builds the engine using the correct presets for your system.

```bash
# clone the repository
git clone https://github.com/shlomnissan/vglx.git
cd vglx

# run the installer
python3 -m tools.installer.main
```

With VGLX installed, you can build this prototype like any other CMake project.

```bash
# clone the repository
git clone https://github.com/shlomnissan/virtual-textures.git
cd virtual-textures

# configure cmake
cmake -S . -B build

# build the project
cmake --build build
```

## License

```
Copyright (c) 2025-present Shlomi Nissan
https://shlom.dev | https://vglx.org

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
