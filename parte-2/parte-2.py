#!/usr/bin/env python3
import os
import sys
import subprocess
from pathlib import Path


def main():
    if len(sys.argv) not in [5, 7]:
        print(
            "Uso: ./programa <vertice-1> <vertice-2> <nombre-del-mapa> <fichero-salida>\n",
            "Opcional: --algorithm <algoritmo> (dijkstra | astar | both).\n",
        )
        return 1

    u = sys.argv[1]
    v = sys.argv[2]
    map_name = sys.argv[3]
    # Resolver ruta de los ficheros de mapa.
    map_path = Path(map_name).resolve()
    co_file = Path(map_name + ".co").resolve()
    gr_file = Path(map_name + ".gr").resolve()
    map_paths = [co_file, gr_file]

    # Algoritmo por defecto
    algorithm = "astar"

    # Comprobación existencia mapa
    if not all(map_path.exists() for map_path in map_paths):
        print(f"Error: no existe el mapa: {map_name}", file=sys.stderr)
        return 1

    output_file = sys.argv[4]

    # Comprobación existencia binario
    root = Path(__file__).resolve().parent
    exe = root / "build" / "graph_parser"

    if not exe.exists():
        print(f"[ERROR]: no existe el binario: {exe}", file=sys.stderr)
        return 1

    # ejecutar: ./parte-2 u v map_name out_file
    if len(sys.argv) == 7:
        algorithm = sys.argv[6]

    cmd = [str(exe), u, v, map_path, output_file, "--algorithm", algorithm]
    return subprocess.call(cmd)


if __name__ == "__main__":
    raise SystemExit(main())
