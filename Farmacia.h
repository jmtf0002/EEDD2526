#ifndef FARMACIA_H
#define FARMACIA_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include "Stock.h"

class MediExpress;
class PaMedicamento;

class Farmacia {
private:
    std::string cif;
    std::string provincia;
    std::string localidad;
    std::string nombre;
    std::string direccion;
    std::string codPostal;
    MediExpress* linkMedi;
    std::map<int,Stock> order;
    int buscaMedicamID(int id_num);

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
    MediExpress* getLinkMedi() const { return linkMedi; }
    bool operator<(const Farmacia& other) const;
    bool operator>(const Farmacia& other) const;
    bool operator==(const Farmacia& other) const;


    int comprarMedicam(int id_num, int n, PaMedicamento*& result);


    void pedidoMedicam(int id_num, int n);


    void nuevoStock(PaMedicamento* pa, int n);


    bool eliminarStock(int id_num);


    int consultarStock(int id_num) const;


    std::vector<PaMedicamento*> buscaMedicamNombre(const std::string& nom) const;
};

#endif // FARMACIA_H