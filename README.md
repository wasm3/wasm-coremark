# wasm-coremark
CoreMark 1.0 ported to WebAssembly.

[CoreMark](https://www.eembc.org/coremark) is a simple, yet sophisticated benchmark that is designed specifically to test the functionality of a processor core. Running CoreMark produces a single-number score allowing users to make quick comparisons between processors.

**Source**: https://github.com/eembc/coremark

**Online demo**: [here](https://wasm3.github.io/wasm-coremark/coremark-minimal.html)

## Builds

- **coremark.wasm** - WASI app
- **coremark-minimal.wasm** - Simple wasm module with minimal runtime dependencies
- **coremark-emcc.*** - Emscripten build

## Running `WASI` app

You can [**open it with your browser**](https://webassembly.sh/?run-command=coremark).

Use any WASI-compatible runtime to run it directly:
```sh
# Wasm3
wasm3 coremark.wasm

# kanaka/wac
wax coremark.wasm

# wasmtime
wasmtime coremark.wasm

# WAVM
wavm run coremark.wasm

# Wasmer
wasmer run --singlepass coremark.wasm
wasmer run --cranelift coremark.wasm
wasmer run --llvm coremark.wasm

# Wasmer-JS (V8)
wasmer-js run coremark.wasm

# Wasmer-JS (V8 interpreter)
node --wasm-interpret-all $(which wasmer-js) run coremark.wasm
```

## Running `Emscripten` version

You can [**open it with your browser**](https://wasm3.github.io/wasm-coremark/coremark-emcc.html).

Run a local copy:
```sh
python3 -m http.server 8000
# visit http://localhost:8000/coremark-emcc.html
```

Using `Node.js`:
```sh
node coremark-emcc.js
```

## Running `minimal` version

You can [**open it with your browser**](https://wasm3.github.io/wasm-coremark/coremark-minimal.html).

Here are some examples of how to load and execute it using different engines:
- [`coremark-minimal.html`](./coremark-minimal.html) - **JavaScript** example using your browser's engine.
- [`coremark-minimal.py`](./coremark-minimal.py) - **Python 3** example using `Wasm3` engine.
- [`wasmi-coremark`](https://github.com/patractlabs/wasm-coremark) - **Rust** example using `Wasmtime`, `Wasm3`, `wasmi`.

To run with any wasm runtime:
1. You need to provide an `u32 env.clock_ms()` function, which should return current time in milliseconds.
2. Call `f32 run()` function. It should take `12..20` seconds to execute and return a CoreMark result.

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

`minimal` build is more complex, and requires modification of `core_main.c`.

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
