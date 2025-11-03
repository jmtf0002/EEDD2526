#ifndef FARMACIA_H
#define FARMACIA_H

#include <string>
#include <iostream>
#include "VDinamico.h"      // Para el stock
#include "PaMedicamento.h"  // Para el stock

// --- Declaración Adelantada ---
// Es IMPRESCINDIBLE para que el compilador sepa qué es "MediExpress*"
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

    // Puntero a la central de MediExpress (relación 'linkMedi')
    // Necesita la declaración adelantada "class MediExpress;"
    MediExpress* linkMedi;
    
    // Stock de medicamentos de la farmacia (relación 'dispense')
    // Asumimos que guardas punteros a los medicamentos que te da MediExpress
    VDinamico<PaMedicamento*> stock;

public:
    // --- Constructores ---

    // Constructor por defecto (Obligatorio para el AVL)
    Farmacia() : linkMedi(nullptr) {}

    // Constructor completo (7 argumentos)
    Farmacia(std::string cif, std::string provincia, std::string localidad,
             std::string nombre, std::string direccion, std::string codPostal,
             MediExpress* me) // "me" es el puntero a MediExpress
        : cif(cif), provincia(provincia), localidad(localidad),
          nombre(nombre), direccion(direccion), codPostal(codPostal), 
          linkMedi(me) {
        // 'stock' se inicializa vacío automáticamente
    }

    // --- Getters ---
    std::string getCif() const { return cif; }
    std::string getProvincia() const { return provincia; }
    std::string getLocalidad() const { return localidad; }
    std::string getNombre() const { return nombre; }
    std::string getDireccion() const { return direccion; }
    std::string getCodPostal() const { return codPostal; }

    // --- Setters ---
    // Necesario para poder buscar en el AVL
    void setCif(const std::string& c) { cif = c; }
    void setProvincia(const std::string& p) { provincia = p; }
    void setLocalidad(const std::string& l) { localidad = l; }
    void setNombre(const std::string& n) { nombre = n; }
    void setDireccion(const std::string& d) { direccion = d; }
    void setCodPostal(const std::string& cp) { codPostal = cp; }


    // --- Operadores (Obligatorios para el AVL<Farmacia>) ---
    // El AVL necesita saber cómo comparar Farmacias (lo hacemos por CIF)

    /**
       * @brief Operador "menor que".
       * Necesario para que el AVL decida si ir a la izquierda.
       * Es crucial que sea 'const' al final.
       */
    bool operator<(const Farmacia& other) const {
        return this->cif < other.cif;
    }

    /**
     * @brief Operador "mayor que".
     * Necesario para que el AVL decida si ir a la derecha.
     * Es crucial que sea 'const' al final.
     */
    bool operator>(const Farmacia& other) const {
        return this->cif > other.cif;
    }

    /**
     * @brief Operador "igual que".
     * Necesario para la búsqueda (buscaRec) y para que el AVL
     * sepa si un elemento ya existe.
     */
    bool operator==(const Farmacia& other) const {
        return this->cif == other.cif;
    }

    // --- MÉTODOS FUNCIONALES (Según UML) ---

    // En Farmacia.h (dentro de la sección public:)

    /**
     * @brief Localiza medicamentos en el stock local por nombre (parcial).
     * @param nombre Búsqueda parcial por cadena (case-sensitive).
     * @return Un vector de punteros a los medicamentos encontrados en el stock.
     */
    VDinamico<PaMedicamento*> localizarMedicamentosPorNombre(const std::string& nombre) const;
    /**
     * @brief Busca un medicamento en el stock local por ID.
     * @return Puntero al medicamento si se encuentra, nullptr si no.
     */
    PaMedicamento* buscaMedicam(int id_num) {
        for (unsigned i = 0; i < stock.tamlog(); ++i) {
            // Asumimos que PaMedicamento tiene un getter para su ID
            if (stock[i]->get_id_num() == id_num) {
          //      std::cout << "Farmacia " << nombre << ": Med " << id_num << " encontrado en stock." << std::endl;
                return stock[i];
            }
        }
     //   std::cout << "Farmacia " << nombre << ": Med " << id_num << " NO encontrado en stock." << std::endl;
        return nullptr;
    }

    /**
     * @brief Pide un medicamento a MediExpress.
     * Llama a MediExpress a través del puntero 'linkMedi'.
     */
    void pedidoMedicam(int id_num);

    /**
     * @brief Añade un medicamento al stock. (Llamado por MediExpress)
     */
    void dispensaMedicam(PaMedicamento* pa) {
        if (pa) {

            // --- AÑADE ESTE IF DE DEBUG ---
            if (pa->get_id_num() == 3640) {
                std::cout << "DEBUG: La farmacia " << nombre
                          << " acaba de recibir el OXIDO DE MAGNESIO (ID 3640)." << std::endl;
            }

            // Esta es la línea que HACE la asignación
            stock.insertar(pa, stock.tamlog());
        }
    }
};

#endif // FARMACIA_H