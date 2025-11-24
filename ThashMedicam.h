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

//Estado de cada celda
enum Estado {
    EMPTY,      // 0: Nunca ocupado
    OCCUPIED,   // 1: Ocupado con un elemento.
    DELETED     // 2: Borrado.
};

// Estructura de las celdas
struct Slot {
    PaMedicamento* medicamento;
    Estado estado;

    Slot() : medicamento(nullptr), estado(EMPTY) {}

};

class ThashMedicam {
private:
    int T; // que sea primo
    int n_elementos; // Contador de elementos ocupados
    std::vector<Slot> tabla;
    float lambda_max; // Factor de carga máximo permitido

    // Tipos de dispersión
    enum HashType { QUADRATIC, DOUBLE };
    HashType current_hash_type;

    // h1(k) = k mod M (posición inicial)
    int h1(unsigned long clave) const {
        return clave % T;
    }

    // h2(k) para Dispersión Doble
    // Variantes: h2(k) = R - (k mod R), donde R es un primo < M
    // Usamos un R fijo para este ejemplo, pero luego si no se cambia
    int h2(unsigned long clave) const {
        // R debe ser un primo menor que T. Usaremos T - 1 como base.
        // Se debe asegurar que h2(k) nunca sea 0.
        // Asumimos R = T - 2 (si T es primo > 2)
        const int R = 31; // Constante pequeña para garantizar R < T
        return 1 + (clave % R);
    }

    // funcion de dispersión

    /**
     * @brief Función de dispersión con secuencia de sondeo.
     * @param clave Clave de dispersión (ID numérico).
     * @param intento Número de sondeo (i).
     * @return int La posición final h(k, i).
     */
    int hash(unsigned long clave, int intento) const;

    // --- FUNCIONES AUXILIARES ---
    bool es_primo(int n) const;
    int siguiente_primo(int n) const;

public:
    // --- CONSTRUCTORES Y DESTRUCTORES ---

    /**
     * @brief Constructor principal. Calcula T para garantizar lambda_max.
     * @param maxElementos Número máximo de elementos esperados.
     * @param lambda Factor de carga determinado (por defecto 0.7).
     */
    ThashMedicam(int maxElementos, float lambda = 0.7);

    /**
     * @brief Constructor copia (Copia Profunda).
     */
    ThashMedicam(const ThashMedicam &thash);

    /**
     * @brief Operador de asignación (Copia Profunda).
     */
    ThashMedicam& operator=(const ThashMedicam &thash);

    /**
     * @brief Destructor (Libera memoria de los PaMedicamento*).
     */
    ~ThashMedicam();


    /**
     * @brief Inserta un PaMedicamento. No permite repetidos.
     * @param clave Clave (ID).
     * @param pa Objeto PaMedicamento a insertar.
     * @return bool Verdadero si se insertó, falso si ya existía.
     */
    bool insertar(unsigned long clave, PaMedicamento &pa);

    /**
     * @brief Busca un PaMedicamento.
     * @param clave Clave (ID).
     * @return Medicam* Puntero al objeto (o nullptr).
     */
    PaMedicamento* buscar(unsigned long clave);

    /**
     * @brief Borra un PaMedicamento (marca como DELETED).
     * @param clave Clave (ID).
     * @return bool Verdadero si se borró, falso si no se encontró.
     */
    bool borrar(unsigned long clave);


    int getM() const { return T; }
    int getNumElementos() const { return n_elementos; }
    float getLambda() const { return (float)n_elementos / T; }
    void setHashType(HashType type) { current_hash_type = type; }

    // void muestraEstadoTabla() const; // Se puede añadir si se requiere.
    // void redispersar(unsigned tam);
};



#endif //THASHMEDICAM_H
