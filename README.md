# virtual-textures

A minimal OpenGL prototype implementing virtual texturing without hardware sparse textures.

This repository contains a small self-contained prototype that implements the core ideas behind virtual texturing. It focuses on correctness and clarity rather than completeness or performance. The project demonstrates how large textures can be split into pages, mapped through a GPU page table, and streamed into a physical atlas on demand based on what the GPU actually samples.

### Why this exists

Virtual texturing is usually explained at a high level and implemented behind layers of engine code. This prototype strips it down to the essentials so the system can be understood and reasoned about end to end.

### Features

- Virtual texture sampling via page table indirection
- Physical texture atlas with page residency
- GPU feedback pass to detect page usage and mip selection
- CPU-side page cache and simple eviction
- Page padding for bilinear filtering

### Project overview

The demo renders a simple plane textured with a virtual texture. A minimap overlay shows the currently resident pages and feedback results making page streaming behavior visible in real time. Camera movement and zoom trigger higher-resolution tiles to be requested and loaded incrementally.

Texture tiles are generated offline using [vtile](https://github.com/shlomnissan/vtile). The sample texture is split into padded tiled mip levels before runtime. This keeps the runtime code focused on page management and streaming rather than image processing.

The example UV grid texture used in the demo is by Maurus Löffel. You can download the [source image here](https://drive.google.com/drive/folders/1K_G_hbFyohR8-xCCAlYx8xhsd_a7Ir7G).

![Virtual texturing demo](https://github.com/user-attachments/assets/8082b086-7b35-4ae5-b33c-01bf09b62796)

## Building the project

The project uses [CMake](http://cmake.org/) and [vcpkg](https://vcpkg.io/en/) for dependency management.

```bash
git clone https://github.com/shlomnissan/virtual-textures.git
cd virtual-textures

cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake

cmake --build build
```

Exact dependency versions are managed through `vcpkg.json`.

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
