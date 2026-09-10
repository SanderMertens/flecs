#!/bin/sh
# Builds the Flecs wasm image used by the "Flecs playground" page of the
# documentation website. The image bundles the full Flecs distribution
# (distr/flecs.c) with a small host application (main.c) that serves the REST
# API through the flecs_explorer_request export, which is what the explorer's
# flecs.js client uses to talk to wasm images.
#
# Requires Emscripten (emcc). Output goes to docs/html by default; pass a
# different output directory as the first argument. Run from any directory.

set -e
cd "$(dirname "$0")/../.."
OUT="${1:-docs/html}"
NAME=flecs_playground

if ! command -v emcc >/dev/null 2>&1; then
    echo "docs/playground/build.sh: emcc not found; install the Emscripten SDK" \
         "(https://emscripten.org/docs/getting_started/) and add it to PATH" >&2
    exit 1
fi

mkdir -p "$OUT"
emcc -O2 -DNDEBUG -DFLECS_SCRIPT_MATH -DFLECS_SCRIPT_PLATFORM \
    -I distr \
    distr/flecs.c docs/playground/main.c \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s STACK_SIZE=1mb \
    -s EXPORTED_RUNTIME_METHODS=cwrap \
    -s EXPORTED_FUNCTIONS=_main,_flecs_explorer_request \
    -s MODULARIZE=1 \
    -s EXPORT_NAME=$NAME \
    -s ENVIRONMENT=web \
    -o "$OUT/$NAME.js"

echo "playground: built $OUT/$NAME.js and $OUT/$NAME.wasm"
