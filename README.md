# wasm3-coremark
CoreMark 1.0 ported to WebAssembly

- **coremark.wasm** - WASI app
- **coremark-minimal.wasm** - Simple wasm module with minimal runtime dependencies
- **coremark-emcc.*** - Emscripten build

## Build instructions

```sh
git clone https://github.com/eembc/coremark.git
cd coremark

# WASI build
make compile PORT_DIR=simple CC=wasicc  PORT_CFLAGS="-O3" EXE=.wasm

# Emscripten build
make compile PORT_DIR=simple CC=emcc    PORT_CFLAGS="-O3" EXE=-emcc.html

# Native 32-bit build, for comparison:
make compile PORT_DIR=simple PORT_CFLAGS="-O3" XCFLAGS="-m32" EXE=.elf
```

`minimal` build is currently impossible without modification of `core_main.c`.

## Toolchain versions

Builds were produced using:
```log
# WASIENV
wasienv 0.5.4
wasi-sdk 12.0
clang version 11.0.0 (https://github.com/llvm/llvm-project 176249bd6732a8044d457092ed932768724a6f06)

# EMCC
emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 1.39.12
clang version 11.0.0 (/b/s/w/ir/cache/git/chromium.googlesource.com-external-github.com-llvm-llvm--project eaa55590945a130131a47a4d2b89e3bbdfced79e)
```
