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
    VDinamico<PaMedicamento> medication;
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
    unsigned int totalMedicamentos() const { return medication.tamlog(); }
    int totalLaboratorios() const { return laboratorios.tam(); }
    int contarMedicamentosSinLab() const;
    int eliminarLabsPorCiudad(const std::string& ciudad);

    // --- MÉTODOS NUEVOS (Solo Declaraciones) ---

    unsigned int getAlturaAVLFarmacias(); // Mover implementación al .cpp

    VDinamico<Farmacia*> getInordenAVLFarmacias(); // Mover implementación al .cpp

    PaMedicamento* buscarCompuesto(int id_num);

    Farmacia* buscarFarmacia(const std::string& cif);

    void suministrarFarmacia(Farmacia& f, int id_num);

    ListaEnlazada<Laboratorio*> buscarLabs(const std::string& nombrePA);
};

#endif //MEDIEXPRESS_H