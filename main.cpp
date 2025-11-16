#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>

#include "MediExpress.h"
#include "Farmacia.h"
#include "PaMedicamento.h"
#include "Laboratorio.h"

/**
 * @brief Parsea una línea de CSV, manejando comillas.
 * (Función auxiliar necesaria para el constructor de MediExpress)
 */
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
    campos.push_back(campo_actual);
    for (std::string& campo : campos) {
        if (!campo.empty() && campo.back() == '\r') {
            campo.pop_back();
        }
    }
    return campos;
}


/** @author  Javier Martínez González jmg00144@red.ujaen.es
      @author José María Torraleja Franco jmtf0002@red.ujaen.es
      */
int main() {

    const std::string archivo_meds = "data/pa_medicamentos.csv";
    const std::string archivo_labs = "data/laboratorios.csv";
    const std::string archivo_farma = "data/farmacias.csv";

    // IDs de los medicamentos
    const int OXIDO_ID = 3640;
    const int CARBONATO_ID = 3632;
    const int CLORURO_ID = 3633;
    const int CIANURO_ID = 9355;
    const int OTRO_ID_BORRAR = 3244;
    const int GRIPE_ID = 997;

    std::cout << "Cargando MediExpress..." << std::endl;
    MediExpress mediExpress(archivo_meds, archivo_labs, archivo_farma);
    std::cout << "Carga completada." << std::endl << std::endl;

    PaMedicamento* med_comprado = nullptr; // Variable de salida para comprarMedicam

    // --- Ejercicio 1: Comprar magnesio en Sevilla ---
    std::cout << "Ejercicio 1: Comprar magnesio en Sevilla" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::vector<Farmacia*> farmacias_sevilla = mediExpress.buscarFarmacias("SEVILLA");
    std::cout << "Numero de farmacias en Sevilla: " << farmacias_sevilla.size() << std::endl << std::endl;

    for (Farmacia* f : farmacias_sevilla) {
        std::cout << "=== Farmacia: " << f->getNombre() << " ===" << std::endl;

        // 12 personas van a comprar
        for (int i = 1; i <= 12; ++i) {

            // Lógica de compra:
            // 1. Intenta OXIDO (3640)
            if (f->buscaMedicamID(OXIDO_ID) >= 1) {
                f->comprarMedicam(OXIDO_ID, 1, med_comprado);
                std::cout << "La persona " << i << " ha comprado una unidad de OXIDO DE MAGNESIO" << std::endl;
            }
            // 2. Si no, intenta CARBONATO (3632)
            else if (f->buscaMedicamID(CARBONATO_ID) >= 1) {
                f->comprarMedicam(CARBONATO_ID, 1, med_comprado);
                std::cout << "La persona " << i << " ha comprado una unidad de CARBONATO DE MAGNESIO" << std::endl;
            }
            // 3. Si no, intenta CLORURO (3633)
            else if (f->buscaMedicamID(CLORURO_ID) >= 1) {
                f->comprarMedicam(CLORURO_ID, 1, med_comprado);
                std::cout << "La persona " << i << " ha comprado una unidad de CLORURO DE MAGNESIO" << std::endl;
            }
            // 4. Si no hay de ninguno, no compra y se solicitan pedidos
            else {
                std::cout << "La persona " << i << " no ha podido comprar ningun medicamento" << std::endl;

                // La llamada a comprarMedicam con stock 0 disparará el pedido (pedidoMedicam)
                // Pedimos 10 unidades para simular un restock
                f->comprarMedicam(OXIDO_ID, 10, med_comprado);
                f->comprarMedicam(CARBONATO_ID, 10, med_comprado);
                f->comprarMedicam(CLORURO_ID, 10, med_comprado);
            }
        }
        std::cout << std::endl; // Espacio entre farmacias
    }


    // --- Ejercicio 2: Farmacias en Madrid con VIRUS ---
    std::cout << "Ejercicio 2: Farmacias en Madrid con VIRUS" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::vector<Farmacia*> farmacias_madrid = mediExpress.buscarFarmacias("MADRID");
    std::cout << "Numero de farmacias en MADRID: " << farmacias_madrid.size() << std::endl << std::endl;

    for (Farmacia* f : farmacias_madrid) {
        std::cout << "=== Farmacia: " << f->getNombre() << " ===" << std::endl;
        std::vector<PaMedicamento*> meds_virus = f->buscaMedicamNombre("VIRUS");

        std::cout << "Numero de VIRUS: " << meds_virus.size() << std::endl;
        if (meds_virus.size() > 0) {
            for (PaMedicamento* med : meds_virus) {
                std::cout << med->get_nombre() << std::endl;
            }
        }
        std::cout << std::endl; // Espacio entre farmacias
    }


    // --- Ejercicio 3: Eliminar CIANURO ---
    std::cout << "Ejercicio 3: Eliminar CIANURO" << std::endl;
    std::cout << "=========================================" << std::endl;

    if (mediExpress.eliminarMedicamento(CIANURO_ID)) {
        std::cout << "Se ha eliminado el CIANURO" << std::endl;
    } else {
        std::cout << "Error: El CIANURO (ID 9355) no se pudo eliminar." << std::endl;
    }

    std::cout << "Procedemos a buscar el CIANURO..." << std::endl;
    if (mediExpress.buscarCompuesto(CIANURO_ID) == nullptr) {
        std::cout << "El CIANURO se ha eliminado correctamente" << std::endl;
    } else {
        std::cout << "ERROR: El CIANURO (ID 9355) sigue en el sistema." << std::endl;
    }

    if (!mediExpress.eliminarMedicamento(OTRO_ID_BORRAR)) {
        std::cout << "No se ha encontrado el medicamento con ID 3244" << std::endl;
    } else {
        std::cout << "Se ha eliminado el medicamento con ID 3244" << std::endl;
    }
    std::cout << std::endl;


    // --- Ejercicio 4: Gripe en Madrid ---
    std::cout << "Ejercicio 4: Gripe en Madrid" << std::endl;
    std::cout << "=========================================" << std::endl;

    const int INCREMENTO_GRIPE = 20;
    const int STOCK_OBJETIVO = 30; // 10 iniciales + 20 de incremento

    PaMedicamento* med_gripe = mediExpress.buscarCompuesto(GRIPE_ID);

    // 1. Incrementar el stock
    if (med_gripe) {
        for (Farmacia* f : farmacias_madrid) {
            f->nuevoStock(med_gripe, INCREMENTO_GRIPE);
        }
    } else {
        std::cout << "Error: No se encontro el medicamento de la gripe ID 997" << std::endl;
    }

    // 2. Listar las que tengan 30 unidades
    for (Farmacia* f : farmacias_madrid) {
        if (f->buscaMedicamID(GRIPE_ID) == STOCK_OBJETIVO) {
            std::cout << f->getNombre() << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "--- Pruebas finalizadas ---" << std::endl;
    return 0;
}