#ifndef MEDIEXPRESS_H
#define MEDIEXPRESS_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include "PaMedicamento.h"
#include "Laboratorio.h"
#include "Farmacia.h"
#include "ThashMedicam.h"

class Farmacia;

std::vector<std::string> parsearFilaCSV(const std::string& linea);

class MediExpress {
private:
    ThashMedicam* idMedication;
    std::multimap<std::string, PaMedicamento*> nombMedication;

    std::list<Laboratorio> laboratorios;


    std::multimap<std::string, Farmacia> farmacias;

public:
    MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma);
    ~MediExpress();

    std::vector<PaMedicamento*> buscarCompuesto(const std::string& nombre);
    PaMedicamento* buscarCompuesto(int id_num);

    std::list<Laboratorio*> buscarLabCiudad(const std::string& ciudad);
    std::list<Laboratorio*> buscarLabsPorCompuesto(const std::string& compuesto);
    Farmacia* buscarFarmacia(const std::string& cif);

    void imprimirMedicamentosPorLaboratorio(int idLab) const;
    void asignarMedsSinLabAMadrid();

    unsigned int totalMedicamentos() const { return idMedication->getNumElementos(); }
    int totalLaboratorios() const { return laboratorios.size(); }
    int contarMedicamentosSinLab() const;
    int eliminarLabsPorCiudad(const std::string& ciudad);

    void suministrarFarmacia(Farmacia& f, int id_num, int n);
    std::vector<Farmacia*> buscarFarmacias(const std::string& provincia);
    bool eliminarMedicamento(int id_num);

    void mostrarEstadoTablaHash() {
        if(idMedication) idMedication->mostrarEstadoTabla();
    }
    void forzarCambioLambda(float nuevo_lambda) {
        if(idMedication) idMedication->setLambda(nuevo_lambda);
    }
    Farmacia* buscarFarmaciaPorCiudad(const std::string& ciudad);
};

#endif //MEDIEXPRESS_H