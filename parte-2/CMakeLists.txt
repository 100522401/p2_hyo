# Versión mínima de CMake requerida para este proyecto.
cmake_minimum_required(VERSION 3.10)

# Definición del proyecto.
# Se llamará 'GraphParser' y usará el compilador de C++.
project(GraphParser CXX)

# Establece el estándar de C++ a C++17.
# Puedes cambiarlo a 11, 14, 20, etc., según necesites.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Nombre del ejecutable que se generará.
set(EXECUTABLE_NAME pathfinder)

# Busca de forma automática todos los archivos con extensión .cpp
# dentro del directorio 'src' y los guarda en la variable SOURCES.
file(GLOB SOURCES "src/*.cpp")

# Crea el ejecutable con el nombre definido anteriormente y los
# archivos fuente encontrados.
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Añade el directorio 'include' a las rutas de búsqueda de cabeceras
# para nuestro ejecutable.
# Esto permite hacer #include <mi_header.h> desde los archivos de 'src'.
# PUBLIC: Significa que si otra librería enlaza contra este target,
# también heredará este directorio de inclusión.
target_include_directories(${EXECUTABLE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
