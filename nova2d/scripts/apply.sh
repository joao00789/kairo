#!/usr/bin/env bash
# Aplica o overlay Nova2D sobre o código-fonte do Godot.
# Uso: ./scripts/apply.sh /caminho/para/godot-master ["Nome da Engine"] [nome_curto]
set -euo pipefail
GODOT="${1:?informe o caminho do godot-master}"
NAME="${2:-Nova2D Engine}"
SHORT="${3:-nova2d}"
HERE="$(cd "$(dirname "$0")/.." && pwd)"

[ -f "$GODOT/SConstruct" ] || { echo "SConstruct não encontrado em $GODOT"; exit 1; }

cp -r "$HERE/modules/nova2d" "$GODOT/modules/"
mkdir -p "$GODOT/nova2d"
cp -r "$HERE/engine" "$HERE/scripts" "$HERE/project_template" "$HERE/docs" "$GODOT/nova2d/"
python3 "$HERE/scripts/branding.py" "$GODOT" "$NAME" "$SHORT"
echo "[ok] Overlay aplicado em $GODOT"
