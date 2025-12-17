#!/usr/bin/env python3
import parte_1 as script
import random


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


if __name__ == "__main__":
    # Ejemplo de uso: generar un tablero 4x4 con un 50% de ocupación
    board_size = 4
    occupancy = 0.5
    random_seed = 123
    generated_board = generate_random_board(board_size, occupancy, random_seed)

    # Imprimir el tablero generado usando la función de parte_1.py
    print(
        f"Tablero generado ({board_size}x{board_size}, {occupancy*100}% lleno, semilla={random_seed}):"
    )
    script.print_board(generated_board)
