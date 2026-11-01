#!/usr/bin/env bash
# Compila o EDITOR rodando no Android (tablet/celular). Experimental.
set -euo pipefail
cd "$(dirname "$0")/../.."
: "${ANDROID_HOME:?defina ANDROID_HOME}"
scons platform=android target=editor arch=arm64 module_kairo_enabled=yes generate_android_binaries=yes -j"$(nproc)"
