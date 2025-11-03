#include "MediExpress.h"
#include <iostream>




MediExpress::MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma) {
    std::cout << "--- Constructor MediExpress: Iniciando Carga ---" << std::endl;

    // --- PASO 1: Leer medicamentos ---
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
                    principiosActivos.insertar(*p, principiosActivos.tamlog());
                    delete p; // Libera el puntero temporal
                    med_count++;
                } catch (const std::exception&) {}
            }
        }
        is_meds.close();
    }
    std::cout << "-> " << med_count << " medicamentos cargados." << std::endl;

    // --- PASO 2: Leer laboratorios ---
    std::cout << "DEBUG (Paso 2): Iniciando carga de laboratorios (" << archivo_labs << ")..." << std::endl;
    std::ifstream is_labs(archivo_labs);
    int lab_count = 0;
    int num_linea = 0; // <-- AÑADIDO: Contador de líneas
    if (is_labs.is_open()) {
        std::string fila;
        while (std::getline(is_labs, fila)) {
            num_linea++; // <-- AÑADIDO: Incrementa en CADA línea

            if (fila.empty()) continue; // Salta líneas vacías
            if (fila.back() == '\r') fila.pop_back();

            std::vector<std::string> campos = parsearFilaCSV(fila);

            if (campos.size() == 5) {
                try {
                    Laboratorio* l = new Laboratorio(std::stoi(campos[0]), campos[1], campos[2], campos[3], campos[4]);
                    laboratorios.insertarFinal(*l);
                    delete l; // Libera el puntero temporal
                    lab_count++;
                } catch (const std::exception& e) {
                    // --- AÑADIDO: Reporta fallos de 'stoi' ---
                    std::cout << "DEBUG (Paso 2): Linea " << num_linea << " DESCARTADA (Error de conversion, ej. ID no numerico o cabecera)."
                              << " Contenido: " << fila << std::endl;
                }
            } else {
                // --- AÑADIDO: Reporta fallos de conteo de campos ---
                std::cout << "DEBUG (Paso 2): Linea " << num_linea << " DESCARTADA (Campos esperados: 5, Encontrados: " << campos.size() << ")."
                          << " Contenido: " << fila << std::endl;
            }
        }
        is_labs.close();
    }
    std::cout << "DEBUG (Paso 2): Carga de laboratorios FINALIZADA. Total: " << lab_count << std::endl;

    // --- PASO 4: Enlazar medicamentos con laboratorios ---
    std::cout << "Paso 4: Enlazando medicamentos y laboratorios..." << std::endl;
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

    // --- PASO 3: Cargar Farmacias en el AVL ---
    std::cout << "Paso 3: Cargando farmacias en AVL..." << std::endl;
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
                    farmacias.insercion(f); // O 'inserta', según tu AVL
                    farma_count++;
                } catch (const std::exception&) {}
            }
        }
        is_farma.close();
    }
    std::cout << "-> " << farma_count << " farmacias cargadas." << std::endl;

    // --- PASO 5: Cargar CIFs y asignar 100 medicamentos cíclicamente ---
    std::cout << "Paso 5: Asignando stock inicial ciclico a farmacias..." << std::endl;
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
        std::cerr << "ERROR (Paso 5): No se pudo re-abrir " << archivo_farma << std::endl;
    }

    // Iteramos y asignamos los medicamentos
    unsigned int num_medicamentos = principiosActivos.tamlog();
    if (num_medicamentos > 0) {
        for (unsigned int i = 0; i < cifs_farmacias.tamlog(); ++i) {
            Farmacia* farmacia = this->buscarFarmacia(cifs_farmacias[i]);
            if (farmacia) {
                int med_start_index = i * 100;
                for (int j = 0; j < 100; ++j) {
                    int med_index_ciclico = (med_start_index + j) % num_medicamentos;
                    // Asumimos que PaMedicamento tiene get_id_num()
                    int id_a_suministrar = principiosActivos[med_index_ciclico].get_id_num();

                    // Llama a suministrarFarmacia.
                    // Para silenciar el log "Suministrando..." y "Recibido...",
                    // comenta los 'cout' DENTRO de MediExpress::suministrarFarmacia
                    // y Farmacia::dispensaMedicam
                    this->suministrarFarmacia(*farmacia, id_a_suministrar);
                }
            } else {
                 std::cerr << "ERROR (Paso 5): No se encontro la farmacia con CIF "
                           << cifs_farmacias[i] << " en el AVL." << std::endl;
            }
        }
    } else {
        std::cerr << "ERROR (Paso 5): No hay medicamentos cargados, omitiendo asignacion ciclica." << std::endl;
    }

    std::cout << "--- Constructor MediExpress: Carga FINALIZADA ---" << std::endl << std::endl;
}


// --- IMPLEMENTACIÓN DEL DESTRUCTOR ---
// Asegúrate de que esto esté en tu .cpp
MediExpress::~MediExpress() {
    // No se necesita CÓDIGO aquí dentro.
    // Los destructores de los miembros (AVL<Farmacia>, VDinamico<PaMedicamento>,
    // y ListaEnlazada<Laboratorio>) se llaman automáticamente
    // porque son objetos, no punteros.

    // El simple hecho de que esta implementación exista soluciona el error.
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
            // Laboratorio ptr_a_borrar = it.dato(); // <-- ELIMINA ESTO (innecesario)

            auto it_siguiente = it;
            it_siguiente.siguiente();

            // Esta es la única llamada que necesitas.
            // La lista se encarga de borrar el nodo Y de destruir el objeto.
            laboratorios.borrar(it);

            // delete &ptr_a_borrar; // <-- ¡¡¡ELIMINA ESTO!!! (LA CAUSA DEL ERROR)

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
    for (unsigned int i = 0; i < principiosActivos.tamlog(); ++i) {
        // Asumimos que PaMedicamento tiene un método get_id_num()
        if (principiosActivos[i].get_id_num() == id_num) {
            // Devuelve la dirección del objeto que está DENTRO del vector
            return &principiosActivos[i]; 
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
    // Para buscar en el AVL<Farmacia>, necesitamos crear un objeto Farmacia
    // "dummy" (ficticio) que solo contenga el CIF, ya que el AVL 
    // usará su operador< para comparar.
    Farmacia farmacia_llave;
    farmacia_llave.setCif(cif);

    // farmacias.buscaRec() buscará y devolverá un puntero al objeto
    // Farmacia que coincide, o nullptr.
    return farmacias.buscaRec(farmacia_llave);
}

/**
 * @brief Suministra un medicamento a una farmacia. (Llamado por Farmacia::pedidoMedicam)
 * @param f Referencia a la farmacia que ha hecho el pedido.
 * @param id_num El ID del medicamento solicitado.
 */
void MediExpress::suministrarFarmacia(Farmacia& f, int id_num) {
    
    // 1. Localizar el medicamento en el almacén central de MediExpress
    PaMedicamento* med_encontrado = this->buscarCompuesto(id_num);

    if (med_encontrado) {
        // 2. Si se encuentra, se lo entregamos a la farmacia.
        // (La farmacia guardará este puntero en su 'stock' local).
     //   std::cout << "MediExpress: Suministrando [" << med_encontrado->get_nombre()
     //             << "] a la farmacia [" << f.getNombre() << "]." << std::endl;
                  
        f.dispensaMedicam(med_encontrado);
    } else {
        // 3. Si no se encuentra el medicamento en la central
     //   std::cout << "MediExpress: AVISO: No se pudo encontrar el medicamento con ID "
        //          << id_num << " para suministrar a [" << f.getNombre() << "]." << std::endl;
    }
}

/**
 * @brief Busca laboratorios que suministran un Principio Activo (búsqueda por nombre).
 * (Este es el método 'buscarLabs' del UML).
 * @param nombrePA Nombre (parcial o completo) del principio activo.
 * @return Una lista enlazada de punteros a los Laboratorios.
 */
ListaEnlazada<Laboratorio*> MediExpress::buscarLabs(const std::string& nombrePA) {
    // Esta función es funcionalmente idéntica a tu 'buscarLabsPorCompuesto'.
    // Simplemente llamamos a la otra función.
    // Si quieres, puedes mover la lógica de 'buscarLabsPorCompuesto' aquí
    // y hacer que 'buscarLabsPorCompuesto' llame a esta.
    return this->buscarLabsPorCompuesto(nombrePA);
}

