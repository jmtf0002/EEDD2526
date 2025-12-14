#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <vector>
#include "Farmacia.h"
#include "MallaRegular.h"  // Dependencia necesaria para buscar en la malla
#include "PaMedicamento.h" // Dependencia para devolver medicamentos

class Usuario {
private:
    int id;
    std::string nombre;
    std::string direccion;
    std::string provincia; // Para distinguir entre usuarios de Jaén, Sevilla, etc.
    double latitud;        // Coordenada Y (aprox)
    double longitud;       // Coordenada X (aprox)

public:
    // Constructor completo
    Usuario(int id, std::string nombre, std::string direccion, std::string provincia, double lat, double lon);

    // Getters
    int getId() const;
    std::string getNombre() const;
    std::string getProvincia() const;
    double getLat() const;
    double getLon() const;

    // --- MÉTODOS REQUERIDOS POR EL PDF (Pág. 2 y UML) ---

    /**
     * Utiliza la malla regular para encontrar las 'n' farmacias más cercanas
     * a la posición actual del usuario.
     * @param malla Puntero a la estructura de MallaRegular cargada con farmacias.
     * @param n Número de farmacias a buscar (por defecto 1).
     * @return Vector con punteros a las farmacias encontradas.
     */
    std::vector<Farmacia*> getFarmaciaCercana(MallaRegular<Farmacia*>* malla, int n = 1);

    /**
     * El usuario pregunta a una farmacia por un medicamento (nombre completo o parcial).
     * @param nombre Parte del nombre del medicamento (ej: "MAGNESIO").
     * @param farmacia Puntero a la farmacia donde se consulta.
     * @return Vector de medicamentos que coinciden con la búsqueda.
     */
    std::vector<PaMedicamento*> quieroMedicam(const std::string& nombre, Farmacia* farmacia);

    /**
     * El usuario compra una cantidad específica de un medicamento en la farmacia.
     * @param med Puntero al medicamento a comprar.
     * @param cant Número de unidades deseadas.
     * @param farmacia Farmacia donde se realiza la compra.
     * @return Número de unidades realmente compradas (depende del stock).
     */
    int comprarMedicam(PaMedicamento* med, int cant, Farmacia* farmacia);
};

#endif // USUARIO_H