#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <list>
#include <fstream> // Necesario para lectura de archivos

#include "MediExpress.h"
#include "Farmacia.h"
#include "Laboratorio.h"
#include "PaMedicamento.h"
#include "ThashMedicam.h"

// Rutas de los archivos de datos
const std::string ARCHIVO_MEDS = "data/pa_medicamentos.csv";
const std::string ARCHIVO_LABS = "data/laboratorios.csv";
const std::string ARCHIVO_FARMA = "data/farmacias.csv";

// --- FUNCIÓN AUXILIAR PARA PARSEAR CSV ---
// (Se mantiene en main como solicitaste)
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
                    campo_actual += '"'; i++;
                } else { en_campo_con_comillas = false; }
            } else { campo_actual += c; }
        } else {
            if (c == delimitador) {
                campos.push_back(campo_actual); campo_actual.clear();
            } else if (c == '"') {
                if (campo_actual.empty()) en_campo_con_comillas = true;
                else campo_actual += c;
            } else { campo_actual += c; }
        }
    }
    campos.push_back(campo_actual);
    for (std::string& campo : campos) if (!campo.empty() && campo.back() == '\r') campo.pop_back();
    return campos;
}

// --- PARTE 1: FUNCIÓN PARA EL ANÁLISIS DE RENDIMIENTO ---
// Usar esta función descomentándola en main para rellenar el Markdown
void pruebaRendimientoTabla(float lambda, ThashMedicam::HashType tipo) {
    std::cout << "\n>>> INICIANDO PRUEBA DE RENDIMIENTO <<<" << std::endl;
    std::cout << "Config: Lambda=" << lambda << " | Tipo=" << (tipo == ThashMedicam::DOUBLE ? "Doble" : "Cuadratica") << std::endl;

    // 1. Contar líneas para dimensionar la tabla
    std::ifstream conteo(ARCHIVO_MEDS);
    int num_lineas = 0; std::string bas;
    while(std::getline(conteo, bas)) num_lineas++;
    conteo.close();

    // 2. Crear Tabla
    ThashMedicam tabla(num_lineas, lambda);
    tabla.setHashType(tipo);

    // 3. Cargar y medir
    std::vector<int> ids_leidos;
    auto start = std::chrono::high_resolution_clock::now();

    std::ifstream is(ARCHIVO_MEDS);
    if(is.is_open()) {
        std::string fila;
        while(std::getline(is, fila)) {
            if(fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);
            if(c.size()>=3) {
                try {
                    int id = std::stoi(c[0]);
                    PaMedicamento pm(id, c[1], c[2]);
                    if(tabla.insertar(id, pm)) ids_leidos.push_back(id);
                } catch(...) {}
            }
        }
        is.close();
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    // 4. Mostrar Resultados
    tabla.mostrarEstadoTabla();
    std::cout << "Tiempo de carga: " << diff.count() << " s" << std::endl;

    std::cout << ">>> FIN PRUEBA <<<\n" << std::endl;
}

// --- MAIN PRINCIPAL ---
int main() {
    // ---------------------------------------------------------
    // PARTE 1: ANÁLISIS (Descomentar para generar datos del Markdown)
    // ---------------------------------------------------------
    // pruebaRendimientoTabla(0.70, ThashMedicam::DOUBLE);
    // return 0;

    // ---------------------------------------------------------
    // PARTE 2: EJECUCIÓN DEL SISTEMA CLÍNICO
    // ---------------------------------------------------------
    std::cout << "=== INICIANDO MEDIEXPRESS ===" << std::endl;
    // Carga de datos
    MediExpress mediExpress(ARCHIVO_MEDS, ARCHIVO_LABS, ARCHIVO_FARMA);

    std::cout << "\nEstado Inicial de la Tabla Hash:" << std::endl;
    mediExpress.mostrarEstadoTablaHash();

    // ---------------------------------------------------------
    // CASO 1: Búsqueda de Compuestos
    // ---------------------------------------------------------
    std::cout << "\n=== CASO 1: BUSQUEDA DE COMPUESTOS ===" << std::endl;
    std::vector<std::string> buscar = {
        "MAGNESIO CLORURO HEXAHIDRATO",
        "CLORURO",
        "ANHIDRO CALCIO CLORURO",
        "LIDOCAINA HIDROCLORURO",
        "MENTA PIPERITA",
        "VIRUS GRIPE"
    };

    for(const auto& nombre : buscar) {
        auto resultados = mediExpress.buscarCompuesto(nombre);
        std::cout << "Buscando '" << nombre << "': " << resultados.size() << " encontrados." << std::endl;
    }

    // ---------------------------------------------------------
    // CASO 2: SEVILLA Y EL MAGNESIO
    // ---------------------------------------------------------
    std::cout << "\n=== CASO 2: SEVILLA Y EL MAGNESIO ===" << std::endl;
    std::vector<Farmacia*> farmaciasSevilla = mediExpress.buscarFarmacias("SEVILLA");
    std::cout << "Farmacias en Sevilla: " << farmaciasSevilla.size() << std::endl;

    int total_comprados = 0;
    int total_pedidos = 0;

    for (Farmacia* f : farmaciasSevilla) {
        for (int i = 0; i < 12; ++i) { // 12 clientes por farmacia
            std::vector<PaMedicamento*> magnesios = mediExpress.buscarCompuesto("MAGNESIO");
            bool comprado = false;

            // Buscar si hay stock de algún magnesio
            for(auto* med : magnesios) {
                if(f->consultarStock(med->get_id_num()) > 0) {
                    PaMedicamento* temp = nullptr;
                    f->comprarMedicam(med->get_id_num(), 1, temp);
                    comprado = true;
                    total_comprados++;
                    break;
                }
            }
            // Si no hay, pedir Óxido de Magnesio (ID 3640)
            if (!comprado) {
                mediExpress.suministrarFarmacia(*f, 3640, 10);
                total_pedidos++;
            }
        }
    }
    std::cout << "Resumen Sevilla: " << total_comprados << " ventas realizadas, "
              << total_pedidos << " pedidos a laboratorios." << std::endl;

    // ---------------------------------------------------------
    // CASO 3: ALERTA EN ÚBEDA
    // ---------------------------------------------------------
    std::cout << "\n=== CASO 3: ALERTA EN UBEDA ===" << std::endl;
    std::vector<Farmacia*> farmaciasJaen = mediExpress.buscarFarmacias("JAEN");
    Farmacia* farmaciaUbeda = nullptr;

    // Buscar farmacia de Úbeda
    for(auto* f : farmaciasJaen) {
        if(f->getLocalidad() == "UBEDA" || f->getDireccion().find("UBEDA") != std::string::npos) {
            farmaciaUbeda = f;
            break;
        }
    }

    if(farmaciaUbeda) {
        std::cout << "Farmacia localizada en: " << farmaciaUbeda->getLocalidad() << std::endl;
        std::string antigeno = "ANTIGENO OLIGOSACARIDO";
        std::vector<PaMedicamento*> listaAntigenos = mediExpress.buscarCompuesto(antigeno);

        std::cout << "Stock ANTES del pedido:" << std::endl;
        for(auto* med : listaAntigenos) {
            std::cout << " - " << med->get_nombre() << ": " << farmaciaUbeda->consultarStock(med->get_id_num()) << std::endl;
        }

        // Pedir 10 unidades de todos los antígenos
        for(auto* med : listaAntigenos) {
            mediExpress.suministrarFarmacia(*farmaciaUbeda, med->get_id_num(), 10);
        }

        std::cout << "Stock DESPUES del pedido:" << std::endl;
        for(auto* med : listaAntigenos) {
            std::cout << " - " << med->get_nombre() << ": " << farmaciaUbeda->consultarStock(med->get_id_num()) << std::endl;
        }
    } else {
        std::cout << "No se encontro farmacia en Ubeda." << std::endl;
    }

    // ---------------------------------------------------------
    // CASO 4: PROHIBICIONES
    // ---------------------------------------------------------
    std::cout << "\n=== CASO 4: PROHIBICION CIANURO Y BISMUTO ===" << std::endl;
    std::vector<std::string> prohibidos = {"CIANURO", "BISMUTO"};

    for(const auto& nombre : prohibidos) {
        std::vector<PaMedicamento*> lista = mediExpress.buscarCompuesto(nombre);
        std::cout << "Procesando " << nombre << ": encontrados " << lista.size() << " tipos." << std::endl;

        // Guardamos IDs antes de borrar para no invalidar iteradores
        std::vector<int> ids_borrar;
        for(auto* m : lista) ids_borrar.push_back(m->get_id_num());

        for(int id : ids_borrar) {
            mediExpress.eliminarMedicamento(id);
        }

        auto check = mediExpress.buscarCompuesto(nombre);
        if(check.empty()) std::cout << " -> Eliminacion correcta." << std::endl;
        else std::cout << " -> ERROR: Aun quedan elementos." << std::endl;
    }

    // ---------------------------------------------------------
    // CASO 5: REDISPERSIÓN (EJERCICIO POR PAREJAS)
    // ---------------------------------------------------------
    std::cout << "\n=== CASO 5: PRUEBA DE REDISPERSION (PAREJAS) ===" << std::endl;

    std::cout << ">> Estado actual de la tabla:" << std::endl;
    mediExpress.mostrarEstadoTablaHash();

    // Bajamos drásticamente el lambda para forzar que el factor de carga actual sea excesivo
    // Esto provocará que 'setLambda' llame internamente a 'redispersar'
    std::cout << ">> Forzando bajada de lambda a 0.1..." << std::endl;
    mediExpress.forzarCambioLambda(0.1f);

    std::cout << ">> Estado DESPUES de forzar (Deberia haber crecido T):" << std::endl;
    mediExpress.mostrarEstadoTablaHash();

    std::cout << "\n=== FIN DE LA PRACTICA ===" << std::endl;
    return 0;
}