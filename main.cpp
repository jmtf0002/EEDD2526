#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "VDinamico.h"
#include "PaMedicamento.h"

// Función de búsqueda secuencial por subcadena en el nombre
VDinamico<PaMedicamento*> buscarCompuesto(const std::string &comp, VDinamico<PaMedicamento> &vMedicamentos) {
    VDinamico<PaMedicamento*> resultado;
    for (unsigned int i = 0; i < vMedicamentos.tamlog(); ++i) {
        if (vMedicamentos[i].get_nombre().find(comp) != std::string::npos) {
            resultado.insertar(&vMedicamentos[i]);
        }
    }
    return resultado;
}

// Función de ordenación por nombre usando burbuja
void ordenarPorNombreBurbuja(VDinamico<PaMedicamento> &v) {
    unsigned int n = v.tamlog();
    for (unsigned int i = 0; i < n - 1; ++i) {
        for (unsigned int j = 0; j < n - i - 1; ++j) {
            if (v[j].get_nombre() > v[j + 1].get_nombre()) {
                std::swap(v[j], v[j + 1]);
            }
        }
    }
}

// Función para contar repeticiones de la primera palabra en nombre
unsigned int contarPrimerasPalabrasRepetidas(VDinamico<PaMedicamento> &v) {
    std::vector<std::string> primerasPalabras;
    for (unsigned int i = 0; i < v.tamlog(); ++i) {
        std::string nombre = v[i].get_nombre();
        std::stringstream ss(nombre);
        std::string primeraPalabra;
        ss >> primeraPalabra;
        primerasPalabras.push_back(primeraPalabra);
    }
    std::sort(primerasPalabras.begin(), primerasPalabras.end());
    unsigned int count = 0;
    for (unsigned int i = 1; i < primerasPalabras.size(); ++i) {
        if (primerasPalabras[i] == primerasPalabras[i - 1]) {
            count++;
        }
    }
    return count;
}

int main(int argc, const char * argv[]) {
    try {
        VDinamico<PaMedicamento> vectorMedic;
        std::ifstream is;
        std::stringstream columnas;
        std::string fila;

        int id_number = 0;
        std::string id_alpha = "";
        std::string nombre = "";
        // LECTURA DE ARCHIVO
        is.open("data/pa_medicamentos.csv");
        if (is.good()) {
            clock_t t_ini = clock();
            int i = 0;

            while (getline(is, fila)) {
                if (!fila.empty()) {
                    columnas.str(fila);
                    columnas.clear();

                    std::string tmp_id;
                    getline(columnas, tmp_id, ';');
                    getline(columnas, id_alpha, ';');
                    getline(columnas, nombre, ';');

                    id_number = std::stoi(tmp_id);

                    PaMedicamento aux(id_number, id_alpha, nombre);
                    vectorMedic.insertar(aux, i);
                    ++i;
                }
            }

            is.close();
            std::cout << "Tiempo lectura: " << ((clock() - t_ini) / (float)CLOCKS_PER_SEC) << " segs." << std::endl;
        } else {
            std::cout << "Error de apertura en archivo" << std::endl;
            return 1;
        }

        // MOSTRAR PRIMEROS 50 ELEMENTOS SIN ORDENAR
        std::cout << "Primeros 50 medicamentos (sin ordenar):" << std::endl;
        unsigned int tamMostrar = std::min(50u, vectorMedic.tamlog());
        for (unsigned int i = 0; i < tamMostrar; ++i) {
            std::cout << vectorMedic[i].get_id_num() << " - " << vectorMedic[i].get_nombre() << std::endl;
        }
        std::cout << std::endl;

        // ORDENAR POR ID_NUM
        clock_t t_ini = clock();
        vectorMedic.ordenar();

        std::cout << "Primeros 50 medicamentos (ordenados por id_num):" << std::endl;
        for (unsigned int i = 0; i < tamMostrar; ++i) {
            std::cout << vectorMedic[i].get_id_num() << " - " << vectorMedic[i].get_nombre() << std::endl;
        }
        std::cout << "Tiempo ordenación: " << ((clock() - t_ini) / (float)CLOCKS_PER_SEC) << " segs." << std::endl;

        // BUSQUEDA BINARIA
        int idsABuscar[] = {350, 409, 820, 9009, 12370};
        for (int id : idsABuscar) {
            PaMedicamento temp(id, "", "");
            int pos = vectorMedic.busquedaBin(temp);
            if (pos != -1)
                std::cout << "Medicamento con ID " << id << " encontrado en posición " << pos << std::endl;
            else
                std::cout << "Medicamento con ID " << id << " NO encontrado" << std::endl;
        }
        std::cout << std::endl;

        // BUSQUEDA POR SUBCADENA "aceite"
        VDinamico<PaMedicamento*> aceites = buscarCompuesto("aceite", vectorMedic);
        std::cout << "Medicamentos que contienen 'aceite':" << std::endl;
        for (unsigned int i = 0; i < aceites.tamlog(); ++i) {
            std::cout << aceites[i]->get_id_num() << " - " << aceites[i]->get_nombre() << std::endl;
        }
        std::cout << std::endl;

        // ORDENAR POR NOMBRE (burbuja)
        ordenarPorNombreBurbuja(vectorMedic);
        std::cout << "Primeros 50 medicamentos (ordenados por nombre):" << std::endl;
        for (unsigned int i = 0; i < tamMostrar; ++i) {
            std::cout << vectorMedic[i].get_nombre() << std::endl;
        }
        std::cout << std::endl;

        // CONTAR PRIMERAS PALABRAS REPETIDAS
        unsigned int repeticiones = contarPrimerasPalabrasRepetidas(vectorMedic);
        std::cout << "Número de primeras palabras repetidas al menos una vez: " << repeticiones << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
