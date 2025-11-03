#ifndef FARMACIA_H
#define FARMACIA_H

#include <string>
#include <iostream>
#include "VDinamico.h"
#include "PaMedicamento.h"

// --- Declaración Adelantada ---
// Necesaria porque 'linkMedi' es un puntero
class MediExpress;

class Farmacia {
private:
    std::string cif;
    std::string provincia;
    std::string localidad;
    std::string nombre;
    std::string direccion;
    std::string codPostal;

    // --- RELACIONES (Atributos UML) ---
    MediExpress* linkMedi;
    VDinamico<PaMedicamento*> stock;

public:
    // --- Constructores ---
    Farmacia(); // Constructor por defecto (Obligatorio para el AVL)

    // Declaración del constructor completo (la implementación va al .cpp)
    Farmacia(std::string cif, std::string provincia, std::string localidad,
             std::string nombre, std::string direccion, std::string codPostal,
             MediExpress* me);

    // --- Getters (pueden quedarse inline) ---
    std::string getCif() const { return cif; }
    std::string getProvincia() const { return provincia; }
    std::string getLocalidad() const { return localidad; }
    std::string getNombre() const { return nombre; }
    std::string getDireccion() const { return direccion; }
    std::string getCodPostal() const { return codPostal; }

    // --- Setters (pueden quedarse inline) ---
    void setCif(const std::string& c) { cif = c; }
    void setProvincia(const std::string& p) { provincia = p; }
    void setLocalidad(const std::string& l) { localidad = l; }
    void setNombre(const std::string& n) { nombre = n; }
    void setDireccion(const std::string& d) { direccion = d; }
    void setCodPostal(const std::string& cp) { codPostal = cp; }

    // --- Operadores (Obligatorios para el AVL, se quedan inline) ---
    bool operator<(const Farmacia& other) const {
        return this->cif < other.cif;
    }
    bool operator>(const Farmacia& other) const {
        return this->cif > other.cif;
    }
    bool operator==(const Farmacia& other) const {
        return this->cif == other.cif;
    }

    // --- MÉTODOS FUNCIONALES (Declaraciones) ---

    /**
     * @brief Localiza medicamentos en el stock local por nombre (parcial).
     */
    VDinamico<PaMedicamento*> localizarMedicamentosPorNombre(const std::string& nombre) const;

    /**
     * @brief Busca un medicamento en el stock local por ID.
     */
    PaMedicamento* buscaMedicam(int id_num);

    /**
     * @brief Pide un medicamento a MediExpress.
     */
    void pedidoMedicam(int id_num);

    /**
     * @brief Añade un medicamento al stock. (Llamado por MediExpress)
     */
    void dispensaMedicam(PaMedicamento* pa);
};

#endif // FARMACIA_H