#!/usr/bin/env bash
# Optional, isolated SFML 2.6.2 runtime/headers for Linux x86_64.
set -euo pipefail
cd "$(dirname "$0")/.."
if [[ "$(uname -m)" != x86_64 ]]; then
  printf '%s\n' 'Este script requiere Linux x86_64. Instala SFML 2.6 para tu arquitectura.' >&2
  exit 1
fi
mkdir -p .deps/downloads .deps/sfml
while read -r checksum file; do
  case "$file" in
    libsfml*) directory=universe/libs/libsfml ;;
    libstb*) directory=universe/libs/libstb ;;
    *) exit 1 ;;
  esac
  path=".deps/downloads/$file"
  if [[ ! -f "$path" ]]; then
    curl --fail --location --retry 2 "https://archive.ubuntu.com/ubuntu/pool/$directory/$file" -o "$path"
  fi
  printf '%s  %s\n' "$checksum" "$path" | sha256sum --check --status
  dpkg-deb -x "$path" .deps/sfml
done < tools/sfml-dependencies.sha256
printf '%s\n' 'SFML 2.6.2 extraido en .deps/sfml. Ejecuta ./tools/build.sh'
