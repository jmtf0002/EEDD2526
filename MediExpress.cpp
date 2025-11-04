#include "MediExpress.h"
#include <iostream>




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
                    PaMedicamento* p = new PaMedicamento(std::stoi(campos[0]), campos[1], campos[2]);
                    medication.insertar(*p, medication.tamlog());
                    delete p;
                    med_count++;
                } catch (const std::exception&) {}
            }
        }
        is_meds.close();
    }
    std::cout << med_count << " medicamentos cargados." << std::endl;

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
                    laboratorios.insertarFinal(*l);
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
    auto it_lab = laboratorios.iteradorInicio();
    if (it_lab.haySiguiente()) {
        for (unsigned int i = 0; i < medication.tamlog(); ++i) {
            if (!it_lab.haySiguiente()) break;
            medication[i].servidoPor(&it_lab.dato());
            if (i % 2 == 1) {
                it_lab.siguiente();
            }
        }
    }

    this->asignarMedsSinLabAMadrid();
    std::cout << "Paso 5: Cargando farmacias en AVL..." << std::endl;
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
                    Farmacia f(campos[0], campos[1], campos[2],
                               campos[3], campos[4], campos[5], this);
                    farmacias.insercion(f);
                    farma_count++;
                } catch (const std::exception&) {}
            }
        }
        is_farma.close();
    }
    std::cout << farma_count << " farmacias cargadas." << std::endl;

    std::cout << "Paso 6: Asignando stock inicial a farmacias..." << std::endl;
    VDinamico<std::string> cifs_farmacias;
    std::ifstream is_farma_cifs(archivo_farma);
    std::string fila_cif;

    if (is_farma_cifs.is_open()) {
        while (std::getline(is_farma_cifs, fila_cif)) {
            if (fila_cif.empty()) continue;
            if (fila_cif.back() == '\r') fila_cif.pop_back();

            std::vector<std::string> campos = parsearFilaCSV(fila_cif);
            if (campos.size() >= 1) {
                cifs_farmacias.insertar(campos[0], cifs_farmacias.tamlog());
            }
        }
        is_farma_cifs.close();
    } else {
        std::cerr << "Error: No se pudo abrir " << archivo_farma << std::endl;
    }

    unsigned int num_medicamentos = medication.tamlog();
    if (num_medicamentos > 0) {
        for (unsigned int i = 0; i < cifs_farmacias.tamlog(); ++i) {
            Farmacia* farmacia = this->buscarFarmacia(cifs_farmacias[i]);
            if (farmacia) {
                int med_start_index = i * 100;
                for (int j = 0; j < 100; ++j) {
                    int med_index_ciclico = (med_start_index + j) % num_medicamentos;

                    int id_a_suministrar = medication[med_index_ciclico].get_id_num();

                    this->suministrarFarmacia(*farmacia, id_a_suministrar);
                }
            } else {
                 std::cerr << "Error: No se encontro la farmacia con CIF "
                           << cifs_farmacias[i] << " en el AVL." << std::endl;
            }
        }
    } else {
        std::cerr << "Error: No hay medicamentos cargados, omitiendo asignacion ciclica." << std::endl;
    }

    std::cout << "=== Constructor MediExpress: Carga finalizada ===" << std::endl << std::endl;
}

int MediExpress::contarMedicamentosSinLab() const {
    int contador = 0;
    for (unsigned int i = 0; i < medication.tamlog(); ++i) {
        if (medication[i].getLaboratorio() == nullptr) {
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
        for (unsigned int i = 0; i < medication.tamlog(); ++i) {
            if (medication[i].get_nombre().find(nombre) != std::string::npos) {
                    resultados.insertar(&medication[i], resultados.tamlog());            }
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
    for (unsigned int i = 0; i < medication.tamlog(); ++i) {
        Laboratorio* lab = medication[i].getLaboratorio();
        if (lab && lab->getId() == idLab) {
            std::cout << "    -> " << medication[i].get_nombre() << std::endl;
            encontrado = true;
        }
    }
    if (!encontrado) {
        std::cout << "    (Este laboratorio no suministra ningun medicamento de la lista)" << std::endl;
    }
}

void MediExpress::asignarMedsSinLabAMadrid() {
    VDinamico<PaMedicamento*> meds_sin_lab;
    for (unsigned int i = 0; i < medication.tamlog(); ++i) {
        if (medication[i].getLaboratorio() == nullptr) {
            meds_sin_lab.insertar(&medication[i], meds_sin_lab.tamlog());
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
    for (unsigned int i = 0; i < medication.tamlog(); ++i) {
        Laboratorio* lab_asignado = medication[i].getLaboratorio();
        if (lab_asignado) {
            for (unsigned int j = 0; j < labs_a_eliminar.tamlog(); ++j) {
                if (lab_asignado == labs_a_eliminar[j]) {
                    medication[i].servidoPor(nullptr);
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

            auto it_siguiente = it;
            it_siguiente.siguiente();


            laboratorios.borrar(it);


            it = it_siguiente;
        } else {
            it.siguiente();
        }
    }

    return labs_a_eliminar.tamlog();
}


/**
 * @brief Sobrecarga de buscarCompuesto. Busca un medicamento por su ID numérico.
 * @param id_num ID numérico del medicamento.
 * @return Puntero al PaMedicamento DENTRO del VDinamico, o nullptr si no se encuentra.
 * @warning (No es 'const' porque devuelve un puntero no-const a un miembro interno).
 */
PaMedicamento* MediExpress::buscarCompuesto(int id_num) {
    for (unsigned int i = 0; i < medication.tamlog(); ++i) {
        if (medication[i].get_id_num() == id_num) {
            return &medication[i];
        }
    }
    return nullptr;
}

/**
 * @brief Busca una farmacia en el AVL por su CIF.
 * @param cif El CIF (Código de Identificación Fiscal) de la farmacia.
 * @return Puntero a la Farmacia DENTRO del AVL, o nullptr si no se encuentra.
 * @warning (No es 'const' porque el método 'buscaRec' del AVL devuelve T*).
 */
Farmacia* MediExpress::buscarFarmacia(const std::string& cif) {

    Farmacia farmacia_llave;
    farmacia_llave.setCif(cif);


    return farmacias.buscaRec(farmacia_llave);
}

/**
 * @brief Suministra un medicamento a una farmacia. (Llamado por Farmacia::pedidoMedicam)
 * @param f Referencia a la farmacia que ha hecho el pedido.
 * @param id_num El ID del medicamento solicitado.
 */
void MediExpress::suministrarFarmacia(Farmacia& f, int id_num) {
    
    PaMedicamento* med_encontrado = this->buscarCompuesto(id_num);

    if (med_encontrado) {

        f.dispensaMedicam(med_encontrado);
    } else {

        std::cout << "MediExpress: AVISO: No se pudo encontrar el medicamento con ID " << id_num << " para suministrar a [" << f.getNombre() << "]." << std::endl;
    }
}


/**
 * @brief Devuelve la altura del árbol AVL de farmacias.
 */
unsigned int MediExpress::getAlturaAVLFarmacias() {
    return farmacias.altura();
}

/**
 * @brief Devuelve un VDinamico con el recorrido inorden del AVL de farmacias.
 */
VDinamico<Farmacia*> MediExpress::getInordenAVLFarmacias() {
    return farmacias.recorreInorden();
}
