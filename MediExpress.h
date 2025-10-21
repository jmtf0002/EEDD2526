#ifndef MEDIEXPRESS_H
#define MEDIEXPRESS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "VDinamico.h"
#include "ListaEnlazada.h"
#include "PaMedicamento.h"
#include "Laboratorio.h"

// Asumimos que la función parsearFilaCSV está definida aquí o en un archivo auxiliar
std::vector<std::string> parsearFilaCSV(const std::string& linea);

class MediExpress {
private:
    // Almacenamos punteros a los objetos, no los objetos mismos
    VDinamico<PaMedicamento*> principiosActivos;
    ListaEnlazada<Laboratorio*> laboratorios;

public:
    // --- Constructor y Destructor ---
    MediExpress(const std::string& archivo_meds, const std::string& archivo_labs);
    ~MediExpress();

    // --- Métodos de Consulta (ahora devuelven colecciones de punteros) ---
    VDinamico<PaMedicamento*> buscarCompuesto(const std::string& nombre) const;
    ListaEnlazada<Laboratorio*> buscarLabCiudad(const std::string& ciudad) const;
    ListaEnlazada<Laboratorio*> buscarLabsPorCompuesto(const std::string& compuesto) const;

    void imprimirMedicamentosPorLaboratorio(int idLab) const;

    // --- Métodos de Operación ---
    void asignarMedsSinLabAMadrid();

    // --- Métodos de Verificación ---
    unsigned int totalMedicamentos() const { return principiosActivos.tamlog(); }
    int totalLaboratorios() const { return laboratorios.tam(); }
    int contarMedicamentosSinLab() const;

    int eliminarLabsPorCiudad(const std::string& ciudad);
};

// --- Implementación del Constructor (adaptado para punteros) ---
MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs) {
    // PASO 1: Leer medicamentos y crear objetos con 'new'
    std::ifstream is_meds(archivo_meds);
    if (is_meds.is_open()) {
        std::string fila;
        while (std::getline(is_meds, fila)) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            if (!campos.empty() && campos.back().empty()) campos.pop_back();
            if (campos.size() >= 3) {
                try {
                    PaMedicamento* p = new PaMedicamento(std::stoi(campos[0]), campos[1], campos[2]);
                    principiosActivos.insertar(p, principiosActivos.tamlog());
                } catch (const std::exception&) {}
            }
        }
        is_meds.close();
    }

    // PASO 2: Leer laboratorios y crear objetos con 'new'
    std::ifstream is_labs(archivo_labs);
    if (is_labs.is_open()) {
        std::string fila;
        while (std::getline(is_labs, fila)) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            if (campos.size() == 5) {
                try {
                    Laboratorio* l = new Laboratorio(std::stoi(campos[0]), campos[1], campos[2], campos[3], campos[4]);
                    laboratorios.insertarFinal(l);
                } catch (const std::exception&) {}
            }
        }
        is_labs.close();
    }

    // PASO 3: Enlazar (ahora trabajamos directamente con los punteros almacenados)
    auto it_lab = laboratorios.iteradorInicio();
    if (it_lab.haySiguiente()) {
        for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
            if (!it_lab.haySiguiente()) break;
            principiosActivos[i]->servidoPor(it_lab.dato());
            if (i % 2 == 1) {
                it_lab.siguiente();
            }
        }
    }
}

// --- Destructor: Imprescindible para liberar la memoria creada con 'new' ---
MediExpress::~MediExpress() {
    // Liberar memoria de los medicamentos
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        delete principiosActivos[i];
    }

    // Liberar memoria de los laboratorios
    auto it = laboratorios.iteradorInicio();
    while (it.haySiguiente()) {
        delete it.dato();
        it.siguiente();
    }
}


// --- Implementación de los Métodos (adaptados para punteros) ---

int MediExpress::contarMedicamentosSinLab() const {
    int contador = 0;
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        if (principiosActivos[i]->getLaboratorio() == nullptr) {
            contador++;
        }
    }
    return contador;
}

ListaEnlazada<Laboratorio*> MediExpress::buscarLabCiudad(const std::string& ciudad) const {
    ListaEnlazada<Laboratorio*> resultados;
    auto it = laboratorios.iteradorInicio();
    while (it.haySiguiente()) {
        if (it.dato()->getLocalidad().find(ciudad) != std::string::npos) {
            resultados.insertarFinal(it.dato()); // it.dato() ya es un puntero
        }
        it.siguiente();
    }
    return resultados;
}

VDinamico<PaMedicamento*> MediExpress::buscarCompuesto(const std::string& nombre) const {
    VDinamico<PaMedicamento*> resultados;
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        if (principiosActivos[i]->get_nombre().find(nombre) != std::string::npos) {
            resultados.insertar(principiosActivos[i], resultados.tamlog());
        }
    }
    return resultados;
}

ListaEnlazada<Laboratorio*> MediExpress::buscarLabsPorCompuesto(const std::string& compuesto) const {
    ListaEnlazada<Laboratorio*> labs_encontrados;
    VDinamico<PaMedicamento*> meds = buscarCompuesto(compuesto);
    VDinamico<int> ids_ya_agregados;

    for (unsigned int i = 0; i < meds.tamlog(); ++i) {
        Laboratorio* lab = meds[i]->getLaboratorio();
        if (lab) {
            bool encontrado = false;
            for (unsigned int j = 0; j < ids_ya_agregados.tamlog(); ++j) {
                if (ids_ya_agregados[j] == lab->getId()) {
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado) {
                labs_encontrados.insertarFinal(lab);
                ids_ya_agregados.insertar(lab->getId(), ids_ya_agregados.tamlog());
            }
        }
    }
    return labs_encontrados;
}

void MediExpress::imprimirMedicamentosPorLaboratorio(int idLab) const {
    std::cout << "   Medicamentos suministrados por el laboratorio con ID " << idLab << ":" << std::endl;
    bool encontrado = false;
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        Laboratorio* lab = principiosActivos[i]->getLaboratorio();
        if (lab && lab->getId() == idLab) {
            std::cout << "    -> " << principiosActivos[i]->get_nombre() << std::endl;
            encontrado = true;
        }
    }
    if (!encontrado) {
        std::cout << "    (Este laboratorio no suministra ningun medicamento de la lista)" << std::endl;
    }
}

void MediExpress::asignarMedsSinLabAMadrid() {
    VDinamico<PaMedicamento*> meds_sin_lab;
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        if (principiosActivos[i]->getLaboratorio() == nullptr) {
            meds_sin_lab.insertar(principiosActivos[i], meds_sin_lab.tamlog());
        }
    }

    ListaEnlazada<Laboratorio*> labs_madrid = buscarLabCiudad("Madrid");
    auto it_lab = labs_madrid.iteradorInicio();
    int asignaciones = 0;

    for (unsigned int i = 0; i < meds_sin_lab.tamlog(); ++i) {
        if (!it_lab.haySiguiente()) break;

        meds_sin_lab[i]->servidoPor(it_lab.dato());
        asignaciones++;
        it_lab.siguiente();
    }
    // No es necesario buscar el medicamento original, porque ya estamos trabajando con punteros a ellos.
}

// --- Implementación del método de eliminación (CORREGIDO) ---

// --- Implementación del método de eliminación (ADAPTADO A TU 'borrar') ---

int MediExpress::eliminarLabsPorCiudad(const std::string& ciudad) {
    VDinamico<Laboratorio*> labs_a_eliminar;

    // Paso 1: Identificar los punteros de los laboratorios que se van a eliminar.
    // Esta parte no cambia.
    auto it_lab = laboratorios.iteradorInicio();
    while (it_lab.haySiguiente()) {
        if (it_lab.dato()->getLocalidad().find(ciudad) != std::string::npos) {
            labs_a_eliminar.insertar(it_lab.dato(), labs_a_eliminar.tamlog());
        }
        it_lab.siguiente();
    }

    if (labs_a_eliminar.tamlog() == 0) {
        return 0; // No hay laboratorios en esa ciudad para eliminar.
    }

    // Paso 2: Desvincular los medicamentos. Esta parte no cambia.
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        Laboratorio* lab_asignado = principiosActivos[i]->getLaboratorio();
        if (lab_asignado) {
            for (unsigned int j = 0; j < labs_a_eliminar.tamlog(); ++j) {
                if (lab_asignado == labs_a_eliminar[j]) {
                    principiosActivos[i]->servidoPor(nullptr);
                    break;
                }
            }
        }
    }

    // --- PASO 3 CORREGIDO ---
    // Recorremos la lista y borramos los nodos sobre la marcha de forma segura.
    auto it = laboratorios.iteradorInicio();
    while (it.haySiguiente()) {
        bool debe_eliminarse = false;
        // Comprobamos si el laboratorio actual está en la lista de eliminación
        for (unsigned int i = 0; i < labs_a_eliminar.tamlog(); ++i) {
            if (it.dato() == labs_a_eliminar[i]) {
                debe_eliminarse = true;
                break;
            }
        }

        if (debe_eliminarse) {
            // 1. Guardamos el puntero al objeto Laboratorio para borrarlo después
            Laboratorio* ptr_a_borrar = it.dato();

            // 2. Guardamos la posición del siguiente nodo ANTES de borrar el actual
            auto it_siguiente = it;
            it_siguiente.siguiente();

            // 3. Borramos el nodo de la lista usando tu método
            laboratorios.borrar(it);

            // 4. Liberamos la memoria del objeto Laboratorio en sí
            delete ptr_a_borrar;

            // 5. Movemos nuestro iterador a la posición que guardamos
            it = it_siguiente;
        } else {
            // Si no borramos, simplemente avanzamos
            it.siguiente();
        }
    }

    return labs_a_eliminar.tamlog();
}

#endif //MEDIEXPRESS_H