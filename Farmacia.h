#ifndef FARMACIA_H
#define FARMACIA_H

#include <string>
#include <iostream>
#include <vector>
#include <set> // Necesario para std::set
#include "Stock.h" // Necesario para la clase Stock

// Declaraciones adelantadas para evitar incluir los .h completos aquí
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

    /**
     * Contenedor de Stock.
     * Reemplaza al antiguo 'std::vector<PaMedicamento*> dispense'.
     * Almacena los objetos Stock, ordenados por id_PaMed.
     */
    std::set<Stock> order;


public:
    Farmacia();
    /**
     * @brief Busca el stock de un medicamento.
     * Método privado según el PDF[cite: 22].
     * @param id_num ID del medicamento.
     * @return El 'num_stock' si existe, o 0 si no existe[cite: 23].
     */
    int buscaMedicamID(int id_num);

    Farmacia(std::string cif, std::string provincia, std::string localidad,
             std::string nombre, std::string direccion, std::string codPostal,
             MediExpress* me);

    // --- Getters y Setters (sin cambios) ---
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
    MediExpress* getLinkMedi() const { return linkMedi; } // Getter para linkMedi

    // --- Operadores (sin cambios) ---
    bool operator<(const Farmacia& other) const;
    bool operator>(const Farmacia& other) const;
    bool operator==(const Farmacia& other) const;

    // --- MÉTODOS NUEVOS/ACTUALIZADOS DE LA PRÁCTICA 4 ---

    /**
     * @brief Simula la compra de un medicamento[cite: 24].
     * @param id_num ID del medicamento a comprar.
     * @param n Número de unidades solicitadas.
     * @param result Puntero de salida para devolver el medicamento (gestionado por MediExpress).
     * @return El stock *inicial* que había antes de la compra[cite: 30].
     */
    int comprarMedicam(int id_num, int n, PaMedicamento*& result);

    /**
     * @brief Solicita un pedido a MediExpress[cite: 31].
     * @param id_num ID del medicamento.
     * @param n Cantidad a pedir (según PDF [cite: 29]).
     */
    void pedidoMedicam(int id_num, int n);

    /**
     * @brief Añade/incrementa el stock de un medicamento[cite: 33].
     * Llamado por MediExpress::suministrarFarmacia.
     * @param pa Puntero al medicamento (para obtener el ID).
     * @param n Cantidad a añadir.
     */
    void nuevoStock(PaMedicamento* pa, int n);

    /**
     * @brief Elimina un medicamento del stock[cite: 34].
     * @param id_num ID del medicamento a eliminar.
     * @return true si se eliminó, false si no existía.
     */
    bool eliminarStock(int id_num);

    /**
     * @brief Busca medicamentos en el stock por nombre parcial[cite: 35].
     * @param nom Fragmento del nombre a buscar.
     * @return Un vector de punteros a PaMedicamento.
     */
    std::vector<PaMedicamento*> buscaMedicamNombre(const std::string& nom) const;
};

#endif // FARMACIA_H