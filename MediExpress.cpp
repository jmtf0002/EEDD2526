#include "MediExpress.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>


MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma) {
    std::cout << "=== Constructor MediExpress: Carga con Multimap Farmacias ===" << std::endl;

    std::ifstream conteo(archivo_meds);
    int num_lineas = 0; std::string bas;
    while(std::getline(conteo, bas)) num_lineas++;
    conteo.close();

    idMedication = new ThashMedicam(num_lineas, 0.7);
    std::vector<int> vMedi;

    std::ifstream is_meds(archivo_meds);
    if (is_meds.is_open()) {
        std::string fila;
        while (std::getline(is_meds, fila)) {
            if (fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);
            if (c.size() >= 3) {
                try {
                    int id = std::stoi(c[0]);
                    PaMedicamento pm(id, c[1], c[2]);
                    if (idMedication->insertar(id, pm)) vMedi.push_back(id);
                } catch (...) {}
            }
        }
        is_meds.close();
    }
    std::cout << vMedi.size() << " medicamentos cargados." << std::endl;

    for (int id : vMedi) {
        PaMedicamento* pMed = idMedication->buscar(id);
        if (pMed) {
            std::stringstream ss(pMed->get_nombre());
            std::string palabra;
            while (ss >> palabra) if (palabra.length()>0) nombMedication.insert(std::make_pair(palabra, pMed));
        }
    }

    std::ifstream is_labs(archivo_labs);
    if (is_labs.is_open()) {
        std::string fila;
        while (std::getline(is_labs, fila)) {
            if (fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);
            if (c.size() == 5) {
                try { laboratorios.emplace_back(std::stoi(c[0]), c[1], c[2], c[3], c[4]); } catch(...) {}
            }
        }
        is_labs.close();
    }

    auto it_lab = laboratorios.begin();
    if (it_lab != laboratorios.end()) {
        for (size_t i = 0; i < vMedi.size(); ++i) {
            if (it_lab == laboratorios.end()) break;
            PaMedicamento* pMed = idMedication->buscar(vMedi[i]);
            if (pMed) pMed->servidoPor(&(*it_lab));
            if (i % 2 == 1) ++it_lab;
        }
    }
    this->asignarMedsSinLabAMadrid();

    std::cout << "Paso 5: Cargando farmacias en Multimap..." << std::endl;
    std::ifstream is_farma(archivo_farma);
    if (is_farma.is_open()) {
        std::string fila;
        while (std::getline(is_farma, fila)) {
            if (fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);
            if (c.size() == 6) {
                Farmacia f(c[0], c[1], c[2], c[3], c[4], c[5], this);
                farmacias.insert(std::make_pair(c[1], f));
            }
        }
        is_farma.close();
    }

    if (!vMedi.empty()) {
        int farma_index = 0;
        for (auto& par : farmacias) {
            Farmacia& f = par.second;
            int start = farma_index * 100;
            for (int j = 0; j < 100; ++j) {
                int idx = (start + j) % (int)vMedi.size();
                suministrarFarmacia(f, vMedi[idx], 10);
            }
            farma_index++;
        }
    }
    std::cout << "=== Carga Finalizada ===" << std::endl << std::endl;
}

MediExpress::~MediExpress() { delete idMedication; }

int MediExpress::contarMedicamentosSinLab() const {
    int contador = 0;
    std::vector<PaMedicamento*> todos = idMedication->getEntradasValidas();
    for (const auto* med : todos) if (med->getLaboratorio() == nullptr) contador++;
    return contador;
}
PaMedicamento* MediExpress::buscarCompuesto(int id_num) { return idMedication->buscar(id_num); }

std::vector<PaMedicamento*> MediExpress::buscarCompuesto(const std::string& nombre) {
    std::stringstream ss(nombre);
    std::string palabra;
    std::vector<std::string> palabras_busqueda;
    while (ss >> palabra) if (!palabra.empty()) palabras_busqueda.push_back(palabra);
    if (palabras_busqueda.empty()) return {};

    std::set<PaMedicamento*> interseccion;
    auto range = nombMedication.equal_range(palabras_busqueda[0]);
    for (auto it = range.first; it != range.second; ++it) interseccion.insert(it->second);

    for (size_t i = 1; i < palabras_busqueda.size(); ++i) {
        std::set<PaMedicamento*> grupo_actual;
        auto range_w = nombMedication.equal_range(palabras_busqueda[i]);
        for (auto it = range_w.first; it != range_w.second; ++it) grupo_actual.insert(it->second);
        std::set<PaMedicamento*> temp;
        std::set_intersection(interseccion.begin(), interseccion.end(),
                              grupo_actual.begin(), grupo_actual.end(),
                              std::inserter(temp, temp.begin()));
        interseccion = temp;
        if (interseccion.empty()) break;
    }
    std::vector<PaMedicamento*> resultado(interseccion.begin(), interseccion.end());
    std::sort(resultado.begin(), resultado.end(), [](PaMedicamento* a, PaMedicamento* b) {
        return a->get_nombre() < b->get_nombre();
    });
    return resultado;
}

std::list<Laboratorio*> MediExpress::buscarLabsPorCompuesto(const std::string& compuesto) {
    std::vector<PaMedicamento*> meds = buscarCompuesto(compuesto);
    std::list<Laboratorio*> labs_encontrados;
    std::set<int> ids_labs;
    for (auto* med : meds) {
        Laboratorio* lab = med->getLaboratorio();
        if (lab && ids_labs.find(lab->getId()) == ids_labs.end()) {
            labs_encontrados.push_back(lab);
            ids_labs.insert(lab->getId());
        }
    }
    return labs_encontrados;
}

void MediExpress::imprimirMedicamentosPorLaboratorio(int idLab) const {
    bool encontrado = false;
    std::vector<PaMedicamento*> todos = idMedication->getEntradasValidas();
    for (const auto* med : todos) {
        Laboratorio* lab = med->getLaboratorio();
        if (lab && lab->getId() == idLab) {
            std::cout << " -> " << med->get_nombre() << std::endl;
            encontrado = true;
        }
    }
    if (!encontrado) std::cout << " (Ninguno)" << std::endl;
}

void MediExpress::asignarMedsSinLabAMadrid() {
    std::list<Laboratorio*> labs_madrid = buscarLabCiudad("Madrid");
    if (labs_madrid.empty()) return;
    std::vector<PaMedicamento*> todos = idMedication->getEntradasValidas();
    auto it_lab = labs_madrid.begin();
    for (auto* med : todos) {
        if (med->getLaboratorio() == nullptr) {
            med->servidoPor(*it_lab);
            ++it_lab;
            if (it_lab == labs_madrid.end()) it_lab = labs_madrid.begin();
        }
    }
}

int MediExpress::eliminarLabsPorCiudad(const std::string& ciudad) {
    std::vector<Laboratorio*> labs_a_eliminar;
    for (auto& lab : laboratorios) {
        if (lab.getLocalidad().find(ciudad) != std::string::npos) labs_a_eliminar.push_back(&lab);
    }
    if (labs_a_eliminar.empty()) return 0;
    std::vector<PaMedicamento*> todos = idMedication->getEntradasValidas();
    for (auto* med : todos) {
        Laboratorio* lab_asig = med->getLaboratorio();
        if (lab_asig) {
            for (auto* target : labs_a_eliminar) if (lab_asig == target) med->servidoPor(nullptr);
        }
    }
    auto it = laboratorios.begin();
    while (it != laboratorios.end()) {
        bool borrar = false;
        for (auto* target : labs_a_eliminar) if (&(*it) == target) borrar = true;
        if (borrar) it = laboratorios.erase(it); else ++it;
    }
    return labs_a_eliminar.size();
}

std::list<Laboratorio*> MediExpress::buscarLabCiudad(const std::string& ciudad) {
    std::list<Laboratorio*> resultados;
    for (auto& lab : laboratorios) {
        if (lab.getLocalidad().find(ciudad) != std::string::npos) resultados.push_back(&lab);
    }
    return resultados;
}

Farmacia* MediExpress::buscarFarmacia(const std::string& cif) {
    for (auto& par : farmacias) if (par.second.getCif() == cif) return &par.second;
    return nullptr;
}

void MediExpress::suministrarFarmacia(Farmacia& f, int id_num, int n) {
    PaMedicamento* med = this->buscarCompuesto(id_num);
    if (med) f.nuevoStock(med, n);
}

std::vector<Farmacia*> MediExpress::buscarFarmacias(const std::string& provincia) {
    std::vector<Farmacia*> encontrados;
    auto range = farmacias.equal_range(provincia);
    for (auto it = range.first; it != range.second; ++it) encontrados.push_back(&it->second);
    return encontrados;
}

bool MediExpress::eliminarMedicamento(int id_num) {
    PaMedicamento* pMed = idMedication->buscar(id_num);
    if (!idMedication->borrar(id_num)) return false;
    if (pMed != nullptr) {
        auto it = nombMedication.begin();
        while (it != nombMedication.end()) {
            if (it->second == pMed) it = nombMedication.erase(it);
            else ++it;
        }
    }
    for (auto& par : farmacias) par.second.eliminarStock(id_num);
    return true;
}

Farmacia* MediExpress::buscarFarmaciaPorCiudad(const std::string& ciudad) {
    for (auto& par : farmacias) {

        if (par.second.getLocalidad().find(ciudad) != std::string::npos) {
            return &par.second;
        }
    }
    return nullptr;
}