#!/bin/sh
# Builds the Flecs documentation website into docs/html.
#
# Requires Doxygen 1.18 or newer, Python 3 and Emscripten (for the wasm image
# behind the "Flecs playground" page, see docs/playground). Set DOXYGEN to use a
# specific doxygen binary. Run from any directory.

set -e
cd "$(dirname "$0")/.."
DOXYGEN="${DOXYGEN:-doxygen}"
python3 docs/cfg/examples.py
"$DOXYGEN" docs/cfg/Doxyfile
python3 docs/cfg/postprocess.py
docs/playground/build.sh docs/html
