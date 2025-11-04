#ifndef FARMACIA_H
#define FARMACIA_H

#include <string>
#include <iostream>
#include "VDinamico.h"
#include "PaMedicamento.h"

// --- Declaración Adelantada ---
class MediExpress;

class Farmacia {
private:
    std::string cif;
    std::string provincia;
    std::string localidad;
    std::string nombre;
    std::string direccion;
    std::string codPostal;

    MediExpress* linkMedi;
    VDinamico<PaMedicamento*> dispense;

public:
    Farmacia();

    Farmacia(std::string cif, std::string provincia, std::string localidad,
             std::string nombre, std::string direccion, std::string codPostal,
             MediExpress* me);

    std::string getCif() const { return cif; }

    std::string getProvincia() const { return provincia; }

    std::string getLocalidad() const { return localidad; }

    std::string getNombre() const { return nombre; }

    std::string getDireccion() const { return direccion; }

    std::string getCodPostal() const { return codPostal; }

    void setCif(const std::string& c) { cif = c; }

    void setProvincia(const std::string& p) { provincia = p; }

    void setLocalidad(const std::string& l) { localidad = l; }

    void setNombre(const std::string& n) { nombre = n; }

    void setDireccion(const std::string& d) { direccion = d; }

    void setCodPostal(const std::string& cp) { codPostal = cp; }


    bool operator<(const Farmacia& other) const {
        return this->cif < other.cif;
    }

    bool operator>(const Farmacia& other) const {
        return this->cif > other.cif;
    }

    bool operator==(const Farmacia& other) const {
        return this->cif == other.cif;
    }

    VDinamico<PaMedicamento*> localizarMedicamentosPorNombre(const std::string& nombre) const;

    PaMedicamento* buscaMedicam(int id_num);

    void pedidoMedicam(int id_num);

    void dispensaMedicam(PaMedicamento* pa);
};

#endif // FARMACIA_H