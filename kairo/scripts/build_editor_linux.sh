#!/usr/bin/env bash
# Compila o editor de desktop (Linux) para testar a engine.
set -euo pipefail
cd "$(dirname "$0")/../.."
scons platform=linuxbsd target=editor dev_build=yes profile=kairo/engine/editor_dev.py -j"$(nproc)"
echo "Editor em: bin/"
