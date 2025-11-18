#include "MediExpress.h"
#include <iostream>
#include <fstream>
#include <vector>

MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma) {
    std::cout << "=== Constructor MediExpress: Iniciando Carga ===" << std::endl;

    std::cout << "Paso 1: Cargando medicamentos..." << std::endl;
    std::ifstream is_meds(archivo_meds);
    int med_count = 0;
    if (is_meds.is_open()) {
        std::string fila;
        while (std::getline(is_meds, fila)) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);
            if (!campos.empty() && campos.back().empty()) campos.pop_back();

            if (campos.size() >= 3) {
                try {
                    int id = std::stoi(campos[0]);
                    PaMedicamento pm(id, campos[1], campos[2]);
                    medication[id] = pm;
                    med_count++;
                } catch (const std::exception&) {}
            }
        }
        is_meds.close();
    }
    std::cout << med_count << " medicamentos cargados." << std::endl;

    std::vector<PaMedicamento*> meds_vector;
    meds_vector.reserve(medication.size());
    for (auto &kv : medication) {
        meds_vector.push_back(&kv.second);
    }

    std::cout << "Paso 2: Iniciando carga de laboratorios..." << std::endl;
    std::ifstream is_labs(archivo_labs);
    int lab_count = 0;
    int num_linea = 0;
    if (is_labs.is_open()) {
        std::string fila;
        while (std::getline(is_labs, fila)) {
            num_linea++;

            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();

            std::vector<std::string> campos = parsearFilaCSV(fila);

            if (campos.size() == 5) {
                try {
                    Laboratorio* l = new Laboratorio(std::stoi(campos[0]), campos[1], campos[2], campos[3], campos[4]);
                    laboratorios.push_back(*l);
                    delete l;
                    lab_count++;
                } catch (const std::exception& e) {
                    std::cout << " Linea " << num_linea << " DESCARTADA (Error de conversion, ej. ID no numerico o cabecera)."
                              << " Contenido: " << fila << std::endl;
                }
            } else {
                std::cout << " Linea " << num_linea << " DESCARTADA (Campos esperados: 5, Encontrados: " << campos.size() << ")."
                          << " Contenido: " << fila << std::endl;
            }
        }
        is_labs.close();
    }
    std::cout  << lab_count << " laboratorios cargados."   << std::endl;

    std::cout << "Paso 4: Enlazando medicamentos y laboratorios..." << std::endl;
    auto it_lab = laboratorios.begin();
    if (it_lab != laboratorios.end()) {
        for (std::size_t i = 0; i < meds_vector.size(); ++i) {
            if (it_lab == laboratorios.end()) break;
            meds_vector[i]->servidoPor(&(*it_lab));
            if (i % 2 == 1) {
                ++it_lab;
            }
        }
    }

    this->asignarMedsSinLabAMadrid();
    
    std::cout << "Paso 5: Cargando farmacias..." << std::endl;
    std::ifstream is_farma(archivo_farma);
    int farma_count = 0;
    if (is_farma.is_open()) {
        std::string fila;
        while (std::getline(is_farma, fila)) {
            if (fila.empty()) continue;
            if (fila.back() == '\r') fila.pop_back();
            std::vector<std::string> campos = parsearFilaCSV(fila);

            if (campos.size() == 6) {
                try {
                    farmacias.emplace_back(campos[0], campos[1], campos[2],
                                           campos[3], campos[4], campos[5], this);
                    farma_count++;
                } catch (const std::exception&) {}
            }
        }
        is_farma.close();
    }
    std::cout << farma_count << " farmacias cargadas." << std::endl;

    std::cout << "Paso 6: Asignando stock inicial a farmacias..." << std::endl;
    const int STOCK_INICIAL = 10;

    unsigned int num_medicamentos = static_cast<unsigned int>(meds_vector.size());
    if (num_medicamentos > 0) {

        for (unsigned int i = 0; i < farmacias.size(); ++i) {
            Farmacia* farmacia = &farmacias[i];

            int med_start_index = static_cast<int>(i) * 100;
            for (int j = 0; j < 100; ++j) {
                int med_index_ciclico = (med_start_index + j) % static_cast<int>(num_medicamentos);
                int id_a_suministrar = meds_vector[med_index_ciclico]->get_id_num();

                this->suministrarFarmacia(*farmacia, id_a_suministrar, STOCK_INICIAL);
            }
        }
    } else {
        std::cerr << "Error: No hay medicamentos cargados, omitiendo asignacion ciclica." << std::endl;
    }

    std::cout << "=== Constructor MediExpress: Carga finalizada ===" << std::endl << std::endl;
}



int MediExpress::contarMedicamentosSinLab() const {
    int contador = 0;
    for (const auto &kv : medication) {
        if (kv.second.getLaboratorio() == nullptr) {
            contador++;
        }
    }
    return contador;
}

std::list<Laboratorio*> MediExpress::buscarLabCiudad(const std::string& ciudad)  {
    std::list<Laboratorio*> resultados;
    auto it = laboratorios.begin();
    while (it != laboratorios.end()) {
        if ((*it).getLocalidad().find(ciudad) != std::string::npos) {
            resultados.push_back(&(*it));
        }
        ++it;
    }
    return resultados;
}

std::vector<PaMedicamento*> MediExpress::buscarCompuesto(const std::string& nombre) {
    std::vector<PaMedicamento*> resultados;
    for (auto &kv : medication) {
        if (kv.second.get_nombre().find(nombre) != std::string::npos) {
            resultados.push_back(&kv.second);
        }
    }
    return resultados;
}

std::list<Laboratorio*> MediExpress::buscarLabsPorCompuesto(const std::string& compuesto) {
    std::list<Laboratorio*> labs_encontrados;
    std::vector<PaMedicamento*> meds = buscarCompuesto(compuesto);
    std::vector<int> ids_ya_agregados;

    for (unsigned int i = 0; i < meds.size(); ++i) {
        Laboratorio* lab = meds[i]->getLaboratorio();
        if (lab) {
            bool encontrado = false;
            for (unsigned int j = 0; j < ids_ya_agregados.size(); ++j) {
                if (ids_ya_agregados[j] == lab->getId()) {
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado) {
                labs_encontrados.push_back(lab);
                ids_ya_agregados.push_back(lab->getId());
            }
        }
    }
    return labs_encontrados;
}

void MediExpress::imprimirMedicamentosPorLaboratorio(int idLab) const {
    std::cout << "   Medicamentos suministrados por el laboratorio con ID " << idLab << ":" << std::endl;
    bool encontrado = false;
    for (const auto &kv : medication) {
        const PaMedicamento& med = kv.second;
        Laboratorio* lab = med.getLaboratorio();
        if (lab && lab->getId() == idLab) {
            std::cout << "    -> " << med.get_nombre() << std::endl;
            encontrado = true;
        }
    }
    if (!encontrado) {
        std::cout << "    (Este laboratorio no suministra ningun medicamento de la lista)" << std::endl;
    }
}

void MediExpress::asignarMedsSinLabAMadrid() {
    std::vector<PaMedicamento*> meds_sin_lab;
    for (auto &kv : medication) {
        if (kv.second.getLaboratorio() == nullptr) {
            meds_sin_lab.push_back(&kv.second);
        }
    }

    std::list<Laboratorio*> labs_madrid = buscarLabCiudad("Madrid");

    auto it_lab = labs_madrid.begin();
    int asignaciones = 0;

    for (unsigned int i = 0; i < meds_sin_lab.size(); ++i) {
        if (it_lab == labs_madrid.end()) break;

        meds_sin_lab[i]->servidoPor(*it_lab);
        asignaciones++;
        ++it_lab;
    }
}

int MediExpress::eliminarLabsPorCiudad(const std::string& ciudad) {
    std::vector<Laboratorio*> labs_a_eliminar;

    auto it_lab = laboratorios.begin();
    while (it_lab != laboratorios.end()) {
        if ((*it_lab).getLocalidad().find(ciudad) != std::string::npos) {
            labs_a_eliminar.push_back(&(*it_lab));
        }
        ++it_lab;
    }

    if (labs_a_eliminar.size() == 0) {
        return 0;
    }

    for (auto &kv : medication) {
        Laboratorio* lab_asignado = kv.second.getLaboratorio();
        if (lab_asignado) {
            for (unsigned int j = 0; j < labs_a_eliminar.size(); ++j) {
                if (lab_asignado == labs_a_eliminar[j]) {
                    kv.second.servidoPor(nullptr);
                    break;
                }
            }
        }
    }

    auto it = laboratorios.begin();
    while (it != laboratorios.end()) {
        bool debe_eliminarse = false;
        for (unsigned int i = 0; i < labs_a_eliminar.size(); ++i) {
            if (&(*it) == labs_a_eliminar[i]) {
                debe_eliminarse = true;
                break;
            }
        }

        if (debe_eliminarse) {
            it = laboratorios.erase(it);
        } else {
            ++it;
        }
    }

    return labs_a_eliminar.size();
}


PaMedicamento* MediExpress::buscarCompuesto(int id_num) {
    auto it = medication.find(id_num);
    if (it != medication.end()) {
        return &it->second;
    }
    return nullptr;
}


Farmacia* MediExpress::buscarFarmacia(const std::string& cif) {
    for (std::size_t i = 0; i < farmacias.size(); ++i) {
        if (farmacias[i].getCif() == cif) {
            return &farmacias[i];
        }
    }
    return nullptr;
}




void MediExpress::suministrarFarmacia(Farmacia& f, int id_num, int n) {
    PaMedicamento* med_encontrado = this->buscarCompuesto(id_num);

    if (med_encontrado) {
        f.nuevoStock(med_encontrado, n);
    } else {
        std::cout << "MediExpress: AVISO: No se pudo encontrar el medicamento con ID " << id_num
                  << " para suministrar a [" << f.getNombre() << "]." << std::endl;
    }
}


std::vector<Farmacia*> MediExpress::buscarFarmacias(const std::string& provincia) {
    std::vector<Farmacia*> farmacias_encontradas;
    for (size_t i = 0; i < farmacias.size(); ++i) {
        if (farmacias[i].getProvincia() == provincia) {
            farmacias_encontradas.push_back(&farmacias[i]);
        }
    }
    return farmacias_encontradas;
}


bool MediExpress::eliminarMedicamento(int id_num) {
    auto it_med = medication.find(id_num);

    if (it_med == medication.end()) {
        return false;
    }

    medication.erase(it_med);

    for (size_t i = 0; i < farmacias.size(); ++i) {
        farmacias[i].eliminarStock(id_num);
    }

    return true;
}