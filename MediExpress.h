#ifndef MEDIEXPRESS_H
#define MEDIEXPRESS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "AVL.h"
#include "VDinamico.h"
#include "ListaEnlazada.h"
#include "PaMedicamento.h"
#include "Laboratorio.h"
#include "Farmacia.h"

std::vector<std::string> parsearFilaCSV(const std::string& linea);

class MediExpress {
private:
    VDinamico<PaMedicamento> principiosActivos;
    ListaEnlazada<Laboratorio> laboratorios;
    AVL<Farmacia> farmacias;
public:
    MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma);
    ~MediExpress();
    VDinamico<PaMedicamento*> buscarCompuesto(const std::string& nombre);
    ListaEnlazada<Laboratorio*> buscarLabCiudad(const std::string& ciudad) const;
    ListaEnlazada<Laboratorio*> buscarLabsPorCompuesto(const std::string& compuesto);
    void imprimirMedicamentosPorLaboratorio(int idLab) const;
    void asignarMedsSinLabAMadrid();
    unsigned int totalMedicamentos() const { return principiosActivos.tamlog(); }
    int totalLaboratorios() const { return laboratorios.tam(); }
    int contarMedicamentosSinLab() const;
    int eliminarLabsPorCiudad(const std::string& ciudad);
    // --- MÉTODOS NUEVOS (de la práctica/UML) ---

    /**
     * @brief Sobrecarga de buscarCompuesto por ID numérico.
     * @return Puntero al PaMedicamento o nullptr si no se encuentra.
     */
    PaMedicamento* buscarCompuesto(int id_num);

    /**
     * @brief Busca una farmacia en el AVL por su CIF.
     * @return Puntero a la Farmacia o nullptr si no se encuentra.
     */
    Farmacia* buscarFarmacia(const std::string& cif);

    /**
     * @brief Suministra un medicamento a una farmacia.
     * (Localiza el med y llama a farmacia.dispensaMedicam)
     */
    void suministrarFarmacia(Farmacia& f, int id_num);

    /**
     * @brief Busca laboratorios que sirven un Principio Activo.
     * (Es el 'buscarLabs' del UML)
     */
    ListaEnlazada<Laboratorio*> buscarLabs(const std::string& nombrePA);
};


MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma) {
    //  Leer medicamentos
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
                    principiosActivos.insertar(*p, principiosActivos.tamlog());
                } catch (const std::exception&) {}
            }
        }
        is_meds.close();
    }

    //  Leer laboratorios
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
                    laboratorios.insertarFinal(*l);
                } catch (const std::exception&) {}
            }
        }
        is_labs.close();
    }

    //  Enlazar
    auto it_lab = laboratorios.iteradorInicio();
    if (it_lab.haySiguiente()) {
        for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
            if (!it_lab.haySiguiente()) break;
            principiosActivos[i].servidoPor(&it_lab.dato());
            if (i % 2 == 1) {
                it_lab.siguiente();
            }
        }
    }
    // --- NUEVO: Cargar Farmacias ---
    std::ifstream is_farma(archivo_farma);
    if (is_farma.is_open()) {
        std::string fila;
        while (std::getline(is_farma, fila)) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            // Asumo formato: 0:cif, 1:prov, 2:local, 3:nombre, 4:dir, 5:cp
            if (campos.size() == 6) {
                try {
                    // Creamos la farmacia pasándole 'this' como puntero a MediExpress
                    // El AVL la copiará
                    Farmacia f(campos[0], campos[1], campos[2],
                               campos[3], campos[4], campos[5], this);

                    farmacias.insercion(f); // Inserta el objeto en el AVL
                } catch (const std::exception&) {}
            }
        }
        is_farma.close();
    }
}






int MediExpress::contarMedicamentosSinLab() const {
    int contador = 0;
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        if (principiosActivos[i].getLaboratorio() == nullptr) {
            contador++;
        }
    }
    return contador;
}

ListaEnlazada<Laboratorio*> MediExpress::buscarLabCiudad(const std::string& ciudad) const {
    ListaEnlazada<Laboratorio*> resultados;
    auto it = laboratorios.iteradorInicio();
    while (it.haySiguiente()) {
        if (it.dato().getLocalidad().find(ciudad) != std::string::npos) {
            resultados.insertarFinal(&it.dato());
        }
        it.siguiente();
    }
    return resultados;
}

    VDinamico<PaMedicamento*> MediExpress::buscarCompuesto(const std::string& nombre) {
        VDinamico<PaMedicamento*> resultados;
        for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
            if (principiosActivos[i].get_nombre().find(nombre) != std::string::npos) {
                    resultados.insertar(&principiosActivos[i], resultados.tamlog());            }
        }
        return resultados;
    }

ListaEnlazada<Laboratorio*> MediExpress::buscarLabsPorCompuesto(const std::string& compuesto) {
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
        Laboratorio* lab = principiosActivos[i].getLaboratorio();
        if (lab && lab->getId() == idLab) {
            std::cout << "    -> " << principiosActivos[i].get_nombre() << std::endl;
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
        if (principiosActivos[i].getLaboratorio() == nullptr) {
            meds_sin_lab.insertar(&principiosActivos[i], meds_sin_lab.tamlog());
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
}



int MediExpress::eliminarLabsPorCiudad(const std::string& ciudad) {
    VDinamico<Laboratorio*> labs_a_eliminar;

    //Buscar laboratorios a eliminar
    auto it_lab = laboratorios.iteradorInicio();
    while (it_lab.haySiguiente()) {
        if (it_lab.dato().getLocalidad().find(ciudad) != std::string::npos) {
            labs_a_eliminar.insertar(&it_lab.dato(), labs_a_eliminar.tamlog());
        }
        it_lab.siguiente();
    }

    if (labs_a_eliminar.tamlog() == 0) {
        return 0;
    }

    //Desvincular medicamentos
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        Laboratorio* lab_asignado = principiosActivos[i].getLaboratorio();
        if (lab_asignado) {
            for (unsigned int j = 0; j < labs_a_eliminar.tamlog(); ++j) {
                if (lab_asignado == labs_a_eliminar[j]) {
                    principiosActivos[i].servidoPor(nullptr);
                    break;
                }
            }
        }
    }


    // Borramos el laboratorio
    auto it = laboratorios.iteradorInicio();
    while (it.haySiguiente()) {
        bool debe_eliminarse = false;
        // Comprobamos si el laboratorio actual está en la lista de eliminación
        for (unsigned int i = 0; i < labs_a_eliminar.tamlog(); ++i) {
            if (&it.dato() == labs_a_eliminar[i]) {
                debe_eliminarse = true;
                break;
            }
        }

        if (debe_eliminarse) {
            Laboratorio ptr_a_borrar = it.dato();

            auto it_siguiente = it;
            it_siguiente.siguiente();

            laboratorios.borrar(it);

            delete &ptr_a_borrar;

            it = it_siguiente;
        } else {
            it.siguiente();
        }
    }

    return labs_a_eliminar.tamlog();
}

#endif //MEDIEXPRESS_H