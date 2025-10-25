#ifndef FARMACIA_H
#define FARMACIA_H

#include <string>
#include "PaMedicamento.h"
using namespace std;

class Farmacia {
private:
    string cif;
    string provincia;
    string localidad;
    string nombre;
    string direccion;
    string codPostal;

public:
    // --- Constructores ---
    Farmacia() = default;

    Farmacia(string cif, string provincia, string localidad,
             string nombre, string direccion, string codPostal)
        : cif(cif), provincia(provincia), localidad(localidad),
          nombre(nombre), direccion(direccion), codPostal(codPostal) {}

    // --- Getters ---
    string getCif() const { return cif; }
    string getProvincia() const { return provincia; }
    string getLocalidad() const { return localidad; }
    string getNombre() const { return nombre; }
    string getDireccion() const { return direccion; }
    string getCodPostal() const { return codPostal; }

    // --- Setters ---
    void setCif(const string& c) { cif = c; }
    void setProvincia(const string& p) { provincia = p; }
    void setLocalidad(const string& l) { localidad = l; }
    void setNombre(const string& n) { nombre = n; }
    void setDireccion(const string& d) { direccion = d; }
    void setCodPostal(const string& cp) { codPostal = cp; }

    // --- Métodos funcionales ---
    // Simula un pedido de medicamento (por id)
    void pedidoMedicam(int id_num) {
        // Aquí se podría implementar la lógica para registrar o enviar un pedido
        cout << "Pedido de medicamento con ID " << id_num
             << " realizado por la farmacia " << nombre << "." << endl;
    }

    // Busca un medicamento según su ID (retorna un objeto PaMedicamento)
    PaMedicamento buscaMedicam(int id_num) {
        // Lógica simulada: en un sistema real buscaría en una base de datos o lista
        cout << "Buscando medicamento con ID " << id_num
             << " en la farmacia " << nombre << "." << endl;

        // Retorna un medicamento de ejemplo
        return PaMedicamento(id_num, "ALPHA" + to_string(id_num), "MedicamentoEjemplo");
    }

    // Dispensa un medicamento (lo entrega al cliente)
    void dispensaMedicam(const PaMedicamento& pa) {
        cout << "La farmacia " << nombre
             << " dispensa el medicamento: " << pa.get_nombre()<< "." << endl;
    }
};

#endif // FARMACIA_H

