#ifndef MEDIEXPRESS_H
#define MEDIEXPRESS_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include "PaMedicamento.h"
#include "Laboratorio.h"
#include "Farmacia.h" // Ahora Farmacia se incluye aquí

std::vector<std::string> parsearFilaCSV(const std::string& linea);

class MediExpress {
private:
    std::map<int, PaMedicamento> medication;
    std::list<Laboratorio> laboratorios;
    std::vector<Farmacia> farmacias; // Usamos vector para farmacias según el UML

public:
    MediExpress(const std::string& archivo_meds, const std::string& archivo_labs, const std::string& archivo_farma);

    // --- Métodos de búsqueda (sin cambios en la firma) ---
    std::vector<PaMedicamento*> buscarCompuesto(const std::string& nombre);
    PaMedicamento* buscarCompuesto(int id_num); // Sobrecarga por ID
    std::list<Laboratorio*> buscarLabCiudad(const std::string& ciudad);
    std::list<Laboratorio*> buscarLabsPorCompuesto(const std::string& compuesto);
    Farmacia* buscarFarmacia(const std::string& cif); // Busca 1 farmacia

    // --- Métodos de utilidad (sin cambios en la firma) ---
    void imprimirMedicamentosPorLaboratorio(int idLab) const;
    void asignarMedsSinLabAMadrid();
    unsigned int totalMedicamentos() const { return medication.size(); }
    int totalLaboratorios() const { return laboratorios.size(); }
    int contarMedicamentosSinLab() const;
    int eliminarLabsPorCiudad(const std::string& ciudad);


    // --- MÉTODOS ACTUALIZADOS/NUEVOS DE LA PRÁCTICA 4 ---

    /**
     * @brief Suministra stock de un medicamento a una farmacia.
     * @param f Referencia a la farmacia (se modifica su stock).
     * @param id_num ID del medicamento.
     * @param n Cantidad de stock a suministrar.
     */
    void suministrarFarmacia(Farmacia& f, int id_num, int n); // <-- DECLARACIÓN CORREGIDA

    /**
     * @brief Devuelve todas las farmacias de una provincia.
     * @param provincia Nombre de la provincia.
     * @return Un vector de punteros a las Farmacias encontradas.
     */
    std::vector<Farmacia*> buscarFarmacias(const std::string& provincia); // <-- NUEVA DECLARACIÓN

    /**
     * @brief Elimina un medicamento del sistema.
     * Lo borra del map 'medication' y de todos los 'Stock' de las farmacias.
     * @param id_num ID del medicamento a eliminar.
     * @return true si se borró, false si no existía.
     */
    bool eliminarMedicamento(int id_num); // <-- NUEVA DECLARACIÓN

};

#endif //MEDIEXPRESS_H