# wasm-coremark
[![Build](https://github.com/wasm3/wasm-coremark/actions/workflows/build.yml/badge.svg)](https://github.com/wasm3/wasm-coremark/actions/workflows/build.yml)

CoreMark 1.01 ported to WebAssembly.

[CoreMark](https://www.eembc.org/coremark) is a simple, yet sophisticated benchmark that is designed specifically to test the functionality of a processor core. Running CoreMark produces a single-number score allowing users to make quick comparisons between processors.

**Source**: https://github.com/eembc/coremark

**Online demo**: [here](https://wasm3.github.io/wasm-coremark/coremark-minimal.html)

## Builds

- **coremark.wasm** - WASI app
- **coremark-minimal.wasm** - Simple wasm module with minimal runtime dependencies
- **coremark-emcc.*** - Emscripten build

## Running `WASI` app

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

With no arguments the benchmark picks its own iteration count, aiming for a run
of 10 seconds or more. All four CoreMark parameters can be given instead:

```sh
# seed1 seed2 seed3 iterations
wasm3 coremark.wasm 0x0 0x0 0x66 60000        # performance run
wasm3 coremark.wasm 0x3415 0x3415 0x66 60000  # validation run
```

Pass `0` as the iteration count to keep the automatic calibration. A run of
under 10 seconds reports a score, but is not a valid CoreMark result.

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
- [`coremark-minimal.mjs`](./coremark-minimal.mjs) - **Node.js** example using V8.
- [`coremark-minimal.py`](./coremark-minimal.py) - **Python 3** example using `Wasm3` engine.
- [`wasmi-coremark`](https://github.com/patractlabs/wasm-coremark) - **Rust** example using `Wasmtime`, `Wasm3`, `wasmi`.

Both local examples expect `coremark-minimal.wasm` next to them:

```sh
# Node.js
node coremark-minimal.mjs

# Python 3, via the wasm3 bindings
pip install pywasm3
./coremark-minimal.py
```

Serve the directory to run the browser example:
```sh
python3 -m http.server 8000
# visit http://localhost:8000/coremark-minimal.html
```

To run with any wasm runtime:
1. You need to provide an `u64 env.clock_ms()` function, which should return current time in milliseconds.
2. Call `f32 run()` function. It should take `12..20` seconds to execute and return a CoreMark result.

Wasm3 supplies `clock_ms` itself, so its CLI can run the module directly:
```sh
wasm3 --func run coremark-minimal.wasm
```

## Build instructions

[`build.sh`](./build.sh) clones the CoreMark sources at a pinned revision,
builds each target in its own scratch copy under `build/`, and drops the result
in the repository root:

```sh
export WASI_SDK=/path/to/wasi-sdk-34.0-x86_64-linux

./build.sh              # coremark.wasm and coremark-minimal.wasm
./build.sh emcc         # coremark-emcc.{html,js,wasm}, needs emcc on PATH
./build.sh native       # coremark.elf, for comparison against the host
```

Three ports live in [`ports/`](./ports):

- **`ports/wasi`** builds on CoreMark's `posix` port. 
- **`ports/emcc`** is the same `posix` port, for the Emscripten build.
- **`ports/minimal`** is a bare-module port derived from CoreMark's `barebones` one: no WASI, no libc I/O, no command line. It takes the clock from an
  `env.clock_ms` import and exports `run`.

## Wasm features

All three wasm builds enable the
[Lime1](https://github.com/WebAssembly/tool-conventions/blob/main/Lime.md#lime1)
feature set, plus tail calls:

```
mutable-globals      bulk-memory-opt
multivalue           extended-const
sign-ext             call-indirect-overlong
nontrapping-fptoint  tail-call
```

An engine has to implement all of them to load the modules. Drop
`WASM_FEATURES` in [`build.sh`](./build.sh) to a smaller set to target an older
one; LLVM spells the Lime1 half of the list as `-mcpu=lime1`.

## Releases

[`.github/workflows/build.yml`](./.github/workflows/build.yml) builds every push
and pull request and uploads the binaries as a run artifact. Pushing a `v*` tag
publishes them as a GitHub release:

```sh
git tag v1.0.0
git push origin v1.0.0
```

The same workflow deploys the demo pages from `main` to GitHub Pages.

## Toolchain versions

The toolchains are pinned in the workflow (`WASI_SDK_VERSION`, `EMSCRIPTEN_VERSION`),
and each build records the exact versions in `toolchain.txt` next to the
binaries:

```log
# WASI
wasi-sdk 34.0
clang version 23.1.0-wasi-sdk

# EMCC
emcc (Emscripten gcc/clang-like replacement + linker emulating GNU ld) 6.0.8
clang version 24.0.0git
```
