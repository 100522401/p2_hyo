#!/usr/bin/env python3
import constraint
import sys


def parser(route):
    matrix = []
    valid_values = {".", "X", "O"}
    with open(route, "r") as f:
        for line_idx, line in enumerate(f, start=1):
            # Format the line.
            line = line.strip().upper()

            # Turn 0's into O's for better user experience (0 and O are very similar).
            line = line.replace("0", "O")

            # Line validations
            if line == "":
                continue
            for character in line:
                if character not in valid_values:
                    raise ValueError(
                        f"Valor inválido en la línea {line_idx}: {character}.\n Valores válidos: {valid_values}."
                    )
            # If everythong OK, add line
            matrix.append(line)

    # Matrix validations
    n = len(matrix)  # Number of rows / cols.
    for i in range(n):
        # Case: matrix is not n x n.
        if len(matrix[i]) != n:
            raise ValueError("La matriz no es cuadrada")

    # Case: n is odd.
    if n % 2 != 0:
        raise ValueError("El tamaño de la matriz debe ser par")
    return matrix


def equalAppearances(*values):
    values = list(values)
    count_x = values.count("X")
    count_o = values.count("O")
    count_empty = values.count("None")

    # Case: Line is full
    if count_empty == 0:
        return count_o == count_x

    # Case: more than half of the line is filled with a value.
    n = len(values)
    half = n / 2
    if count_x > half or count_o > half:
        return False

    # Undefined case, keep searching for solutions.
    return True


def maxTwoOnARow(*values):
    values = list(values)
    count = 0
    last = None
    for value in values:
        # Undefined case, keep counting
        if value is None:
            return True

        if value == last:
            count += 1
            if count > 2:
                return False
        else:
            last = value
            count = 1

    return True


def print_board(matrix):
    """Prints the board in a pretty format."""
    n = len(matrix)
    line = "+---" * n + "+"

    print(line)
    for i in range(n):
        row = "|"
        for j in range(n):
            if matrix[i][j] == ".":
                row += "   |"
            else:
                row += f" {matrix[i][j]} |"
        print(row)
    print(line)


def write_board(matrix, f):
    """Writes the board to a file in a pretty format."""
    n = len(matrix)
    line = "+---" * n + "+"

    f.write(line + "\n")
    for i in range(n):
        row = "|"
        for j in range(n):
            if matrix[i][j] == ".":
                row += "   |"
            else:
                row += f" {matrix[i][j]} |"
        f.write(row + "\n")
    f.write(line + "\n")


def write_solution(solution, n, f):
    # Create empty matrix
    matrix = [["" for _ in range(n)] for _ in range(n)]

    # Fill it up with the solver's dictionary
    for (i, j), value in solution.items():
        matrix[i][j] = value

    write_board(matrix, f)


def solve(matrix: list[list[str]]):
    """
    Configura y resuelve el problema de Binairo CSP para una matriz dada.

    Args:
        matrix (list[str]): El tablero de Binairo a resolver.

    Returns:
        list[dict]: Una lista de soluciones encontradas. Cada solución es un
                    diccionario que mapea coordenadas (fila, col) a un valor ('X' o 'O').
    """
    n = len(matrix)
    manager = constraint.Problem()

    # Variables y dominios
    domain_X = ["X"]
    domain_O = ["O"]
    domain_empty = ["X", "O"]

    for i in range(n):
        for j in range(n):
            variable = (i, j)
            if matrix[i][j] == "X":
                manager.addVariable(variable, domain_X)
            elif matrix[i][j] == "O":
                manager.addVariable(variable, domain_O)
            else:
                manager.addVariable(variable, domain_empty)

    # Restricciones
    for i in range(n):
        row_vars = [(i, j) for j in range(n)]
        col_vars = [(j, i) for j in range(n)]
        manager.addConstraint(equalAppearances, row_vars)
        manager.addConstraint(maxTwoOnARow, row_vars)
        manager.addConstraint(equalAppearances, col_vars)
        manager.addConstraint(maxTwoOnARow, col_vars)

    return manager.getSolutions()


def main():
    """Solves the Binairo CSP with python-constraint.
    Params:
        - infile:  path to the file containing the specific problem.
        - outfile: path to the file where the solution will be outputted.
    """
    if len(sys.argv) != 3:
        print("Uso: python parte-1.py <fichero-entrada> <fichero-salida>")
        sys.exit(1)

    # Argparsing
    route_in = sys.argv[1]
    route_out = sys.argv[2]

    matrix = parser(route_in)

    # Resolver el problema
    solutions = solve(matrix)
    n = len(matrix)

    # Imprimir resultados en la consola
    print_board(matrix)
    print(f"Se han encontrado {len(solutions)} soluciones.")

    # Escribir el tablero inicial y la primera solución en el fichero de salida
    with open(route_out, "w") as f:
        write_board(matrix, f)
        f.write("\n")
        if len(solutions) > 0:
            f.write("Solución encontrada:\n")
            write_solution(solutions[0], n, f)


if __name__ == "__main__":
    main()
