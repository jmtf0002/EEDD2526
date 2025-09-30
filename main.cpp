#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>

#include "PaMedicamento.h"
#include "VDinamico.h"   // tu implementación de plantilla (header + impl included)

int main(int argc, const char * argv[]) {
    std::ifstream is("pa_medicamentos.csv");
    if (!is.is_open()) {
        std::cerr << "Error de apertura en archivo pa_medicamentos.csv\n";
        return 1;
    }

    VDinamico<PaMedicamento> vectorMedic; // vector dinámico que iremos llenando
    std::string fila;
    int contador = 0;

    clock_t t_ini = clock();

    std::stringstream columnas;

    while (std::getline(is, fila)) {
        if (fila.empty()) continue;

        // preparar stringstream
        columnas.clear();
        columnas.str(fila);

        std::string id_number_s, id_alpha, nombre;
        std::getline(columnas, id_number_s, ';');
        std::getline(columnas, id_alpha, ';');
        std::getline(columnas, nombre, ';');

        // quitar posible '\r' final en nombre o id_alpha
        if (!nombre.empty() && nombre.back() == '\r') nombre.pop_back();
        if (!id_alpha.empty() && id_alpha.back() == '\r') id_alpha.pop_back();
        if (!id_number_s.empty() && id_number_s.back() == '\r') id_number_s.pop_back();

        try {
            PaMedicamento p(id_number_s, id_alpha, nombre);
            vectorMedic.insertar(p); // inserta al final
            ++contador;
        } catch (const std::exception& e) {
            std::cerr << "Fila " << (contador+1) << " ignorada por error: " << e.what() << " -> '" << fila << "'\n";
            // no incrementamos contador de elementos insertados en caso de error
        }
    }

    is.close();

    double elapsed = (clock() - t_ini) / (double)CLOCKS_PER_SEC;

    std::cout << "Leidos e insertados en vector: " << vectorMedic.tamLog() << " elementos.\n";
    std::cout << "Tiempo lectura+insercion: " << elapsed << " segs.\n\n";

    // Mostrar identificadores (id_num e id_alpha) de los primeros 50 elementos (o menos si hay menos)
    unsigned int toshow = std::min<unsigned int>(50u, vectorMedic.tamLog());
    std::cout << "Primeros " << toshow << " identificadores (id_num ; id_alpha):\n";
    for (unsigned int i = 0; i < toshow; ++i) {
        const PaMedicamento &pm = vectorMedic[i];
        std::cout << (i+1) << ": " << pm.getIdNum() << " ; " << pm.getIdAlpha() <<" ; "<< pm.getNombre() << "\n";
    }

    if (toshow == 0) std::cout << "(vector vacío)\n";

    // Ordenar por id_num (PaMedicamento::operator<)
    if (vectorMedic.tamLog() > 1) {
        vectorMedic.ordenar();
    }

    // Mostrar los primeros 50 elementos (o menos)
    unsigned int toshow2 = std::min<unsigned int>(50u, vectorMedic.tamLog());
    std::cout << "Mostrando datos de los primeros " << toshow << " medicamentos (id_num ; id_alpha ; nombre):\n\n";
    for (unsigned int i = 0; i < toshow; ++i) {
        const PaMedicamento &pm = vectorMedic[i];
        std::cout << (i+1) << ": "
                  << pm.getIdNum() << " ; "
                  << pm.getIdAlpha() << " ; "
                  << pm.getNombre() << "\n";
    }

    if (toshow == 0) std::cout << "(vector vacío)\n";


    return 0;
}
