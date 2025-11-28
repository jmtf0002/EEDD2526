//
// Created by javim on 24/11/2025.
//

#ifndef THASHMEDICAM_H
#define THASHMEDICAM_H

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#include "PaMedicamento.h"

// Estado de cada celda de la tabla hash
enum Estado {
    EMPTY,      // 0: Nunca ocupado
    OCCUPIED,   // 1: Ocupado con un elemento
    DELETED     // 2: Borrado (tumba)
};

// Estructura de las celdas
struct Slot {
    PaMedicamento* medicamento;
    Estado estado;

    Slot() : medicamento(nullptr), estado(EMPTY) {}
};

class ThashMedicam {
public:
    // Tipos de dispersión disponibles
    enum HashType { QUADRATIC, DOUBLE };

private:
    int T;              // Tamaño de la tabla (primo)
    int n_elementos;    // Número de elementos ocupados
    std::vector<Slot> tabla;
    float lambda_max;   // Factor de carga máximo permitido
    HashType current_hash_type; // Tipo de dispersión configurada

    // --- ESTADÍSTICAS DE RENDIMIENTO ---
    unsigned int max_colisiones;    // Máximo de colisiones en una sola inserción
    unsigned long total_colisiones; // Total acumulado de colisiones
    unsigned int num_ops_insertar;  // Total de operaciones de inserción
    unsigned int num_max_10;        // Inserciones que superaron 10 colisiones
    unsigned int num_redispersiones;// Número de veces que se ha redimensionado la tabla

    // --- FUNCIONES DE HASH PRIVADAS ---
    // h1(k) = k mod T
    int h1(unsigned long clave) const {
        return clave % T;
    }

    // h2(k) para Dispersión Doble: 1 + (k mod R)
    int h2(unsigned long clave) const {
        // Usamos R = T - 2 (o 1 si T es muy pequeño) para asegurar R < T
        int R_dyn = (T > 2) ? T - 2 : 1;
        return 1 + (clave % R_dyn);
    }

    /**
     * @brief Función de dispersión principal.
     * Calcula la posición basándose en el tipo (Quadratic o Double).
     */
    int hash(unsigned long clave, int intento) const;

    // Funciones auxiliares matemáticas
    bool es_primo(int n) const;
    int siguiente_primo(int n) const;

public:
    // --- CONSTRUCTORES Y DESTRUCTOR ---
    /**
     * @brief Constructor. Calcula T para cumplir lambda.
     */
    ThashMedicam(int maxElementos, float lambda = 0.7);
    ThashMedicam(const ThashMedicam &thash);
    ThashMedicam& operator=(const ThashMedicam &thash);
    ~ThashMedicam();

    // --- OPERACIONES PRINCIPALES ---
    /**
     * @brief Inserta un medicamento. Si supera lambda, redispersa.
     * @return true si se insertó, false si ya existía.
     */
    bool insertar(unsigned long clave, PaMedicamento &pa);

    /**
     * @brief Busca un medicamento por su ID.
     */
    PaMedicamento* buscar(unsigned long clave);

    /**
     * @brief Borra un medicamento (marca como DELETED).
     */
    bool borrar(unsigned long clave);

    // --- MÉTODOS DE REDISPERSIÓN (PAREJAS) ---
    void redispersar(unsigned int nuevo_tam);

    /**
     * @brief Cambia el factor de carga máximo.
     * Si el nuevo lambda es menor que el actual, fuerza redispersión inmediata.
     */
    void setLambda(float l);

    // --- GETTERS Y UTILIDADES ---
    int getM() const { return T; }
    int getNumElementos() const { return n_elementos; }
    void setHashType(HashType type) { current_hash_type = type; }

    // Estadísticas
    float factorCarga() const { return (float)n_elementos / T; }
    unsigned int tamTabla() const { return (unsigned int)T; }
    unsigned int maxColisiones() const { return max_colisiones; }
    unsigned int numMax10() const { return num_max_10; }
    float promedioColisiones() const {
        if (num_ops_insertar == 0) return 0.0f;
        return (float)total_colisiones / num_ops_insertar;
    }

    // Utilidades para MediExpress
    void mostrarEstadoTabla();

    /**
     * @brief Devuelve punteros a todos los medicamentos válidos.
     * Necesario para recorrer la tabla desde fuera (iteradores).
     */
    std::vector<PaMedicamento*> getEntradasValidas() const;
};

#endif //THASHMEDICAM_H