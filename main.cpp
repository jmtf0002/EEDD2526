#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

#include "ListaEnlazada.h"
#include "VDinamico.h"
#include "PaMedicamento.h"

/**  @author Javier Martínez González mgg00000@red.ujaen.es
     @author Jose María Torraleja Franco jmtf0002@red.ujaen.es */




int main(int argc, const char * argv[]) {
    ListaEnlazada<int> lista;

    // 1) Insertar al final 101..200
    for (int i = 101; i <= 200; ++i) {
        int v = i;
        lista.insertarFinal(v);
    }

    std::cout << "Después de insertar 101..200 al final:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 2) Insertar por el comienzo 98..1 (decreciente)
    for (int i = 98; i >= 1; --i) {
        int v = i;
        lista.insertarInicio(v);
    }

    std::cout << "Después de insertar 98..1 al inicio:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 3) Insertar 100 delante del 101
    {
        auto it = lista.iteradorInicio();
        bool found = false;
        while (it.haySiguiente()) {
            if (it.dato() == 101) { found = true; break; }
            it.siguiente();
        }
        if (found) {
            int val100 = 100;
            lista.insertarDelante(it, val100);
        } else {
            std::cerr << "No se encontró 101 para insertar delante.\n";
        }
    }

    std::cout << "Después de insertar 100 delante de 101:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 4) Insertar 99 detrás del 98
    {
        auto it = lista.iteradorInicio();
        bool found = false;
        while (it.haySiguiente()) {
            if (it.dato() == 98) { found = true; break; }
            it.siguiente();
        }
        if (found) {
            int val99 = 99;
            lista.insertarDetras(it, val99);
        } else {
            std::cerr << "No se encontró 98 para insertar detrás.\n";
        }
    }

    std::cout << "Después de insertar 99 detrás de 98:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 5) Borrar los 10 primeros y los 10 últimos
    for (int i = 0; i < 10; ++i) lista.borrarInicio();
    for (int i = 0; i < 10; ++i) lista.borrarFinal();

    std::cout << "Después de borrar 10 primeros y 10 últimos:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 6) Borrar todos los múltiplos de 10
    {
        auto it = lista.iteradorInicio();
        while (it.haySiguiente()) {
            int val = it.dato();
            if (val % 10 == 0) {
                ListaEnlazada<int>::Iterador toDel = it;
                it.siguiente();
                lista.borrar(toDel);
            } else {
                it.siguiente();
            }
        }
    }

    std::cout << "Después de borrar múltiplos de 10:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    return 0;
}
