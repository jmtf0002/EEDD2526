cmake_minimum_required(VERSION 3.20)

# Define el nombre del proyecto e indica que es un proyecto de C++ (CXX)
project(EEDD2526_P4 LANGUAGES CXX)

# Configura el estándar de C++ (C++17 es recomendado para STL)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Buena práctica para evitar extensiones no estándar

# Agrega el ejecutable y lista TODOS tus archivos .cpp
# Los archivos .h NO se listan aquí.
add_executable(EEDD2526
        main.cpp
        MediExpress.cpp
        Farmacia.cpp
        Stock.cpp
        # NOTA: Si Laboratorio o PaMedicamento tuvieran sus propios
        # archivos .cpp, deberías añadirlos aquí también.
        # Por ahora, asumo que no los tienen.
)