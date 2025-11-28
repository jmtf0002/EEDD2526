#include "MediExpress.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

// (Si ya tienes parsearFilaCSV en main, puedes borrar esta implementación.
// Si la borraste de main para ponerla aquí, déjala).
// Para asegurar que compila, la incluyo aquí como static o inline,
// o asumo que sigues la estructura anterior.
// *NOTA*: Si te da error de redefinición, borra este bloque y déjalo solo en main o aquí.
// Lo pongo comentado para que uses el que ya tenías en main o descomentes este.
/*
std::vector<std::string> parsearFilaCSV(const std::string& linea) {
    // ... tu implementación ...
    // (Ver respuesta anterior si la necesitas)
}
*/
// Asumimos que parsearFilaCSV está accesible (enlazado desde main o definido aquí).


MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma) {
    std::cout << "=== Constructor MediExpress: Carga con Multimap Farmacias ===" << std::endl;

    // 1. CARGA DE HASH
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

    // 2. INDEXAR NOMBRES
    for (int id : vMedi) {
        PaMedicamento* pMed = idMedication->buscar(id);
        if (pMed) {
            std::stringstream ss(pMed->get_nombre());
            std::string palabra;
            while (ss >> palabra) if (palabra.length()>0) nombMedication.insert(std::make_pair(palabra, pMed));
        }
    }

    // 3. LABORATORIOS
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

    // 4. ENLACE MED-LAB
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

    // -----------------------------------------------------------
    // 5. CARGA DE FARMACIAS (AHORA EN MULTIMAP)
    // -----------------------------------------------------------
    std::cout << "Paso 5: Cargando farmacias en Multimap..." << std::endl;
    std::ifstream is_farma(archivo_farma);
    if (is_farma.is_open()) {
        std::string fila;
        while (std::getline(is_farma, fila)) {
            if (fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);
            if (c.size() == 6) {
                // Campos: 0:CIF, 1:PROVINCIA, 2:LOCALIDAD, 3:NOMBRE, 4:DIRECCION, 5:CP
                Farmacia f(c[0], c[1], c[2], c[3], c[4], c[5], this);

                // INSERTAMOS EN MULTIMAP: Clave = Provincia (c[1])
                farmacias.insert(std::make_pair(c[1], f));
            }
        }
        is_farma.close();
    }

    // -----------------------------------------------------------
    // 6. STOCK INICIAL
    // -----------------------------------------------------------
    // "Enlazar de forma consecutiva cada 100 medicamentos" [PDF Pág 4]
    // Al ser un multimap, iteramos por el orden de las claves (alfabético provincias).
    if (!vMedi.empty()) {
        int farma_index = 0;
        for (auto& par : farmacias) { // Iteramos el mapa: pair<string, Farmacia>
            Farmacia& f = par.second; // Accedemos al objeto Farmacia

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

MediExpress::~MediExpress() {
    delete idMedication;
}

// ... (Resto de métodos buscarCompuesto, etc. IGUALES QUE ANTES) ...
// Para abreviar, copio solo los que cambian por culpa del multimap farmacias.
// Asegúrate de copiar tus implementaciones de buscarCompuesto, etc. aquí.

int MediExpress::contarMedicamentosSinLab() const {
    int contador = 0;
    std::vector<PaMedicamento*> todos = idMedication->getEntradasValidas();
    for (const auto* med : todos) if (med->getLaboratorio() == nullptr) contador++;
    return contador;
}
PaMedicamento* MediExpress::buscarCompuesto(int id_num) { return idMedication->buscar(id_num); }
// (Incluye aquí la impl. de buscarCompuesto(string), imprimir, asignarMedsSinLab, etc.)
// ...
// ...

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
    return std::vector<PaMedicamento*>(interseccion.begin(), interseccion.end());
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

// -----------------------------------------------------------
// MÉTODOS AFECTADOS POR EL CAMBIO A MULTIMAP
// -----------------------------------------------------------

Farmacia* MediExpress::buscarFarmacia(const std::string& cif) {
    // Como el mapa está ordenado por PROVINCIA, buscar por CIF requiere iterar todo.
    // Iteramos el multimap. 'par' es pair<const string, Farmacia>
    for (auto& par : farmacias) {
        if (par.second.getCif() == cif) {
            return &par.second;
        }
    }
    return nullptr;
}

void MediExpress::suministrarFarmacia(Farmacia& f, int id_num, int n) {
    PaMedicamento* med = this->buscarCompuesto(id_num);
    if (med) f.nuevoStock(med, n);
}

std::vector<Farmacia*> MediExpress::buscarFarmacias(const std::string& provincia) {
    std::vector<Farmacia*> encontrados;

    // AQUÍ ES DONDE EL MULTIMAP BRILLA:
    // Usamos equal_range para obtener solo las farmacias de esa provincia.
    auto range = farmacias.equal_range(provincia);

    // Iteramos solo en el rango devuelto
    for (auto it = range.first; it != range.second; ++it) {
        // it->first es la provincia, it->second es el objeto Farmacia
        encontrados.push_back(&it->second);
    }

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

    // 3. Borrar stock de farmacias
    // Tenemos que recorrer todo el multimap
    for (auto& par : farmacias) {
        par.second.eliminarStock(id_num);
    }
    return true;
}