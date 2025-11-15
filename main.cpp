#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono> // Para medir el tiempo

#include "MediExpress.h"
#include "Farmacia.h"

#include "PaMedicamento.h"
#include "Laboratorio.h"

#include <filesystem>
#include <iostream>




std::vector<std::string> parsearFilaCSV(const std::string& linea) {
    std::vector<std::string> campos;
    std::string campo_actual;
    bool en_campo_con_comillas = false;
    char delimitador = ';';

    for (size_t i = 0; i < linea.length(); ++i) {
        char c = linea[i];

        if (en_campo_con_comillas) {
            if (c == '"') {
                if (i + 1 < linea.length() && linea[i + 1] == '"') {
                    campo_actual += '"';
                    i++;
                } else {
                    en_campo_con_comillas = false;
                }
            } else {
                campo_actual += c;
            }
        } else {
            if (c == delimitador) {
                campos.push_back(campo_actual);
                campo_actual.clear();
            } else if (c == '"') {

                if (campo_actual.empty()) {
                    en_campo_con_comillas = true;
                } else {
                    campo_actual += c;
                }
            } else {

                campo_actual += c;
            }
        }
    }

    // Añadimos el último campo
    campos.push_back(campo_actual);

    // Limpieza final de \r (retorno de carro)
    for (std::string& campo : campos) {
        if (!campo.empty() && campo.back() == '\r') {
            campo.pop_back();
        }
    }

    return campos;
}


Farmacia* buscarEnVDinamico(std::vector<Farmacia>& v, const std::string& cif) {
    for (unsigned i = 0; i < v.size(); ++i) {
        if (v[i].getCif() == cif) {
            return &v[i];
        }
    }
    return nullptr;
}



/**  @author  Javier Martínez González jmg00144@red.ujaen.es
      @author José María Torraleja Franco jmtf0002@red.ujaen.es
      */

int main() {

    const std::string archivo_meds = "data/pa_medicamentos.csv";
    const std::string archivo_labs = "data/laboratorios.csv";
    const std::string archivo_farma = "data/farmacias.csv";


    MediExpress mediExpress(archivo_meds, archivo_labs, archivo_farma);



    //Programa de prueba 1

    std::vector<Farmacia> vFarmacias;
    std::ifstream is_farma_vd(archivo_farma);
    std::string fila;
    if (is_farma_vd.is_open()) {
        while (std::getline(is_farma_vd, fila)) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            if (campos.size() == 6) {
                Farmacia f(campos[0], campos[1], campos[2],
                           campos[3], campos[4], campos[5], &mediExpress);
                vFarmacias.push_back(f);
            }
        }
        is_farma_vd.close();
    } else {
        std::cerr << "Error: No se pudo abrir " << archivo_farma << " para VDinamico." << std::endl;
        return 1;
    }

    // Leer los 500 primeros CIFs
    std::vector<std::string> cif_a_buscar;
    std::ifstream is_farma_cif(archivo_farma);
    int contador_cif = 0;
    if (is_farma_cif.is_open()) {
        while (std::getline(is_farma_cif, fila) && contador_cif < 500) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            if (campos.size() >= 1) {
                cif_a_buscar.push_back(campos[0]);
                contador_cif++;
            }
        }
        is_farma_cif.close();
    }


    //  Pruebas de tiempo
    std::cout << "Tiempo de busqueda farmacias AVL vs. vector" << std::endl;
    std::cout << "====================================" << std::endl;


    auto inicio_avl = std::chrono::high_resolution_clock::now();
    for (unsigned i = 0; i < cif_a_buscar.size(); ++i) {
        mediExpress.buscarFarmacia(cif_a_buscar[i]);
    }
    auto fin_avl = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_avl = fin_avl - inicio_avl;
    std::cout << "Tiempo de lectura AVL: " << tiempo_avl.count() << " segs." << std::endl;


    auto inicio_vd = std::chrono::high_resolution_clock::now();
    for (unsigned i = 0; i < cif_a_buscar.size(); ++i) {
        buscarEnVDinamico(vFarmacias, cif_a_buscar[i]);
    }
    auto fin_vd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_vd = fin_vd - inicio_vd;
    std::cout << "Tiempo de lectura vector: " << tiempo_vd.count() << " segs." << std::endl;
    std::cout << std::endl; // Espacio


    // Mostrar altura del AVL ---
    std::cout << "Altura del arbol AVL" << std::endl;
    std::cout << "====================================" << std::endl;

    std::cout << std::endl; // Espacio


    //  Recorrer Inorden y mostrar 100 primeras ---
    std::cout << "\n Recorrido Inorden (CIFs farmacias) ---" << std::endl;
    std::cout << "====================================" << std::endl;

    std::cout << "NO HAY EVIDENTEMENTE RECORRIDO INORDEN IMPLEMENTADO EN EL AVL," << std::endl;







    // Programa de prueba 2

    // Crear vector buffer con CIFs
    std::vector<std::string> cif_buffer = {
        "37656422V", "46316032N", "77092934Q", "33961602D", "B62351861", "B62351861",
        "B65828113", "46138599R", "35069965W", "37579913Y", "37682300C",
        "37643742X", "46112335A", "47980171D", "38116138D", "46315600V",
        "37640233C", "37931842N", "33964303L", "35022080A", "B66046640",
        "E66748344", "47640201W", "B66621954", "46121385Z", "X6806622W",
        "46046390E"
    };

    const int OXIDO_MAGNESIO_ID = 3640;

    // Buscar "ÓXIDO DE MAGNESIO" y pedir si falta
    std::cout << "Buscar OXIDO DE MAGNESIO" << std::endl;
    std::cout << "========================================" << std::endl;


    for (const std::string& cif : cif_buffer) {
        Farmacia* farmacia = mediExpress.buscarFarmacia(cif);
        if (farmacia) {
            PaMedicamento* med = farmacia->buscaMedicam(OXIDO_MAGNESIO_ID);
            if (med == nullptr) {
                std::cout << " La farmacia con CIF " << cif << " no tiene el medicamento disponible y se ha solicitado el pedido" << std::endl;
                farmacia->pedidoMedicam(OXIDO_MAGNESIO_ID);
            } else {
                std::cout << " La farmacia con CIF " << cif << " tiene el medicamento disponible" << std::endl;
            }
        } else {
            std::cout << " La farmacia con CIF " << cif << " no existe en el sistema." << std::endl;
        }
    }

    // Volver a comprobar
    std::cout << std::endl;
    for (const std::string& cif : cif_buffer) {
        Farmacia* farmacia = mediExpress.buscarFarmacia(cif);
        if (farmacia) {
            if (farmacia->buscaMedicam(OXIDO_MAGNESIO_ID) != nullptr) {
                std::cout << "La farmacia con CIF " << cif << " tiene el medicamento disponible" << std::endl;
            } else {
                std::cout << "ERROR: La farmacia con CIF " << cif << " aun no tiene el medicamento." << std::endl;
            }
        }

    }
    std::cout << std::endl;


    // Buscar y contar laboratorios que trabajen con "MAGNESIO"
    std::cout << "Buscar y contar laboratorios que trabajen con MAGNESIO" << std::endl;
    std::cout << "=================================================" << std::endl;
    std::list<Laboratorio*> labs_magnesio = mediExpress.buscarLabsPorCompuesto("MAGNESIO");
    std::cout << "El numero de laboratorios que trabajan con MAGNESIO es: " << labs_magnesio.size() << std::endl;
    std::cout << std::endl;


    // Buscar farmacias suministradas con "VIRUS"
    std::cout << "Localizar laboratorios que suministran a las farmacias con VIRUS" << std::endl;
    std::cout << "=================================================" << std::endl;

    for (const std::string& cif : cif_buffer) {
        Farmacia* farmacia = mediExpress.buscarFarmacia(cif);
        if (farmacia) {
            std::vector<PaMedicamento*> meds_virus = farmacia->localizarMedicamentosPorNombre("VIRUS");

            std::cout << "Laboratorios (ids) que suministran a la farmacia con CIF: " << cif << " con VIRUS: " << meds_virus.size();

            if (meds_virus.size() > 0) {
                std::cout << " -> ";
                for (unsigned int i = 0; i < meds_virus.size(); ++i) {
                    Laboratorio* lab = meds_virus[i]->getLaboratorio();
                    if (lab) {
                        std::cout << lab->getId() << "/";
                    }
                }
            }
            std::cout << std::endl;
        }
    }

    return 0;
}