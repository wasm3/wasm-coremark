#!/usr/bin/env bash
#
# Builds the WebAssembly CoreMark binaries. The EEMBC sources are fetched from
# upstream, unmodified; each target builds in its own scratch copy under
# ./build, and the result is copied to the repository root.
#
# Usage:
#   ./build.sh [target ...]      targets: wasi minimal emcc native (default: wasi minimal)
#
# Environment:
#   WASI_SDK      wasi-sdk install to use   (default: $HOME/wasi-sdk)
#   CC_NATIVE     compiler for the native reference build (default: cc)
#   COREMARK_REF  CoreMark commit or tag to build (default: pinned below)

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$ROOT/build"
WASI_SDK="${WASI_SDK:-$HOME/wasi-sdk}"
CC_NATIVE="${CC_NATIVE:-cc}"

# The Lime1 feature set, plus tail calls. Lime1 is what LLVM also spells
# -mcpu=lime1; the features are listed one by one so the set stays visible and
# tail-call can ride along with it.
#   https://github.com/WebAssembly/tool-conventions/blob/main/Lime.md#lime1
#
# The modules these produce need an engine that implements all of it -- that is
# rather the point here, but it does rule out older runtimes.
WASM_FEATURES="-mmutable-globals -mmultivalue -msign-ext -mnontrapping-fptoint \
-mbulk-memory-opt -mextended-const -mcall-indirect-overlong -mtail-call"

# Upstream as of 2025-05-01
COREMARK_REPO="https://github.com/eembc/coremark.git"
COREMARK_REF="${COREMARK_REF:-1f483d5b8316753a742cbf5590caf5bd0a4e4777}"

# Clone once into ./build/coremark, at the pinned revision.
fetch_src() {
    local src="$BUILD/coremark"
    if [ ! -d "$src/.git" ]; then
        mkdir -p "$BUILD"
        git clone --quiet "$COREMARK_REPO" "$src"
    fi
    if ! git -C "$src" rev-parse --quiet --verify "$COREMARK_REF^{commit}" > /dev/null; then
        git -C "$src" fetch --quiet origin
    fi
    git -C "$src" -c advice.detachedHead=false checkout --quiet "$COREMARK_REF"
}

# fresh_tree <name> -> path to a pristine copy of the CoreMark sources
fresh_tree() {
    local dir="$BUILD/$1"
    fetch_src
    rm -rf "$dir"
    mkdir -p "$dir"
    git -C "$BUILD/coremark" archive HEAD | tar -x -C "$dir"
    echo "$dir"
}

need_wasi_sdk() {
    if [ ! -x "$WASI_SDK/bin/clang" ]; then
        echo "No wasi-sdk at $WASI_SDK -- set WASI_SDK to an install of" >&2
        echo "https://github.com/WebAssembly/wasi-sdk/releases" >&2
        exit 1
    fi
}

# The WASI app. ports/wasi builds on CoreMark's posix port rather than the
# simple one, for two reasons:
#   - posix defaults SEED_METHOD to SEED_ARG, so the documented seed and
#     iteration arguments actually reach the benchmark.
#   - posix times with clock_gettime(CLOCK_REALTIME). The simple port uses
#     clock(), which wasi-libc implements on CLOCK_PROCESS_CPUTIME_ID -- a
#     clock WASI dropped, and one several runtimes reject outright, leaving
#     CoreMark spinning in its calibration loop.
build_wasi() {
    need_wasi_sdk
    local dir; dir="$(fresh_tree wasi)"
    cp -r "$ROOT/ports/wasi" "$dir/wasi"
    make -C "$dir" compile \
        PORT_DIR=wasi \
        CC="$WASI_SDK/bin/clang" \
        PORT_CFLAGS="-O3 $WASM_FEATURES" \
        EXE=.wasm
    cp "$dir/coremark.wasm" "$ROOT/coremark.wasm"
    echo "-> coremark.wasm"
}

# The bare module: no WASI, no libc I/O, imports env.clock_ms, exports run().
build_minimal() {
    need_wasi_sdk
    local dir; dir="$(fresh_tree minimal)"
    cp -r "$ROOT/ports/minimal" "$dir/minimal"
    patch -s -d "$dir" -p1 < "$ROOT/ports/minimal/core_main-report-score.patch"
    make -C "$dir" compile \
        PORT_DIR=minimal \
        CC="$WASI_SDK/bin/clang" \
        PORT_CFLAGS="-O3 $WASM_FEATURES" \
        EXE=.wasm
    cp "$dir/coremark.wasm" "$ROOT/coremark-minimal.wasm"
    echo "-> coremark-minimal.wasm"
}

# The Emscripten build, for browsers that want the JS shell.
build_emcc() {
    if ! command -v emcc > /dev/null; then
        echo "emcc not on PATH -- install and activate emsdk first" >&2
        exit 1
    fi
    local dir; dir="$(fresh_tree emcc)"
    cp -r "$ROOT/ports/emcc" "$dir/emcc"
    make -C "$dir" compile \
        PORT_DIR=emcc \
        CC=emcc \
        PORT_CFLAGS="-O3 $WASM_FEATURES" \
        EXE=-emcc.html
    cp "$dir"/coremark-emcc.html "$dir"/coremark-emcc.js "$dir"/coremark-emcc.wasm "$ROOT/"
    echo "-> coremark-emcc.{html,js,wasm}"
}

# A native binary from the same sources, to compare a runtime against the host.
build_native() {
    local dir; dir="$(fresh_tree native)"
    make -C "$dir" compile \
        PORT_DIR=posix \
        CC="$CC_NATIVE" \
        PORT_CFLAGS="-O3" \
        EXE=.elf
    cp "$dir/coremark.elf" "$ROOT/coremark.elf"
    echo "-> coremark.elf"
}

targets=("$@")
if [ ${#targets[@]} -eq 0 ]; then
    targets=(wasi minimal)
fi

for t in "${targets[@]}"; do
    case "$t" in
        wasi)    build_wasi ;;
        minimal) build_minimal ;;
        emcc)    build_emcc ;;
        native)  build_native ;;
        *) echo "unknown target: $t (wasi minimal emcc native)" >&2; exit 1 ;;
    esac
done
