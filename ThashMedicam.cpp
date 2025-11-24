//
// Created by javim on 24/11/2025.
//

#include "ThashMedicam.h"

bool ThashMedicam::es_primo(int n) const {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0) return false;
    return true;
}

int ThashMedicam::siguiente_primo(int n) const {
    int primo = n;
    // Empezar a buscar en n, no en n+1, por si n es primo
    while (!es_primo(primo)) {
        primo++;
    }
    return primo;
}

ThashMedicam::ThashMedicam(int maxElementos, float lambda)
    : lambda_max(lambda), n_elementos(0), current_hash_type(DOUBLE) { // Inicializamos con DOUBLE

    // Cálculo de T: T >= maxElementos / lambda
    int tam_minimo = (int)std::ceil(maxElementos / lambda);
    T = siguiente_primo(tam_minimo);

    tabla.resize(T);
}

ThashMedicam::~ThashMedicam() {
    // Liberación de memoria: solo liberar los objetos PaMedicamento apuntados
    for (int i = 0; i < T; ++i) {
        if (tabla[i].estado == OCCUPIED) {
            delete tabla[i].medicamento;
        }
    }
}

ThashMedicam::ThashMedicam(const ThashMedicam &thash)
    : T(thash.T), n_elementos(thash.n_elementos), tabla(thash.T),
      lambda_max(thash.lambda_max), current_hash_type(thash.current_hash_type) {

    // Copia profunda de los elementos OCCUPIED
    for (int i = 0; i < T; ++i) {
        tabla[i].estado = thash.tabla[i].estado;
        if (tabla[i].estado == OCCUPIED) {
            // Se asume que PaMedicamento tiene un constructor copia
            tabla[i].medicamento = new PaMedicamento(*thash.tabla[i].medicamento);
        } else {
            tabla[i].medicamento = nullptr;
        }
    }
}

ThashMedicam& ThashMedicam::operator=(const ThashMedicam &thash) {
    if (this != &thash) {
        // 1. Liberar recursos existentes
        for (int i = 0; i < T; ++i) {
            if (tabla[i].estado == OCCUPIED) {
                delete tabla[i].medicamento;
            }
        }

        // 2. Copiar parámetros y redimensionar
        T = thash.T;
        n_elementos = thash.n_elementos;
        lambda_max = thash.lambda_max;
        current_hash_type = thash.current_hash_type;
        tabla.resize(T);

        // 3. Copia profunda
        for (int i = 0; i < T; ++i) {
            tabla[i].estado = thash.tabla[i].estado;
            if (tabla[i].estado == OCCUPIED) {
                tabla[i].medicamento = new PaMedicamento(*thash.tabla[i].medicamento);
            } else {
                tabla[i].medicamento = nullptr;
            }
        }
    }
    return *this;
}

// Colisiones

int ThashMedicam::hash(unsigned long clave, int intento) const {
    int pos_final = 0;

    if (current_hash_type == DOUBLE) {
        // h(k, i) = (h1(k) + i * h2(k)) mod T
        pos_final = (h1(clave) + intento * h2(clave)) % T;
    } else if (current_hash_type == QUADRATIC) {
        // h(k, i) = (h1(k) + i^2) mod T
        pos_final = (h1(clave) + intento * intento) % T;
    }

    // Asegurar que la posición sea positiva
    return (pos_final < 0) ? pos_final + T : pos_final;
}

bool ThashMedicam::insertar(unsigned long clave, PaMedicamento &pa) {
    int intento = 0;
    int pos;
    // Guarda la primera posición DELETED encontrada para una posible inserción
    int pos_deleted = -1;

    // El sondeo debe revisar T slots como máximo para garantizar terminación
    while (intento < T) {
        pos = hash(clave, intento);

        if (tabla[pos].estado == OCCUPIED) {
            // Verificar si el elemento ya está (no se permiten repetidos)
            if (tabla[pos].medicamento->get_id_num() == clave) {
                return false;
            }
        } else if (tabla[pos].estado == DELETED) {
            // Guardar la primera posición DELETED como sitio de inserción prioritario
            if (pos_deleted == -1) {
                pos_deleted = pos;
            }
        } else { // tabla[pos].estado == EMPTY
            // 1. Se encontró un slot EMPTY.
            // 2. El sitio de inserción será el primer DELETED encontrado o el EMPTY actual.
            int pos_final = (pos_deleted != -1) ? pos_deleted : pos;

            // Si insertamos en un DELETED, liberamos el puntero que ya estaba en el Slot.
            if (tabla[pos_final].estado == DELETED) {
                delete tabla[pos_final].medicamento; // Asegura que el puntero anterior se libere si estaba
            }

            // Inserción: Creación de copia en memoria dinámica (Heap)
            tabla[pos_final].medicamento = new PaMedicamento(pa);
            tabla[pos_final].estado = OCCUPIED;
            n_elementos++;

            return true;
        }

        intento++;
    }

    // la tabla está llena (n_elementos == T) o hubo un error.
    return false;
}

PaMedicamento  * ThashMedicam::buscar(unsigned long clave) {
    int intento = 0;
    int pos;

    while (intento < T) {
        pos = hash(clave, intento);

        if (tabla[pos].estado == OCCUPIED) {
            // Encontrado: Comprobar la clave
            if (tabla[pos].medicamento->get_id_num() == clave) {
                return tabla[pos].medicamento;
            }
        } else if (tabla[pos].estado == EMPTY) {
            // EMPTY: La clave no está ni puede estar más adelante en esta secuencia.
            return nullptr;
        }
        // Si es DELETED, debemos seguir explorando

        intento++;
    }

    return nullptr; // No encontrado después de explorar toda la tabla
}

bool ThashMedicam::borrar(unsigned long clave) {
    int intento = 0;
    int pos;

    while (intento < T) {
        pos = hash(clave, intento);

        if (tabla[pos].estado == OCCUPIED) {
            if (tabla[pos].medicamento->get_id_num() == clave) {
                // Encontrado: Cambiar estado a DELETED y liberar memoria del objeto
                tabla[pos].estado = DELETED;
                delete tabla[pos].medicamento;
                tabla[pos].medicamento = nullptr; // Poner a nullptr para seguridad
                n_elementos--;
                return true;
            }
        } else if (tabla[pos].estado == EMPTY) {
            // EMPTY: La clave no existe
            return false;
        }
        // Si es DELETED, seguimos buscando (probar la siguiente posición de sondeo)

        intento++;
    }

    return false; // No encontrado
}