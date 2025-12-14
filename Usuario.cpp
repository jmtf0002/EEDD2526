#include "Usuario.h"
#include <iostream>

// Constructor
Usuario::Usuario(int _id, std::string _nombre, std::string _dir, std::string _prov, double _lat, double _lon)
    : id(_id), nombre(_nombre), direccion(_dir), provincia(_prov), latitud(_lat), longitud(_lon) {}

// Getters básicos
int Usuario::getId() const { return id; }
std::string Usuario::getNombre() const { return nombre; }
std::string Usuario::getProvincia() const { return provincia; }
double Usuario::getLat() const { return latitud; }
double Usuario::getLon() const { return longitud; }

// Método: getFarmaciaCercana
// Busca en la malla usando la longitud como X y la latitud como Y
std::vector<Farmacia*> Usuario::getFarmaciaCercana(MallaRegular<Farmacia*>* malla, int n) {
    if (!malla) return std::vector<Farmacia*>();
    // Asumimos que la malla trabaja con (X=Longitud, Y=Latitud)
    return malla->buscarCercana(this->longitud, this->latitud, n);
}

// Método: quieroMedicam
// Delega la búsqueda a la farmacia (o a MediExpress a través de ella si fuera necesario).
// El PDF indica: "Entonces la farmacia busca el/los medicamento/s..."
std::vector<PaMedicamento*> Usuario::quieroMedicam(const std::string& nombre, Farmacia* farmacia) {
    if (!farmacia) return std::vector<PaMedicamento*>();

    // Necesitas asegurarte de que Farmacia tenga este método implementado.
    // Si tu clase Farmacia no tiene búsqueda por nombre, deberás añadirla o
    // usar MediExpress::buscarCompuesto desde el main.
    // Aquí asumimos la estructura ideal descrita en el PDF:
    return farmacia->buscaMedicamNombre(nombre);
}

// Método: comprarMedicam
// Intenta comprar y devuelve cuántas unidades se compró realmente.
int Usuario::comprarMedicam(PaMedicamento* med, int cant, Farmacia* farmacia) {
    if (!farmacia || !med) return 0;

    PaMedicamento* medComprado = nullptr;
    // Llamada al método de Farmacia (asumiendo firma de prácticas anteriores)
    // comprarMedicam(int idMedicamento, int cantidad, PaMedicamento*& retorno)
    int unidadesCompradas = farmacia->comprarMedicam(med->get_id_num(), cant, medComprado);

    return unidadesCompradas;
}