import constraint
import sys


def parser(route):
    matrix = []
    with open(route, "r") as f:
        for line in f:
            line = line.strip()
            if line == "":
                continue
            for character in line:
                if character not in {'.', 'X', 'O'}:
                    raise ValueError("Existen valores no válidos en el archivo .in")
            matrix.append(line)

    #Matrix validations
    n = len(matrix)
    for i in range(n):
        if len(matrix[i]) != n:
            raise ValueError("La matriz no es cuadrada")
        
    if n%2 != 0:
        raise ValueError("El tamaño de la matriz debe ser par")
    return matrix


def equalAppearances(*values):
    values = list(values)
    count_X = values.count('X')
    count_O = values.count('O')
    count_empty = values.count('None')

    # Caso en el que la linea este llena 
    if count_empty == 0:
        return count_O == count_X

    #caso en el que más de la mitad de la línea este formada por un valor
    n = len(values)
    half = n/2
    if count_X > half or count_O > half:
        return False
    
    #caso no definido, sigue formando soluciones
    return True


def maxThreeOnARow(*values):
    values = list(values)
    count = 0
    last = None
    for value in values:
        #caso no definido, sigue contando
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

    n = len(matrix)
    line = "+---" * n + "+"

    print(line)
    for i in range(n):
        row = "|"
        for j in range(n):
            if matrix[i][j] == '.':
                row += "   |"
            else:
                row += f" {matrix[i][j]} |"
        print(row)
    print(line)

def write_board(matrix, f):
    n = len(matrix)
    line = "+---" * n + "+"

    f.write(line + "\n")
    for i in range(n):
        row = "|"
        for j in range(n):
            if matrix[i][j] == '.':
                row += "   |"
            else:
                row += f" {matrix[i][j]} |"
        f.write(row + "\n")
    f.write(line + "\n")

def write_solution(solution, n, f):
    # Crear matriz vacía
    matrix = [['' for _ in range(n)] for _ in range(n)]
    
    # Llenar matriz con el diccionario del solver
    for (i, j), value in solution.items():
        matrix[i][j] = value

    # Imprimir usando tu mismo método
    write_board(matrix, f) 

def main():
    if len(sys.argv) != 3:
        print("Uso: python parte-1.py <fichero-entrada> <fichero-salida>")
        sys.exit(1)

    route_in = sys.argv[1]
    route_out = sys.argv[2]

    matrix = parser(route_in)
    
    # create a new problem
    manager = constraint.Problem()

    #variables
    domain_X = ['X']
    domain_O = ['O']
    domain_empty = ['X', 'O']
    
    n = len(matrix)
    variables = []

    for i in range(n):
        for j in range(n):
            variable = (i, j)
            variables.append(variable)
            if matrix[i][j] == 'X':
                manager.addVariable(variable, domain_X)
            elif matrix[i][j] == 'O':
                manager.addVariable(variable, domain_O)
            else:
                manager.addVariable(variable, domain_empty)

    #constraints
    for i in range(n):
        row_vars = [(i, j) for j in range(n)]
        manager.addConstraint(equalAppearances, row_vars)
        manager.addConstraint(maxThreeOnARow, row_vars)

    for j in range(n):
        col_vars = [(i, j) for i in range(n)]
        manager.addConstraint(equalAppearances, col_vars)
        manager.addConstraint(maxThreeOnARow, col_vars)

    # Soluciones
    solutions = manager.getSolutions()
    print_board(matrix)
    print(f"Se han encontrado {len(solutions)} soluciones.")
    
    with open(route_out, "w") as f:
        write_board(matrix, f)
        f.write('\n')
        write_solution(solutions[0], n, f)
    

if __name__ == "__main__":
    main()
