//
// Created by javim on 24/11/2025.
//

#include "ThashMedicam.h"
#include <iostream>
#include <cmath>

// --- FUNCIONES AUXILIARES ---
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
    while (!es_primo(primo)) {
        primo++;
    }
    return primo;
}

// --- CONSTRUCTOR Y DESTRUCTOR ---
ThashMedicam::ThashMedicam(int maxElementos, float lambda)
    : lambda_max(lambda), n_elementos(0), current_hash_type(DOUBLE),
      max_colisiones(0), total_colisiones(0), num_ops_insertar(0), num_max_10(0),
      num_redispersiones(0) {

    // Calculamos tamaño T para garantizar factor de carga < lambda
    int tam_minimo = (int)std::ceil(maxElementos / lambda);
    T = siguiente_primo(tam_minimo);
    tabla.resize(T);
}

ThashMedicam::~ThashMedicam() {
    for (int i = 0; i < T; ++i) {
        if (tabla[i].estado == OCCUPIED) {
            delete tabla[i].medicamento;
        }
    }
}

// --- COPIA Y ASIGNACIÓN ---
ThashMedicam::ThashMedicam(const ThashMedicam &thash)
    : T(thash.T), n_elementos(thash.n_elementos), tabla(thash.T),
      lambda_max(thash.lambda_max), current_hash_type(thash.current_hash_type),
      max_colisiones(thash.max_colisiones), total_colisiones(thash.total_colisiones),
      num_ops_insertar(thash.num_ops_insertar), num_max_10(thash.num_max_10),
      num_redispersiones(thash.num_redispersiones) {

    for (int i = 0; i < T; ++i) {
        tabla[i].estado = thash.tabla[i].estado;
        if (tabla[i].estado == OCCUPIED) {
            tabla[i].medicamento = new PaMedicamento(*thash.tabla[i].medicamento);
        } else {
            tabla[i].medicamento = nullptr;
        }
    }
}

ThashMedicam& ThashMedicam::operator=(const ThashMedicam &thash) {
    if (this != &thash) {
        // 1. Liberar memoria actual
        for (int i = 0; i < T; ++i) {
            if (tabla[i].estado == OCCUPIED) delete tabla[i].medicamento;
        }

        // 2. Copiar atributos
        T = thash.T;
        n_elementos = thash.n_elementos;
        lambda_max = thash.lambda_max;
        current_hash_type = thash.current_hash_type;

        // Copiar estadísticas
        max_colisiones = thash.max_colisiones;
        total_colisiones = thash.total_colisiones;
        num_ops_insertar = thash.num_ops_insertar;
        num_max_10 = thash.num_max_10;
        num_redispersiones = thash.num_redispersiones;

        // 3. Copia profunda de la tabla
        tabla.resize(T);
        for (int i = 0; i < T; ++i) {
            tabla[i].estado = thash.tabla[i].estado;
            if (tabla[i].estado == OCCUPIED) {
                tabla[i].medicamento = new PaMedicamento(*thash.tabla[i].medicamento);
            } else {
                tabla[i].medicamento = nullptr;
                tabla[i].estado = thash.tabla[i].estado;
            }
        }
    }
    return *this;
}

// --- FUNCIÓN HASH ---
int ThashMedicam::hash(unsigned long clave, int intento) const {
    unsigned long pos_final = 0;

    if (current_hash_type == DOUBLE) {
        // h(k, i) = (h1(k) + i * h2(k)) % T
        pos_final = (h1(clave) + (unsigned long)intento * h2(clave)) % T;
    } else { // QUADRATIC
        // h(k, i) = (h1(k) + i*i) % T
        pos_final = (h1(clave) + (unsigned long)intento * intento) % T;
    }

    return (int)pos_final;
}

// --- REDISPERSIÓN ---
void ThashMedicam::redispersar(unsigned int nuevo_tam) {
    std::cout << "  [ThashMedicam] Redispersando tabla... (T anterior: " << T << ", Nuevo T: " << nuevo_tam << ")" << std::endl;
    num_redispersiones++;

    // 1. Guardar todos los elementos actuales
    std::vector<PaMedicamento*> antiguos = getEntradasValidas();

    // 2. Limpiar la tabla actual (sin borrar los objetos de memoria todavía)
    tabla.clear();

    // 3. Reconfigurar tabla
    T = nuevo_tam;
    tabla.resize(T); // Todo a EMPTY
    n_elementos = 0; // Se recalculará al insertar

    // 4. Reinsertar los objetos en la nueva tabla
    for (PaMedicamento* p : antiguos) {
        insertar(p->get_id_num(), *p); // Inserta copia nueva
        delete p; // Borramos el objeto antiguo para no tener duplicados en heap
    }
}

void ThashMedicam::setLambda(float l) {
    lambda_max = l;
    // Comprobación inmediata para el ejercicio de parejas
    if (factorCarga() > lambda_max) {
        std::cout << "  [ThashMedicam] Nuevo lambda (" << l << ") menor que factor actual. Forzando redispersion." << std::endl;
        int nuevo_tam = siguiente_primo((int)(T * 1.30));
        redispersar(nuevo_tam);
    }
}

// --- INSERTAR ---
bool ThashMedicam::insertar(unsigned long clave, PaMedicamento &pa) {
    // 1. Comprobar factor de carga antes de insertar
    if (factorCarga() >= lambda_max) {
        int nuevo_tam = siguiente_primo((int)(T * 1.30));
        redispersar(nuevo_tam);
    }

    int intento = 0;
    int pos;
    int pos_deleted = -1;
    num_ops_insertar++;

    while (intento < T) {
        pos = hash(clave, intento);

        if (tabla[pos].estado == OCCUPIED) {
            // Duplicado encontrado
            if (tabla[pos].medicamento->get_id_num() == clave) {
                return false;
            }
        } else if (tabla[pos].estado == DELETED) {
            // Recordamos el primer hueco borrado
            if (pos_deleted == -1) pos_deleted = pos;
        } else { // EMPTY
            // --- ESTADÍSTICAS ---
            if (intento > max_colisiones) max_colisiones = intento;
            total_colisiones += intento;
            if (intento > 10) num_max_10++;
            // --------------------

            // Insertamos (preferentemente en hueco borrado si lo vimos)
            int pos_final = (pos_deleted != -1) ? pos_deleted : pos;

            if (tabla[pos_final].estado == DELETED && tabla[pos_final].medicamento) {
                delete tabla[pos_final].medicamento;
            }

            tabla[pos_final].medicamento = new PaMedicamento(pa);
            tabla[pos_final].estado = OCCUPIED;
            n_elementos++;
            return true;
        }
        intento++;
    }
    return false; // Tabla llena (caso raro si hay redispersión)
}

// --- BUSCAR ---
PaMedicamento* ThashMedicam::buscar(unsigned long clave) {
    int intento = 0;
    while (intento < T) {
        int pos = hash(clave, intento);

        if (tabla[pos].estado == OCCUPIED) {
            if (tabla[pos].medicamento->get_id_num() == clave) {
                return tabla[pos].medicamento;
            }
        } else if (tabla[pos].estado == EMPTY) {
            return nullptr; // No está y no puede estar más adelante
        }
        intento++;
    }
    return nullptr;
}

// --- BORRAR ---
bool ThashMedicam::borrar(unsigned long clave) {
    int intento = 0;
    while (intento < T) {
        int pos = hash(clave, intento);

        if (tabla[pos].estado == OCCUPIED) {
            if (tabla[pos].medicamento->get_id_num() == clave) {
                delete tabla[pos].medicamento;
                tabla[pos].medicamento = nullptr;
                tabla[pos].estado = DELETED;
                n_elementos--;
                return true;
            }
        } else if (tabla[pos].estado == EMPTY) {
            return false;
        }
        intento++;
    }
    return false;
}

// --- UTILIDADES ---
void ThashMedicam::mostrarEstadoTabla() {
    std::cout << "\n=== ESTADO INTERNO TABLA HASH ===" << std::endl;
    std::cout << " Tipo Hash: " << (current_hash_type == DOUBLE ? "Doble" : "Cuadratica") << std::endl;
    std::cout << " Tamano (T): " << T << " | Elementos: " << n_elementos << std::endl;
    std::cout << " Factor Carga: " << factorCarga() << " (Max: " << lambda_max << ")" << std::endl;
    std::cout << " Num Redispersiones: " << num_redispersiones << std::endl;
    std::cout << "---------------------------------" << std::endl;
    std::cout << " Max Colisiones: " << maxColisiones() << std::endl;
    std::cout << " Promedio Colisiones: " << promedioColisiones() << std::endl;
    std::cout << " Inserciones > 10 colisiones: " << numMax10() << std::endl;
    std::cout << "=================================\n" << std::endl;
}

std::vector<PaMedicamento*> ThashMedicam::getEntradasValidas() const {
    std::vector<PaMedicamento*> encontrados;
    encontrados.reserve(n_elementos);
    for (const auto& slot : tabla) {
        if (slot.estado == OCCUPIED && slot.medicamento != nullptr) {
            encontrados.push_back(slot.medicamento);
        }
    }
    return encontrados;
}