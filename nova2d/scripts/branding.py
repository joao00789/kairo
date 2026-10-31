#!/usr/bin/env python3
"""Troca o nome da engine em version.py. Uso: branding.py <raiz_do_godot> "Nome" nome_curto"""
import re, sys, pathlib

root = pathlib.Path(sys.argv[1])
name = sys.argv[2] if len(sys.argv) > 2 else "Nova2D Engine"
short = sys.argv[3] if len(sys.argv) > 3 else "nova2d"

p = root / "version.py"
s = p.read_text(encoding="utf-8")
s = re.sub(r'^short_name = ".*"$', f'short_name = "{short}"', s, flags=re.M)
s = re.sub(r'^name = ".*"$', f'name = "{name}"', s, flags=re.M)
p.write_text(s, encoding="utf-8")
print(f"[branding] name={name!r} short_name={short!r}")
