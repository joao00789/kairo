#!/usr/bin/env bash
# Gera os templates Android 2D (release + debug) em bin/. Requer: SDK/NDK Android, JDK 17, scons.
set -euo pipefail
cd "$(dirname "$0")/../.."   # raiz do código-fonte
: "${ANDROID_HOME:?defina ANDROID_HOME (SDK do Android)}"
for T in template_release template_debug; do
  scons platform=android target=$T arch=arm64 profile=kairo/engine/android_template_2d.py \
        generate_android_binaries=$([ "$T" = template_release ] && echo yes || echo no) -j"$(nproc)"
done
echo "Templates em: bin/ (e AAR/APK em platform/android/java/lib/build)"
