#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono> // Para medir el tiempo

// --- Clases de la Práctica ---
#include "MediExpress.h"
#include "Farmacia.h"
#include "VDinamico.h"
#include "AVL.h"
#include "PaMedicamento.h"
#include "Laboratorio.h"
#include "ListaEnlazada.h"

std::vector<std::string> parsearFilaCSV(const std::string& linea) {
    std::vector<std::string> campos;
    std::string campo_actual;
    bool en_campo_con_comillas = false;
    char delimitador = ';';

    for (size_t i = 0; i < linea.length(); ++i) {
        char c = linea[i];

        if (en_campo_con_comillas) {
            // --- 1. Estamos DENTRO de un campo con comillas ---
            if (c == '"') {
                // Encontramos una comilla. ¿Es de cierre o escapada?
                if (i + 1 < linea.length() && linea[i + 1] == '"') {
                    // Es una comilla escapada (""). Añadimos UNA comilla.
                    campo_actual += '"';
                    i++; // Nos saltamos la segunda comilla
                } else {
                    // Es la comilla de cierre.
                    en_campo_con_comillas = false;
                }
            } else {
                // Caracter normal (incluido ';'), lo añadimos tal cual.
                campo_actual += c;
            }
        } else {
            // --- 2. Estamos FUERA de un campo con comillas ---
            if (c == delimitador) {
                // Fin del campo.
                campos.push_back(campo_actual);
                campo_actual.clear(); // Limpiamos para el siguiente
            } else if (c == '"') {
                // Inicio de un campo con comillas.
                // (Solo si el campo está vacío. Si no, es una comilla malformada)
                if (campo_actual.empty()) {
                    en_campo_con_comillas = true;
                } else {
                    campo_actual += c;
                }
            } else {
                // Caracter normal
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

/**
 * @brief Función auxiliar para la búsqueda lineal en el VDinamico.
 * (Necesaria para la Prueba I, Tarea 6)
 */
Farmacia* buscarEnVDinamico(VDinamico<Farmacia>& v, const std::string& cif) {
    for (unsigned i = 0; i < v.tamlog(); ++i) {
        if (v[i].getCif() == cif) {
            return &v[i]; // Devuelve puntero al objeto encontrado
        }
    }
    return nullptr; // No encontrado
}


int main() {
    // --- Archivos de datos ---
    // Asegúrate de que las rutas sean correctas (ej. "data/farmacias.csv")
    const std::string archivo_meds = "data/pa_medicamentos.csv";
    const std::string archivo_labs = "data/laboratorios.csv";
    const std::string archivo_farma = "data/farmacias.csv";

    // --- INICIALIZACIÓN DE MEDIEXPRESS ---
    // El constructor (que has proporcionado) ejecuta toda la carga:
    // Pasos 1-4 de Prueba II (Carga Meds, Labs, Farms-AVL, Enlace Med-Lab)
    // Paso 5 de Prueba II (Asignación cíclica de 100 meds)
    std::cout << "Inicializando MediExpress... (Esto puede tardar)" << std::endl;
    MediExpress mediExpress(archivo_meds, archivo_labs, archivo_farma);
    std::cout << "Inicializacion de MediExpress completada." << std::endl << std::endl;


    // =============================================================
    // --- INICIO DE PROGRAMA DE PRUEBA I (Rendimiento) ---
    // =============================================================

    // --- Tarea 3: Cargar Farmacias en un VDinamico (para comparar) ---
    VDinamico<Farmacia> vFarmacias;
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
                vFarmacias.insertar(f, vFarmacias.tamlog());
            }
        }
        is_farma_vd.close();
    } else {
        std::cerr << "Error critico: No se pudo abrir " << archivo_farma << " para VDinamico." << std::endl;
        return 1;
    }

    // --- Tarea 4: Leer los 500 primeros CIFs ---
    VDinamico<std::string> cif_a_buscar;
    std::ifstream is_farma_cif(archivo_farma);
    int contador_cif = 0;
    if (is_farma_cif.is_open()) {
        while (std::getline(is_farma_cif, fila) && contador_cif < 500) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            if (campos.size() >= 1) {
                cif_a_buscar.insertar(campos[0], cif_a_buscar.tamlog());
                contador_cif++;
            }
        }
        is_farma_cif.close();
    } // (Error de apertura ya comprobado antes)


    // --- Tarea 5-7: Pruebas de tiempo ---
    std::cout << "Tiempo de busqueda farmacias AVL vs. vector" << std::endl;
    std::cout << "====================================" << std::endl;

    // --- Tarea 5: Buscar 500 CIFs en el AVL y cronometrar ---
    auto inicio_avl = std::chrono::high_resolution_clock::now();
    for (unsigned i = 0; i < cif_a_buscar.tamlog(); ++i) {
        mediExpress.buscarFarmacia(cif_a_buscar[i]);
    }
    auto fin_avl = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_avl = fin_avl - inicio_avl;
    std::cout << "Tiempo de lectura AVL: " << tiempo_avl.count() << " segs." << std::endl;


    // --- Tarea 6: Buscar 500 CIFs en el VDinamico y cronometrar ---
    auto inicio_vd = std::chrono::high_resolution_clock::now();
    for (unsigned i = 0; i < cif_a_buscar.tamlog(); ++i) {
        buscarEnVDinamico(vFarmacias, cif_a_buscar[i]);
    }
    auto fin_vd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tiempo_vd = fin_vd - inicio_vd;
    std::cout << "Tiempo de lectura vector: " << tiempo_vd.count() << " segs." << std::endl;
    std::cout << std::endl; // Espacio


    // --- Tarea 8: Mostrar altura del AVL ---
    std::cout << "Altura del arbol AVL" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Altura: " << mediExpress.getAlturaAVLFarmacias() << std::endl;
    std::cout << std::endl; // Espacio


    // --- Tarea 9: Recorrer Inorden y mostrar 100 primeras ---
    std::cout << "\n Recorrido Inorden (CIFs farmacias) ---" << std::endl;
    std::cout << "====================================" << std::endl;

    VDinamico<Farmacia*> farmacias_inorden = mediExpress.getInordenAVLFarmacias();

    int limite = (farmacias_inorden.tamlog() < 100) ? farmacias_inorden.tamlog() : 100;

    for (int i = 0; i < limite; ++i) {

        std::cout << "  " << (i+1) << ". CIF: " << farmacias_inorden[i]->getCif()<<std::endl;
    }


    // =============================================================
    // --- INICIO DE PROGRAMA DE PRUEBA II (Funcionalidad) ---
    // =============================================================

    // --- Accion 1: Crear vector buffer con CIFs ---
    std::vector<std::string> cif_buffer = {
        "37656422V", "46316032N", "77092934Q", "33961602D", "B62351861", "B62351861",
        "B65828113", "46138599R", "35069965W", "37579913Y", "37682300C",
        "37643742X", "46112335A", "47980171D", "38116138D", "46315600V",
        "37640233C", "37931842N", "33964303L", "35022080A", "B66046640",
        "E66748344", "47640201W", "B66621954", "46121385Z", "X6806622W",
        "46046390E"
    };
    const int OXIDO_MAGNESIO_ID = 3640;

    // --- Accion 2: Buscar "ÓXIDO DE MAGNESIO" (ID 3640) y pedir si falta ---
    std::cout << "Buscar OXIDO DE MAGNESIO" << std::endl;
    std::cout << "========================================" << std::endl;

    // --- Primera pasada: Comprobar y pedir ---
    for (const std::string& cif : cif_buffer) {
        Farmacia* farmacia = mediExpress.buscarFarmacia(cif);
        if (farmacia) {
            PaMedicamento* med = farmacia->buscaMedicam(OXIDO_MAGNESIO_ID);
            if (med == nullptr) {
                std::cout << " La farmacia con CIF " << cif << " no tiene el medicamento disponible y se ha solicitado el pedido" << std::endl;
                farmacia->pedidoMedicam(OXIDO_MAGNESIO_ID); // Hace el pedido
            } else {
                std::cout << " La farmacia con CIF " << cif << " tiene el medicamento disponible" << std::endl;
            }
        } else {
            std::cout << " La farmacia con CIF " << cif << " no existe en el sistema." << std::endl;
        }
    }

    // --- Segunda pasada: Volver a comprobar ---
    std::cout << std::endl; // Espacio
    for (const std::string& cif : cif_buffer) {
        Farmacia* farmacia = mediExpress.buscarFarmacia(cif);
        if (farmacia) {
            if (farmacia->buscaMedicam(OXIDO_MAGNESIO_ID) != nullptr) {
                std::cout << "La farmacia con CIF " << cif << " tiene el medicamento disponible" << std::endl;
            } else {
                // Esto no debería pasar si la lógica es correcta
                std::cout << "ERROR: La farmacia con CIF " << cif << " aun no tiene el medicamento." << std::endl;
            }
        }
        // No imprimimos nada si la farmacia no existe (ya se reportó)
    }
    std::cout << std::endl; // Espacio


    // --- Accion 3: Buscar y contar laboratorios que trabajen con "MAGNESIO" ---
    std::cout << "Buscar y contar laboratorios que trabajen con MAGNESIO" << std::endl;
    std::cout << "=================================================" << std::endl;
    // Usamos el método 'buscarLabs' (que seguramente llama a 'buscarLabsPorCompuesto')
    ListaEnlazada<Laboratorio*> labs_magnesio = mediExpress.buscarLabs("MAGNESIO");
    std::cout << "El numero de laboratorios que trabajan con MAGNESIO es: " << labs_magnesio.tam() << std::endl;
    std::cout << std::endl; // Espacio


    // --- Tarea "Para los que trabajan en parejas" ---
    std::cout << "Localizar laboratorios que suministran a las farmacias con VIRUS" << std::endl;
    std::cout << "=================================================" << std::endl;

    for (const std::string& cif : cif_buffer) {
        Farmacia* farmacia = mediExpress.buscarFarmacia(cif);
        if (farmacia) {
            // 1. Usamos el nuevo método de Farmacia para buscar "VIRUS" en su stock
            VDinamico<PaMedicamento*> meds_virus = farmacia->localizarMedicamentosPorNombre("VIRUS");

            std::cout << "Laboratorios (ids) que suministran a la farmacia con CIF: " << cif << " con VIRUS: " << meds_virus.tamlog();

            // 2. Si encontramos, iteramos y mostramos los IDs de los labs
            if (meds_virus.tamlog() > 0) {
                std::cout << " -> ";
                for (unsigned int i = 0; i < meds_virus.tamlog(); ++i) {
                    Laboratorio* lab = meds_virus[i]->getLaboratorio(); // Asume que PaMedicamento tiene getLaboratorio()
                    if (lab) {
                        std::cout << lab->getId() << "/"; // Asume que Laboratorio tiene getId()
                    }
                }
            }
            std::cout << std::endl;
        }
    }

    return 0;
}