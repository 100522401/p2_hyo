#!/usr/bin/env python3
import os
import sys
import subprocess
from pathlib import Path

def main():
    if len(sys.argv) != 5:
        print("Uso: ./programa <vertice-1> <vertice-2> <nombre-del-mapa> <fichero-salida>")
        return 1

    u = sys.argv[1]
    v = sys.argv[2]
    map_name = sys.argv[3]
    output_file = sys.argv[4]

    # Comprobación existencia binario
    root = Path(__file__).resolve().parent
    exe = root / "build" / "graph_parser"
    
    if not exe.exists():
        print(f"Error: no existe el binario: {exe}", file=sys.stderr)
        return 1
    
    # ejecutar: ./parte-2 u v map_name out_file
    cmd = [str(exe), u, v, map_name, output_file]
    return subprocess.call(cmd)

if __name__ == "__main__":
    raise SystemExit(main())