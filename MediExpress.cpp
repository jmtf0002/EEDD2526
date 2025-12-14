#include "MediExpress.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <limits> // Para numeric_limits

std::vector<std::string> parsearFilaCSV(const std::string& linea) {
    std::vector<std::string> campos;
    std::string campo_actual;
    bool en_campo_con_comillas = false;
    char delimitador = ';';
    for (size_t i = 0; i < linea.length(); ++i) {
        char c = linea[i];
        if (en_campo_con_comillas) {
            if (c == '"') {
                if (i + 1 < linea.length() && linea[i + 1] == '"') { campo_actual += '"'; i++; }
                else { en_campo_con_comillas = false; }
            } else { campo_actual += c; }
        } else {
            if (c == delimitador) { campos.push_back(campo_actual); campo_actual.clear(); }
            else if (c == '"') { if (campo_actual.empty()) en_campo_con_comillas = true; else campo_actual += c; }
            else { campo_actual += c; }
        }
    }
    campos.push_back(campo_actual);
    for (std::string& campo : campos) if (!campo.empty() && campo.back() == '\r') campo.pop_back();
    return campos;
}


// Constructor actualizado
MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs,
                         const std::string& archivo_farma, const std::string& archivo_users) {

    std::cout << "=== Inicializando MediExpress ===" << std::endl;
    this->grid = nullptr;

    // ---------------------------------------------------------
    // 1. CARGA DE MEDICAMENTOS (Código Previo)
    // ---------------------------------------------------------
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

    // Indizar por nombre
    for (int id : vMedi) {
        PaMedicamento* pMed = idMedication->buscar(id);
        if (pMed) {
            std::stringstream ss(pMed->get_nombre());
            std::string palabra;
            while (ss >> palabra) if (palabra.length()>0) nombMedication.insert(std::make_pair(palabra, pMed));
        }
    }

    // ---------------------------------------------------------
    // 2. CARGA DE LABORATORIOS (Código Previo)
    // ---------------------------------------------------------
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
    // Enlazar laboratorios con medicamentos
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

    // ---------------------------------------------------------
    // 3. CARGA DE FARMACIAS CON COORDENADAS
    // ---------------------------------------------------------
    std::cout << "Cargando farmacias y calculando limites UTM..." << std::endl;

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    // Asegúrate de que ARCHIVO_FARMA apunte a "farmacias-coord.csv" en main.cpp
    std::ifstream is_farma(archivo_farma);
    if (is_farma.is_open()) {
        std::string fila;
        while (std::getline(is_farma, fila)) {
            if (fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);

            // --- CORRECCIÓN AQUÍ: COMPROBAR 8 COLUMNAS Y PASAR 9 ARGUMENTOS ---
            if (c.size() >= 8) {
                // Constructor: CIF, Prov, Loc, Nom, Dir, CP, Lat, Lon, MediExpress*
                Farmacia f(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7], this);

                double lat = f.getY();
                double lon = f.getX();
                if (lon < minX) minX = lon;
                if (lon > maxX) maxX = lon;
                if (lat < minY) minY = lat;
                if (lat > maxY) maxY = lat;

                farmacias.insert(std::make_pair(c[1], f)); // Clave: Provincia
            }
        }
        is_farma.close();
    }

    // ---------------------------------------------------------
    // 4. CREACIÓN Y OPTIMIZACIÓN DE LA MALLA REGULAR
    // ---------------------------------------------------------
    std::cout << "Coordenadas de la malla: Min[" << minX << ", " << minY << "], Max[" << maxX << ", " << maxY << "]" << std::endl;

    int nDiv = 10; // Empezamos con un número bajo
    bool mallaOptima = false;

    // Bucle para encontrar el tamaño óptimo (máx elementos por celda entre 10 y 15)
    // El PDF dice "probar con diferentes números... debiendo estar entre 10-15"
    while (!mallaOptima) {
        MallaRegular<Farmacia*> tempGrid(minX, minY, maxX, maxY, nDiv);

        // Insertar todas las farmacias en la malla temporal
        for (auto& par : farmacias) {
            tempGrid.insertar(par.second.getX(), par.second.getY(), &par.second);
        }

        unsigned maxElem = tempGrid.maxElementosPorCelda();

        // Criterio de parada del PDF
        if (maxElem <= 15 && maxElem >= 10) {
            // Hemos encontrado la configuración adecuada
            std::cout << "Probando malla con " << nDiv << " divisiones..." << std::endl;
            std::cout << "Celda mas poblada con: " << maxElem << std::endl;
            std::cout << "Media ocupacion: " << tempGrid.promedioElementosPorCelda() << std::endl;

            // Creamos la malla definitiva en el heap
            this->grid = new MallaRegular<Farmacia*>(minX, minY, maxX, maxY, nDiv);
            for (auto& par : farmacias) {
                this->grid->insertar(par.second.getX(), par.second.getY(), &par.second);
            }
            mallaOptima = true;
        } else {
            // Ajuste simple: Si hay muchas farmacias por celda, aumentamos divisiones
            // Si hay pocas, las disminuimos (aunque empezamos bajo, así que subimos)
            if (maxElem > 15) nDiv += 10;
            else if (maxElem < 10) nDiv -= 5; // Caso raro si empezamos bajo

            if (nDiv <= 1) nDiv = 2; // Seguridad
            // Seguridad anti-bucle infinito:
            if (nDiv > 2000) {
                // Fallback si no convergen los datos
                this->grid = new MallaRegular<Farmacia*>(minX, minY, maxX, maxY, nDiv);
                 for (auto& par : farmacias) {
                    this->grid->insertar(par.second.getX(), par.second.getY(), &par.second);
                }
                mallaOptima = true;
            }
        }
    }

 // ---------------------------------------------------------
    // 5. CARGA DE USUARIOS (ADAPTADO A TU CSV)
    // ---------------------------------------------------------
    std::cout << "Cargando usuarios..." << std::endl;
    std::ifstream is_users(archivo_users);
    if (is_users.is_open()) {
        std::string fila;
        int countUsers = 0;
        while (std::getline(is_users, fila)) {
            if (fila.empty()) continue; if(fila.back()=='\r') fila.pop_back();
            std::vector<std::string> c = parsearFilaCSV(fila);

            try {
                // CASO A: Tu formato actual (4 columnas: ID;Ciudad;Lat;Lon)
                if (c.size() >= 4) {
                    // 1. Reemplazar comas por puntos en las coordenadas
                    std::replace(c[2].begin(), c[2].end(), ',', '.');
                    std::replace(c[3].begin(), c[3].end(), ',', '.');

                    // 2. Extraer datos
                    int id = std::stoi(c[0]);
                    std::string ciudadProv = c[1]; // Usamos la ciudad como provincia
                    double lat = std::stod(c[2]);  // Latitud
                    double lon = std::stod(c[3]);  // Longitud

                    // 3. Generar datos ficticios para Nombre y Dirección (no vienen en el CSV)
                    std::string nombre = "Usuario " + ciudadProv + " (" + std::to_string(id) + ")";
                    std::string dir = "Direccion desconocida";

                    // 4. Crear usuario
                    // Constructor: ID, Nombre, Dir, Provincia, Lat, Lon
                    Usuario* u = new Usuario(id, nombre, dir, ciudadProv, lat, lon);
                    users[id] = u;
                    countUsers++;
                }
                // CASO B: Formato original del PDF (6 columnas)
                else if (c.size() >= 6) {
                    // Intento de compatibilidad por si cambias de archivo
                    int id = std::stoi(c[0]);
                    // Asumimos que aquí ya vienen con puntos
                    double lat = std::stod(c[4]);
                    double lon = std::stod(c[5]);
                    Usuario* u = new Usuario(id, c[1], c[2], c[3], lat, lon);
                    users[id] = u;
                    countUsers++;
                }
            } catch (...) {
                // Si falla una línea (ej: cabecera o formato mal), la saltamos sin cerrar el programa
            }
        }
        std::cout << "Usuarios cargados correctamente: " << countUsers << std::endl;
        is_users.close();
    } else {
        std::cerr << "ERROR: No se pudo abrir el archivo de usuarios: " << archivo_users << std::endl;
    }

    // ---------------------------------------------------------
    // 6. SUMINISTRO INICIAL (Stock) (Código Previo)
    // ---------------------------------------------------------
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

MediExpress::~MediExpress() {
    delete idMedication;
    delete grid; // Limpiar la malla
    for(auto& p : users) delete p.second; // Limpiar usuarios
}

// ---------------------------------------------------------
// RESTO DE MÉTODOS (Sin cambios mayores, solo mantenerlos)
// ---------------------------------------------------------

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
    // Primera palabra
    auto range = nombMedication.equal_range(palabras_busqueda[0]);
    for (auto it = range.first; it != range.second; ++it) interseccion.insert(it->second);

    // Intersección con el resto
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
    // Buscar por clave en el multimap (suponiendo que la clave es Provincia o Ciudad)
    // El PDF dice: multimap<string, Farmacia> con clave la PROVINCIA.
    // OJO: En la carga anterior usé c[2] (Ciudad/Provincia) como clave.
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