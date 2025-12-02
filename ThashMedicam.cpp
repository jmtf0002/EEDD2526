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
    while (!es_primo(primo)) primo++;
    return primo;
}

int ThashMedicam::anterior_primo(int n) const {
    int primo = n - 1;
    while (primo >= 2) {
        if (es_primo(primo)) return primo;
        primo--;
    }
    return 2;
}

// --- HASH ---
int ThashMedicam::h2_v1(unsigned long clave) const {
    static int R = -1;
    static int cached_T = -1;
    if (R == -1 || cached_T != T) {
        R = anterior_primo(T);
        cached_T = T;
    }
    return 1 + (clave % R);
}

int ThashMedicam::h2_v2(unsigned long clave) const {
    static int R2 = -1;
    static int cached_T2 = -1;
    if (R2 == -1 || cached_T2 != T) {
        int R1 = anterior_primo(T);
        R2 = anterior_primo(R1);
        cached_T2 = T;
    }
    return 1 + (clave % R2);
}

int ThashMedicam::hash(unsigned long clave, int intento) const {
    unsigned long pos_final = 0;
    if (current_hash_type == QUADRATIC) {
        pos_final = (h1(clave) + (unsigned long)intento * intento) % T;
    } else if (current_hash_type == DOUBLE_1) {
        pos_final = (h1(clave) + (unsigned long)intento * h2_v1(clave)) % T;
    } else {
        pos_final = (h1(clave) + (unsigned long)intento * h2_v2(clave)) % T;
    }
    return (int)pos_final;
}

// --- CONSTRUCTOR ---
ThashMedicam::ThashMedicam(int maxElementos, float lambda)
    : lambda_max(lambda), n_elementos(0), current_hash_type(DOUBLE_1),
      max_colisiones(0), total_colisiones(0), num_ops_insertar(0), num_max_10(0),
      num_redispersiones(0) {
    int tam_minimo = (int)std::ceil(maxElementos / lambda);
    T = siguiente_primo(tam_minimo);
    tabla.resize(T);
}

ThashMedicam::~ThashMedicam() {
    for (int i = 0; i < T; ++i) if (tabla[i].estado == OCCUPIED) delete tabla[i].medicamento;
}

// --- COPIA Y ASIGNACION ---
ThashMedicam::ThashMedicam(const ThashMedicam &thash)
    : T(thash.T), n_elementos(thash.n_elementos), tabla(thash.T),
      lambda_max(thash.lambda_max), current_hash_type(thash.current_hash_type),
      max_colisiones(thash.max_colisiones), total_colisiones(thash.total_colisiones),
      num_ops_insertar(thash.num_ops_insertar), num_max_10(thash.num_max_10),
      num_redispersiones(thash.num_redispersiones) {
    for (int i = 0; i < T; ++i) {
        tabla[i].estado = thash.tabla[i].estado;
        if (tabla[i].estado == OCCUPIED) tabla[i].medicamento = new PaMedicamento(*thash.tabla[i].medicamento);
        else tabla[i].medicamento = nullptr;
    }
}

ThashMedicam& ThashMedicam::operator=(const ThashMedicam &thash) {
    if (this != &thash) {
        for (int i = 0; i < T; ++i) if (tabla[i].estado == OCCUPIED) delete tabla[i].medicamento;
        T = thash.T;
        n_elementos = thash.n_elementos;
        lambda_max = thash.lambda_max;
        current_hash_type = thash.current_hash_type;
        max_colisiones = thash.max_colisiones;
        total_colisiones = thash.total_colisiones;
        num_ops_insertar = thash.num_ops_insertar;
        num_max_10 = thash.num_max_10;
        num_redispersiones = thash.num_redispersiones;
        tabla.resize(T);
        for (int i = 0; i < T; ++i) {
            tabla[i].estado = thash.tabla[i].estado;
            if (tabla[i].estado == OCCUPIED) tabla[i].medicamento = new PaMedicamento(*thash.tabla[i].medicamento);
            else { tabla[i].medicamento = nullptr; tabla[i].estado = thash.tabla[i].estado; }
        }
    }
    return *this;
}

// --- REDISPERSIÓN ---
void ThashMedicam::redispersar(unsigned int nuevo_tam) {
    num_redispersiones++;
    std::vector<PaMedicamento*> antiguos = getEntradasValidas();
    tabla.clear();
    T = nuevo_tam;
    tabla.resize(T);
    n_elementos = 0;
    for (PaMedicamento* p : antiguos) {
        insertar(p->get_id_num(), *p);
        delete p;
    }
}

void ThashMedicam::setLambda(float l) {
    lambda_max = l;
    if (factorCarga() > lambda_max) {
        int nuevo_tam = siguiente_primo((int)(T * 1.30));
        redispersar(nuevo_tam);
    }
}

// --- INSERTAR ---
bool ThashMedicam::insertar(unsigned long clave, PaMedicamento &pa) {
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
            if (tabla[pos].medicamento->get_id_num() == clave) return false;
        } else if (tabla[pos].estado == DELETED) {
            if (pos_deleted == -1) pos_deleted = pos;
        } else {
            if (intento > max_colisiones) max_colisiones = intento;
            total_colisiones += intento;
            if (intento > 10) num_max_10++;

            int pos_final = (pos_deleted != -1) ? pos_deleted : pos;
            if (tabla[pos_final].estado == DELETED && tabla[pos_final].medicamento) delete tabla[pos_final].medicamento;

            tabla[pos_final].medicamento = new PaMedicamento(pa);
            tabla[pos_final].estado = OCCUPIED;
            n_elementos++;
            return true;
        }
        intento++;
    }
    return false;
}

PaMedicamento* ThashMedicam::buscar(unsigned long clave) {
    int intento = 0;
    while (intento < T) {
        int pos = hash(clave, intento);
        if (tabla[pos].estado == OCCUPIED) {
            if (tabla[pos].medicamento->get_id_num() == clave) return tabla[pos].medicamento;
        } else if (tabla[pos].estado == EMPTY) return nullptr;
        intento++;
    }
    return nullptr;
}

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
        } else if (tabla[pos].estado == EMPTY) return false;
        intento++;
    }
    return false;
}

// --- MOSTRAR ESTADO (Formato Ejercicio 5) ---
void ThashMedicam::mostrarEstadoTabla() {
    std::cout << "Tamano de la tabla: " << T << std::endl;
    std::cout << "Numero de medicamentos: " << n_elementos << std::endl;
    std::cout << "Maximo colisiones: " << max_colisiones << std::endl;
    std::cout << "Factor de carga: " << factorCarga() << std::endl;
    std::cout << "Promedio de colisiones: " << promedioColisiones() << std::endl;
    std::cout << "Mas de 10 colisiones: " << num_max_10 << std::endl;
    std::cout << "Total colisiones: " << total_colisiones << std::endl;
    std::cout << "Redispersiones: " << num_redispersiones << std::endl;
}

std::vector<PaMedicamento*> ThashMedicam::getEntradasValidas() const {
    std::vector<PaMedicamento*> encontrados;
    for (const auto& slot : tabla) {
        if (slot.estado == OCCUPIED && slot.medicamento != nullptr) encontrados.push_back(slot.medicamento);
    }
    return encontrados;
}