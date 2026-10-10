#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p bin
compiler="${CXX:-g++}"
flags=(-std=c++17 -Wall -Wextra -Wpedantic -O2 -I include)
for suite in core physics interaction; do
  "$compiler" "${flags[@]}" "tests/test_${suite}.cpp" -o "bin/test_${suite}"
  "bin/test_${suite}"
done
"$compiler" "${flags[@]}" src/console.cpp -o bin/spin_console
if [[ "${1:-}" == '--core-only' ]]; then exit 0; fi
sfml_flags=()
sfml_root="${SNL_SFML_ROOT:-$PWD/.deps/sfml/usr}"
if [[ -f "$sfml_root/include/SFML/Graphics.hpp" ]]; then
  sfml_lib="$sfml_root/lib/x86_64-linux-gnu"
  if [[ ! -d "$sfml_lib" ]]; then sfml_lib="$sfml_root/lib"; fi
  sfml_flags=(-Wl,--disable-new-dtags -I "$sfml_root/include" -L "$sfml_lib" "-Wl,-rpath,$sfml_lib" '-Wl,-rpath,$ORIGIN/lib')
fi
"$compiler" "${flags[@]}" main.cpp src/Application/DesktopApplication.cpp src/Graphics/*.cpp \
  "${sfml_flags[@]}" -lsfml-graphics -lsfml-window -lsfml-system -o bin/spin_network
printf '%s\n' 'Compilacion lista. Ejecuta ./bin/spin_network'
