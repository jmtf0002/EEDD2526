#include "Farmacia.h"
#include "MediExpress.h"
#include "PaMedicamento.h"
#include "Stock.h"
#include <iostream>
#include <string> // Necesario para stod

Farmacia::Farmacia() : linkMedi(nullptr), latitud(0.0), longitud(0.0) {}

// Constructor actualizado
Farmacia::Farmacia(std::string cif, std::string provincia, std::string localidad,
                   std::string nombre, std::string direccion, std::string codPostal,
                   std::string lat, std::string lon, MediExpress* me)
    : cif(cif), provincia(provincia), localidad(localidad),
      nombre(nombre), direccion(direccion), codPostal(codPostal),
      linkMedi(me) {

    // Convertimos las coordenadas de string a double
    try {
        this->latitud = std::stod(lat);
        this->longitud = std::stod(lon);
    } catch (...) {
        // Si hay error en el formato (ej: vacío), ponemos 0
        this->latitud = 0.0;
        this->longitud = 0.0;
    }
}

bool Farmacia::operator<(const Farmacia& other) const {
    return this->cif < other.cif;
}
bool Farmacia::operator>(const Farmacia& other) const {
    return this->cif > other.cif;
}
bool Farmacia::operator==(const Farmacia& other) const {
    return this->cif == other.cif;
}

int Farmacia::buscaMedicamID(int id_num) {
    auto it = order.find(id_num);
    if (it != order.end()) {
        return it->second.getNumStock();
    } else {
        return 0;
    }
}

int Farmacia::comprarMedicam(int id_num, int n, PaMedicamento*& result) {
    if (!linkMedi) {
        std::cerr << "Error: Farmacia " << nombre << " no conectada a MediExpress." << std::endl;
        result = nullptr;
        return 0;
    }

    int stock_actual = this->buscaMedicamID(id_num);

    result = linkMedi->buscarCompuesto(id_num);
    if (result == nullptr) {
        return 0; // Si no existe el medicamento, no hay stock
    }

    if (stock_actual >= n) {
        // Hay suficiente stock
        auto it = order.find(id_num);
        if (it != order.end()) {
            it->second.decrementa(n);
            return n; // Devolvemos lo que se ha comprado (n)
        }
    } else {
        // NO hay suficiente (o es 0).
        // El PDF dice: "Si no hay suficientes, al menos se llevará las que queden"
        // NO debemos hacer pedidoMedicam aquí, eso lo gestiona el main.
        if (stock_actual > 0) {
            auto it = order.find(id_num);
            if (it != order.end()) {
                it->second.decrementa(stock_actual); // Se lleva todo lo que queda
            }
            return stock_actual; // Devolvemos lo que se pudo comprar
        }
    }

    return 0; // No se pudo comprar nada
}

void Farmacia::pedidoMedicam(int id_num, int n) {
    if (linkMedi == nullptr) {
        std::cout << "Error: Farmacia " << nombre
                  << " no esta conectada a MediExpress. Pedido cancelado." << std::endl;
        return;
    }
    // Llama a MediExpress para suministrar (aumentar stock)
    linkMedi->suministrarFarmacia(*this, id_num, n);
}

void Farmacia::nuevoStock(PaMedicamento* pa, int n) {
    if (pa == nullptr) return;

    int id = pa->get_id_num();
    auto it = order.find(id);

    if (it != order.end()) {
        it->second.incrementa(n);
    } else {
        order.insert(std::make_pair(id, Stock(id, n)));
    }
}

bool Farmacia::eliminarStock(int id_num) {
    return order.erase(id_num) > 0;
}

// Busca medicamentos por nombre DENTRO del stock de la farmacia
std::vector<PaMedicamento*> Farmacia::buscaMedicamNombre(const std::string& nom) const {
    std::vector<PaMedicamento*> encontrados;
    if (!linkMedi) {
        return encontrados;
    }

    for (const auto& par : order) {
        // Obtenemos el objeto medicamento a partir del ID guardado en stock
        PaMedicamento* med = linkMedi->buscarCompuesto(par.first);

        // Si existe y contiene el nombre buscado
        if (med && med->get_nombre().find(nom) != std::string::npos) {
            encontrados.push_back(med);
        }
    }
    return encontrados;
}

int Farmacia::consultarStock(int id_num) const {
    auto it = order.find(id_num);
    if (it != order.end()) {
        return it->second.getNumStock();
    }
    return 0;
}