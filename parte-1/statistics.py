#!/usr/bin/env python3
import parte_1 as script
import random
import time
import numpy as np
from typing import List
import matplotlib.pyplot as plt


def generate_random_board(n: int, fill_ratio: float, seed: int) -> list[list[str]]:
    """
    Genera un tablero de Binairo aleatorio de tamaño n x n.

    Args:
        n (int): El tamaño del tablero (debe ser un número par).
        fill_ratio (float): Proporción de casillas a rellenar (entre 0.0 y 1.0).
        seed (int): Semilla para el generador de números aleatorios para
                    reproducibilidad.

    Returns:
        list[list[str]]: Una matriz representando el tablero generado.
                         'X', 'O' para casillas llenas, '.' para vacías.
    """
    if n % 2 != 0:
        raise ValueError("El tamaño del tablero (n) debe ser un número par.")
    if not 0.0 <= fill_ratio <= 1.0:
        raise ValueError("El ratio de llenado (fill_ratio) debe estar entre 0.0 y 1.0.")

    random.seed(seed)

    # Crear una lista con todas las coordenadas posibles
    all_coords = [(r, c) for r in range(n) for c in range(n)]

    # Calcular el número de celdas a rellenar y elegir sus coordenadas
    num_to_fill = int(n * n * fill_ratio)
    fill_coords = random.sample(all_coords, k=num_to_fill)

    # Crear el tablero vacío
    board = [["." for _ in range(n)] for _ in range(n)]

    # Rellenar las coordenadas elegidas con 'X' o 'O'
    for r, c in fill_coords:
        board[r][c] = random.choice(["X", "O"])

    return board


def measure_performance(
    num_runs: int, board_size: int, fill_ratio: float, base_seed: int
) -> np.ndarray:
    """
    Mide el tiempo de ejecución de la resolución de varios tableros de Binairo.

    Args:
        num_runs (int): Número de tableros a generar y resolver.
        board_size (int): Tamaño de los tableros.
        fill_ratio (float): Proporción de llenado de los tableros.
        base_seed (int): Semilla base para la generación de tableros. Se usará
                         una semilla diferente para cada ejecución (base_seed + i).

    Returns:
        np.ndarray: Un array de numpy con los tiempos de ejecución en segundos
                    para cada ejecución.
    """
    execution_times = []
    for i in range(num_runs):
        # Usar una semilla diferente para cada tablero para asegurar variedad
        current_seed = base_seed + i
        board = generate_random_board(board_size, fill_ratio, current_seed)

        start_time = time.perf_counter()
        script.solve(board)
        end_time = time.perf_counter()

        duration = end_time - start_time
        execution_times.append(duration)
        print(
            f"Run {i+1}/{num_runs} ({board_size}x{board_size}, {fill_ratio*100:.0f}%): {duration:.4f}s"
        )

    return np.array(execution_times)


def run_and_plot_size_vs_time(
    board_sizes: List[int], num_runs: int, fill_ratio: float, base_seed: int
):
    """
    Ejecuta mediciones para diferentes tamaños de tablero y genera una gráfica
    de Tamaño vs. Tiempo de ejecución.

    Args:
        board_sizes (List[int]): Lista de tamaños de tablero a probar (e.g., [4, 6, 8]).
        num_runs (int): Número de ejecuciones por cada tamaño de tablero.
        fill_ratio (float): Proporción de llenado a mantener constante.
        base_seed (int): Semilla base para la generación de tableros.
    """
    mean_times = []
    std_devs = []

    print("--- Iniciando experimento: Tamaño de Tablero vs. Tiempo ---")
    for size in board_sizes:
        print(f"\n--- Probando tamaño de tablero: {size}x{size} ---")
        times = measure_performance(num_runs, size, fill_ratio, base_seed)
        mean_times.append(np.mean(times))
        std_devs.append(np.std(times))

    # --- Generación de la gráfica ---
    fig, ax = plt.subplots()

    # Dibujar la línea con los tiempos medios y las barras de error para la desviación
    ax.errorbar(
        board_sizes,
        mean_times,
        yerr=std_devs,
        fmt="-o",
        capsize=5,
        label="Tiempo de ejecución promedio",
    )

    ax.set_xlabel("Tamaño del Tablero (N)")
    ax.set_ylabel("Tiempo de Ejecución (segundos)")
    ax.set_title(
        f"Tiempo de Ejecución vs. Tamaño del Tablero\n"
        f"({num_runs} ejecuciones por tamaño, {fill_ratio*100:.0f}% de ocupación)"
    )
    ax.set_xticks(board_sizes)  # Asegura que todos los tamaños aparezcan en el eje X
    ax.legend()
    plt.tight_layout()

    # Guardar la gráfica en un fichero
    output_filename = "size_vs_time.png"
    plt.savefig(output_filename)
    print(f"\nGráfica guardada como '{output_filename}'")

    plt.show()


def run_and_plot_fill_vs_time(
    fill_ratios: List[float], num_runs: int, board_size: int, base_seed: int
):
    """
    Ejecuta mediciones para diferentes ratios de llenado y genera una gráfica
    de Ocupación vs. Tiempo de ejecución.

    Args:
        fill_ratios (List[float]): Lista de ratios de llenado a probar (e.g., [0.1, 0.2, 0.3]).
        num_runs (int): Número de ejecuciones por cada ratio.
        board_size (int): Tamaño del tablero a mantener constante.
        base_seed (int): Semilla base para la generación de tableros.
    """
    mean_times = []
    std_devs = []

    print("--- Iniciando experimento: Ocupación de Tablero vs. Tiempo ---")
    for ratio in fill_ratios:
        print(f"\n--- Probando ratio de ocupación: {ratio*100:.0f}% ---")
        times = measure_performance(num_runs, board_size, ratio, base_seed)
        mean_times.append(np.mean(times))
        std_devs.append(np.std(times))

    # --- Generación de la gráfica ---
    fig, ax = plt.subplots()

    # Convertir ratios a porcentajes para el eje X
    fill_percentages = [r * 100 for r in fill_ratios]

    # Dibujar la línea con los tiempos medios y las barras de error para la desviación
    ax.errorbar(
        fill_percentages,
        mean_times,
        yerr=std_devs,
        fmt="-o",
        capsize=5,
        label="Tiempo de ejecución promedio",
    )

    ax.set_xlabel("Porcentaje de Ocupación (%)")
    ax.set_ylabel("Tiempo de Ejecución (segundos)")
    ax.set_title(
        f"Tiempo de Ejecución vs. Ocupación del Tablero\n"
        f"(Tablero {board_size}x{board_size}, {num_runs} ejecuciones por ratio)"
    )
    ax.set_xticks(fill_percentages)  # Asegura que todos los porcentajes aparezcan
    ax.legend()
    plt.tight_layout()

    # Guardar la gráfica en un fichero
    output_filename = "fill_vs_time.png"
    plt.savefig(output_filename)
    print(f"\nGráfica guardada como '{output_filename}'")

    plt.show()


if __name__ == "__main__":
    # --- Parámetros para el experimento ---
    # Tamaños de tablero a probar (deben ser pares)
    BOARD_SIZES_TO_TEST = [4, 6, 8]
    NUM_RUNS = 30  # Número de tableros a generar por cada configuración
    BASE_SEED = 42

    # --- Experimento 1: Tamaño vs. Tiempo ---
    # run_and_plot_size_vs_time(BOARD_SIZES_TO_TEST, NUM_RUNS, 0.3, BASE_SEED)

    # --- Experimento 2: Ocupación vs. Tiempo ---
    FILL_RATIOS_TO_TEST = [x / (8) for x in range(1, 8)]
    run_and_plot_fill_vs_time(FILL_RATIOS_TO_TEST, NUM_RUNS, 6, BASE_SEED)
