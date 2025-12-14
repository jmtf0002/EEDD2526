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
#include "MallaRegular.h" // Nueva dependencia
#include "Usuario.h"      // Nueva dependencia

class Farmacia; // Forward declaration
class Usuario;  // Forward declaration

std::vector<std::string> parsearFilaCSV(const std::string& linea);

class MediExpress {
private:
    ThashMedicam* idMedication;
    std::multimap<std::string, PaMedicamento*> nombMedication;
    std::list<Laboratorio> laboratorios;
    std::multimap<std::string, Farmacia> farmacias;

    // --- NUEVOS ATRIBUTOS PRÁCTICA 6 ---
    std::map<int, Usuario*> users; // Mapa de usuarios por ID
    MallaRegular<Farmacia*>* grid; // La malla regular de punteros a farmacia
    // -----------------------------------

public:
    // Constructor actualizado con el fichero de usuarios
    MediExpress(const std::string& archivo_meds, const std::string& archivo_labs,
                const std::string& archivo_farma, const std::string& archivo_users);

    ~MediExpress();

    // Métodos existentes
    std::vector<PaMedicamento*> buscarCompuesto(const std::string& nombre);
    PaMedicamento* buscarCompuesto(int id_num);
    std::list<Laboratorio*> buscarLabCiudad(const std::string& ciudad);
    std::list<Laboratorio*> buscarLabsPorCompuesto(const std::string& compuesto);
    Farmacia* buscarFarmacia(const std::string& cif);
    Farmacia* buscarFarmaciaPorCiudad(const std::string& ciudad);
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

    // --- NUEVOS MÉTODOS PARA ACCEDER A LA ESTRUCTURA ---
    std::map<int, Usuario*>& getUsuarios() { return users; }
    MallaRegular<Farmacia*>* getMalla() { return grid; }
    // ----------------------------------------------------
};

#endif // MEDIEXPRESS_H