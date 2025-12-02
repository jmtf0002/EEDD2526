#ifndef THASHMEDICAM_H
#define THASHMEDICAM_H

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

#include "PaMedicamento.h"

enum Estado {
    EMPTY,      // 0: Nunca ocupado
    OCCUPIED,   // 1: Ocupado con un elemento
    DELETED     // 2: Borrado (tumba)
};

struct Slot {
    PaMedicamento* medicamento;
    Estado estado;
    Slot() : medicamento(nullptr), estado(EMPTY) {}
};

class ThashMedicam {
public:
    enum HashType { QUADRATIC, DOUBLE_1, DOUBLE_2 };

private:
    int T;
    int n_elementos;
    std::vector<Slot> tabla;
    float lambda_max;
    HashType current_hash_type;

    // Estadísticas
    unsigned int max_colisiones;
    unsigned long total_colisiones;
    unsigned int num_ops_insertar;
    unsigned int num_max_10;
    unsigned int num_redispersiones;

    // Hash helpers
    int h1(unsigned long clave) const { return clave % T; }
    int h2_v1(unsigned long clave) const;
    int h2_v2(unsigned long clave) const;
    int hash(unsigned long clave, int intento) const;

    bool es_primo(int n) const;
    int siguiente_primo(int n) const;
    int anterior_primo(int n) const;

public:
    ThashMedicam(int maxElementos, float lambda = 0.7);
    ThashMedicam(const ThashMedicam &thash);
    ThashMedicam& operator=(const ThashMedicam &thash);
    ~ThashMedicam();

    bool insertar(unsigned long clave, PaMedicamento &pa);
    PaMedicamento* buscar(unsigned long clave);
    bool borrar(unsigned long clave);

    void redispersar(unsigned int nuevo_tam);
    void setLambda(float l);

    // Getters y Setters
    int getM() const { return T; }
    int getNumElementos() const { return n_elementos; }
    void setHashType(HashType type) { current_hash_type = type; }

    // Estadísticas públicas
    float factorCarga() const { return (float)n_elementos / T; }
    unsigned int tamTabla() const { return (unsigned int)T; }
    unsigned int maxColisiones() const { return max_colisiones; }
    unsigned int numMax10() const { return num_max_10; }
    unsigned long getTotalColisiones() const { return total_colisiones; }     // NUEVO
    unsigned int getNumRedispersiones() const { return num_redispersiones; }  // NUEVO

    float promedioColisiones() const {
        if (num_ops_insertar == 0) return 0.0f;
        return (float)total_colisiones / num_ops_insertar;
    }

    // Modificado para imprimir con el formato exacto del Ejercicio 5
    void mostrarEstadoTabla();

    std::vector<PaMedicamento*> getEntradasValidas() const;
};

#endif //THASHMEDICAM_H