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
    // En MediExpress.h (dentro de la sección public:)

    /**
     * @brief Devuelve la altura del árbol AVL de farmacias.
     */
    unsigned int getAlturaAVLFarmacias() {
        return farmacias.altura(); // Asume que tu AVL tiene el método publico altura()
    }

    /**
     * @brief Devuelve un VDinamico con el recorrido inorden del AVL de farmacias.
     */
    VDinamico<Farmacia*> getInordenAVLFarmacias() {
        // Asume que tu AVL tiene recorreInorden() que devuelve un VDinamico<T>
        // Si devuelve VDinamico<T*>, tendrás que adaptar este código.
        return farmacias.recorreInorden();
    }
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



#endif //MEDIEXPRESS_H