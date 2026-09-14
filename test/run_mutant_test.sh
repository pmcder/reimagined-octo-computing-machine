#!/bin/sh
# Test command invoked once per mutant by universalmutator's analyze_mutants.
#
# Deliberately compiles src/properties_parse.c, the test file, and Unity
# from scratch every time (rather than reusing CMake's incremental build)
# so a mutant swapped into place is guaranteed to actually be recompiled -
# an incremental build can silently reuse a stale, pre-mutation object
# file and make every mutant look "not killed". Sanitizers match the
# props-parse-tests CMake target so this catches the same bug classes.
set -e

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
UNITY_SRC="$ROOT_DIR/build/_deps/unity-src/src"
BIN="$ROOT_DIR/build/mutation_test_bin"

cc -std=c11 -g -fsanitize=address,undefined \
   -I"$ROOT_DIR/src" \
   -I"$ROOT_DIR/include" \
   -I"$UNITY_SRC" \
   -DFIXTURES_DIR="\"$ROOT_DIR/test/fixtures/\"" \
   -o "$BIN" \
   "$ROOT_DIR/src/properties_parse.c" \
   "$ROOT_DIR/test/test_properties_parse.c" \
   "$UNITY_SRC/unity.c"

"$BIN"
