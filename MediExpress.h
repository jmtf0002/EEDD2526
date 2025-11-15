#ifndef MEDIEXPRESS_H
#define MEDIEXPRESS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include "AVL.h"
#include "VDinamico.h"
#include "ListaEnlazada.h"
#include "PaMedicamento.h"
#include "Laboratorio.h"
#include "Farmacia.h"
#include <map>

std::vector<std::string> parsearFilaCSV(const std::string& linea);

class MediExpress {
private:
    std::map<int, PaMedicamento> medication;
    std::list<Laboratorio> laboratorios;
    std::vector<Farmacia> farmacias;
public:
    MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma);

    VDinamico<PaMedicamento*> buscarCompuesto(const std::string& nombre);

    ListaEnlazada<Laboratorio*> buscarLabCiudad(const std::string& ciudad) ;

    ListaEnlazada<Laboratorio*> buscarLabsPorCompuesto(const std::string& compuesto);

    void imprimirMedicamentosPorLaboratorio(int idLab) const;

    void asignarMedsSinLabAMadrid();

    unsigned int totalMedicamentos() const { return medication.size(); }

    int totalLaboratorios() const { return laboratorios.size(); }

    int contarMedicamentosSinLab() const;

    int eliminarLabsPorCiudad(const std::string& ciudad);



    PaMedicamento* buscarCompuesto(int id_num);

    Farmacia* buscarFarmacia(const std::string& cif);

    void suministrarFarmacia(Farmacia& f, int id_num);

};

#endif //MEDIEXPRESS_H