#include "Farmacia.h"
#include "MediExpress.h"  // Necesario para llamar a MediExpress
#include "PaMedicamento.h" // Necesario para get_id_num() y get_nombre()
#include "Stock.h"        // Necesario para el std::set<Stock>
#include <iostream>       // Para std::cout

Farmacia::Farmacia() : linkMedi(nullptr) {}

Farmacia::Farmacia(std::string cif, std::string provincia, std::string localidad,
                   std::string nombre, std::string direccion, std::string codPostal,
                   MediExpress* me)
    : cif(cif), provincia(provincia), localidad(localidad),
      nombre(nombre), direccion(direccion), codPostal(codPostal),
      linkMedi(me) {
    // 'order' (el std::set) se inicializa vacío automáticamente
}

// --- Operadores (sin cambios) ---
bool Farmacia::operator<(const Farmacia& other) const {
    return this->cif < other.cif;
}
bool Farmacia::operator>(const Farmacia& other) const {
    return this->cif > other.cif;
}
bool Farmacia::operator==(const Farmacia& other) const {
    return this->cif == other.cif;
}

// --- MÉTODOS DE LA PRÁCTICA 4 ---

/**
 * @brief Busca el stock de un medicamento por ID.
 * Versión privada que devuelve el stock (0 si no existe)[cite: 22, 23].
 */
int Farmacia::buscaMedicamID(int id_num) {
    // Creamos un objeto Stock "dummy" solo con el ID para buscar [cite: 77]
    Stock dummy_para_buscar(id_num);

    auto it = order.find(dummy_para_buscar);

    if (it != order.end()) {
        return it->getNumStock(); // Devuelve el stock si lo encuentra [cite: 23]
    } else {
        return 0; // Devuelve 0 si no existe [cite: 23]
    }
}

/**
 * @brief Simula la compra de un medicamento[cite: 24].
 */
int Farmacia::comprarMedicam(int id_num, int n, PaMedicamento*& result) {
    if (!linkMedi) {
        std::cerr << "Error: Farmacia " << nombre << " no conectada a MediExpress." << std::endl;
        result = nullptr;
        return 0;
    }

    // 1. Obtener el stock inicial y el medicamento
    int stock_inicial = this->buscaMedicamID(id_num);
    result = linkMedi->buscarCompuesto(id_num); // Obtenemos el medicamento para devolverlo [cite: 30]

    if (result == nullptr) {
        // MediExpress no conoce este medicamento, no se puede hacer nada.
        return stock_inicial;
    }

    // 2. Comprobar si hay stock suficiente
    if (stock_inicial >= n) {
        // Sí hay stock: decrementar [cite: 28]
        Stock dummy(id_num);
        auto it = order.find(dummy);

        if (it != order.end()) {
            // ¡OJO! No se puede modificar un elemento de std::set in-situ.
            // Hay que extraerlo, modificarlo y volverlo a insertar.
            Stock stock_actualizado = *it; // Copiamos
            order.erase(it);               // Borramos el viejo
            stock_actualizado.decrementa(n); // Modificamos la copia
            order.insert(stock_actualizado); // Insertamos el actualizado
        }
    } else {
        // No hay stock: pedir a MediExpress [cite: 29]
        // El PDF dice que se pide el 'n' (número de stock que quiere incrementar) [cite: 29]
        this->pedidoMedicam(id_num, n);
    }

    // 3. Devolver el stock *inicial* [cite: 30]
    return stock_inicial;
}

/**
 * @brief Llama a MediExpress para pedir más unidades[cite: 31].
 */
void Farmacia::pedidoMedicam(int id_num, int n) {
    if (linkMedi == nullptr) {
        std::cout << "Error: Farmacia " << nombre
                  << " no esta conectada a MediExpress. Pedido cancelado." << std::endl;
        return;
    }
    // Llama a la nueva versión de suministrarFarmacia (que debemos crear en MediExpress)
    linkMedi->suministrarFarmacia(*this, id_num, n);
}

/**
 * @brief Añade nuevo stock o incrementa el existente[cite: 33].
 */
void Farmacia::nuevoStock(PaMedicamento* pa, int n) {
    if (pa == nullptr) return;

    Stock dummy(pa->get_id_num());
    auto it = order.find(dummy);

    if (it != order.end()) {
        // Ya existe, incrementar [cite: 33]
        Stock stock_actualizado = *it;
        order.erase(it);
        stock_actualizado.incrementa(n);
        order.insert(stock_actualizado);
    } else {
        // No existe, crear nuevo [cite: 33]
        Stock nuevo_stock(pa->get_id_num(), n);
        order.insert(nuevo_stock);
    }
}

/**
 * @brief Elimina un Stock por ID[cite: 34].
 */
bool Farmacia::eliminarStock(int id_num) {
    Stock dummy(id_num);
    auto it = order.find(dummy);

    if (it != order.end()) {
        order.erase(it);
        return true;
    }
    return false; // No se encontró
}

/**
 * @brief Busca medicamentos por nombre parcial[cite: 35].
 * Como Farmacia ya no almacena los PaMedicamento,
 * debe pedírselos a MediExpress.
 */
std::vector<PaMedicamento*> Farmacia::buscaMedicamNombre(const std::string& nom) const {
    std::vector<PaMedicamento*> encontrados;
    if (!linkMedi) {
        return encontrados;
    }

    // Iteramos sobre nuestro set de 'Stock'
    for (const Stock& s : order) {
        // Pedimos a MediExpress el PaMedicamento completo usando el ID
        PaMedicamento* med = linkMedi->buscarCompuesto(s.getIdPaMed());

        // Si existe y el nombre coincide, lo añadimos
        if (med && med->get_nombre().find(nom) != std::string::npos) {
            encontrados.push_back(med);
        }
    }
    return encontrados;
}

// Los métodos antiguos (buscaMedicam, dispensaMedicam, localizarMedicamentosPorNombre)
// han sido reemplazados por los nuevos (buscaMedicamID, nuevoStock, buscaMedicamNombre).