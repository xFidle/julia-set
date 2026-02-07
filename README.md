# Julia Set
## Overview
A simple [Julia Set](https://en.wikipedia.org/wiki/Julia_set) implementation
written in x86_64 assembly and C. It uses AVX2 instruction set to accelerate
computations by processing four double-precision pixels simultaneously in
256-bit vectors. Moreover, a custom threadpool is used to utilize all of the
CPU's power. The generated pixel buffer is rendered using
[SDL3](https://github.com/libsdl-org/SDL). There is no GPU acceleration - 
computations are performed only on CPU. Project was developled under Linux OS.

## Demo

https://github.com/user-attachments/assets/54b41dbb-d75b-4273-a780-d5b80ba74497

https://github.com/user-attachments/assets/6a25990e-43e3-4192-ad94-73b94abb5a42

## Requirements
Required tools:
- x86_64 CPU with AVX2 support
- CMake 3.25+, make, NASM
- pthreads
- [SDL3 development libraries](https://wiki.libsdl.org/SDL3/README-linux)

## Installation
Clone the repo with submodules:
```bash
git clone --recursive git@github.com:xFidle/julia-set.git
```

Build executable:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make   
```

Run executable:
```bash
./src/julia-set
```

## Usage
To interact with program use controls listed below: 
| Key | Action |
|-----|--------|
| H/J/K/L or arrows | Change fractal shape (real/imaginary components) |
| W/S/A/D | Change camera's position |
| Z/X | Zoom in/out |
| R | Reset to default view |
| ESC | Quit |
