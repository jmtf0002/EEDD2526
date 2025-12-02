#include "ThashMedicam.h"
#include <iostream>
#include <cmath>

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
    while (primo > 2 && !es_primo(primo)) primo--;
    return primo;
}

ThashMedicam::ThashMedicam(int maxElementos, float lambda)
    : lambda_max(lambda), n_elementos(0), current_hash_type(DOUBLE_1),
      max_colisiones(0), total_colisiones(0), num_ops_insertar(0), num_max_10(0),
      num_redispersiones(0) {
    int tam_minimo = (int)std::ceil(maxElementos / lambda);
    T = siguiente_primo(tam_minimo);
    R_prime = anterior_primo(T);
    tabla.resize(T);
}

ThashMedicam::~ThashMedicam() {
    for (int i = 0; i < T; ++i) if (tabla[i].estado == OCCUPIED) delete tabla[i].medicamento;
}

ThashMedicam::ThashMedicam(const ThashMedicam &thash)
    : T(thash.T), n_elementos(thash.n_elementos), tabla(thash.T),
      lambda_max(thash.lambda_max), current_hash_type(thash.current_hash_type),
      R_prime(thash.R_prime),
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
        T = thash.T; n_elementos = thash.n_elementos; lambda_max = thash.lambda_max;
        current_hash_type = thash.current_hash_type; R_prime = thash.R_prime;
        max_colisiones = thash.max_colisiones; total_colisiones = thash.total_colisiones;
        num_ops_insertar = thash.num_ops_insertar; num_max_10 = thash.num_max_10;
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

// --- FUNCIÓN HASH ACTUALIZADA ---
int ThashMedicam::hash(unsigned long clave, int intento) const {
    unsigned long pos_final = 0;

    // 1. Cuadrática Estándar: h(k) + i^2
    if (current_hash_type == QUADRATIC) {
        pos_final = (h1(clave) + (unsigned long)intento * intento) % T;
    }
    // 2. Doble 1: h2 = 1 + (k % R)  <-- Esta es la "buena" del PDF
    else if (current_hash_type == DOUBLE_1) {
        int h2 = 1 + (clave % R_prime);
        pos_final = (h1(clave) + (unsigned long)intento * h2) % T;
    }
    // 3. Doble 2: h2 = R - (k % R)  <-- Variante común alternativa
    else if (current_hash_type == DOUBLE_2) {
        int h2 = R_prime - (clave % R_prime);
        pos_final = (h1(clave) + (unsigned long)intento * h2) % T;
    }

    return (int)pos_final;
}

void ThashMedicam::redispersar(unsigned int nuevo_tam) {
    std::cout << "  [ThashMedicam] Redispersando tabla... (T anterior: " << T << ", Nuevo T: " << nuevo_tam << ")" << std::endl;
    num_redispersiones++;
    std::vector<PaMedicamento*> antiguos = getEntradasValidas();
    tabla.clear();
    T = nuevo_tam;
    R_prime = anterior_primo(T);
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
        std::cout << "  [ThashMedicam] Nuevo lambda (" << l << ") menor que factor actual. Forzando redispersion." << std::endl;
        int nuevo_tam = siguiente_primo((int)(T * 1.30));
        redispersar(nuevo_tam);
    }
}

bool ThashMedicam::insertar(unsigned long clave, PaMedicamento &pa) {
    if (factorCarga() >= lambda_max) {
        int nuevo_tam = siguiente_primo((int)(T * 1.30));
        redispersar(nuevo_tam);
    }
    int intento = 0; int pos; int pos_deleted = -1;
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

void ThashMedicam::mostrarEstadoTabla() {
    std::string tipoStr = "Desconocido";
    if(current_hash_type == QUADRATIC) tipoStr = "Cuadratica";
    else if(current_hash_type == DOUBLE_1) tipoStr = "Doble 1";
    else if(current_hash_type == DOUBLE_2) tipoStr = "Doble 2";

    std::cout << "\n=== ESTADO INTERNO TABLA HASH ===" << std::endl;
    std::cout << " Tipo Hash: " << tipoStr << std::endl;
    std::cout << " Tamano (T): " << T << " (R_prime: " << R_prime << ")" << " | Elementos: " << n_elementos << std::endl;
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
        if (slot.estado == OCCUPIED && slot.medicamento != nullptr) encontrados.push_back(slot.medicamento);
    }
    return encontrados;
}