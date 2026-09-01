#!/usr/bin/env node
//
// Runs coremark-minimal.wasm on the JavaScript engine built into Node. The
// module imports one function, env.clock_ms, and exports run().

import { readFileSync } from "node:fs";

const wasmFile = new URL("./coremark-minimal.wasm", import.meta.url);

const { instance } = await WebAssembly.instantiate(readFileSync(wasmFile), {
    env: {
        clock_ms: () => BigInt(Date.now()),
    },
});

console.log("Running CoreMark 1.01... [should take 12..20 seconds]");

const res = instance.exports.run();

if (res > 1) {
    console.log(`Result: ${res.toFixed(3)}`);
} else {
    console.log("Error");
    process.exit(1);
}
