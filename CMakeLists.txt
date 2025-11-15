# Versión mínima de CMake (puedes usar la que tengas)
cmake_minimum_required(VERSION 3.20)

# Nombre de tu proyecto
project(EEDD2526)

# Usa el estándar C++ 17 (o 11, 14, etc.)
set(CMAKE_CXX_STANDARD 20)

# Aquí está la línea clave.
# Lista TODOS tus archivos .cpp, y NINGÚN archivo .h
add_executable(EEDD2526
        main.cpp
        MediExpress.cpp
        Farmacia.cpp
)