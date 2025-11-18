#include "Farmacia.h"
#include "MediExpress.h"
#include "PaMedicamento.h"
#include "Stock.h"
#include <iostream>

Farmacia::Farmacia() : linkMedi(nullptr) {}

Farmacia::Farmacia(std::string cif, std::string provincia, std::string localidad,
                   std::string nombre, std::string direccion, std::string codPostal,
                   MediExpress* me)
    : cif(cif), provincia(provincia), localidad(localidad),
      nombre(nombre), direccion(direccion), codPostal(codPostal),
      linkMedi(me) {
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
    Stock dummy_para_buscar(id_num);

    auto it = order.find(dummy_para_buscar);

    if (it != order.end()) {
        return it->getNumStock();
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

    int stock_inicial = this->buscaMedicamID(id_num);
    result = linkMedi->buscarCompuesto(id_num);

    if (result == nullptr) {
        return stock_inicial;
    }

    if (stock_inicial >= n) {
        Stock dummy(id_num);
        auto it = order.find(dummy);

        if (it != order.end()) {
            Stock stock_actualizado = *it;
            order.erase(it);
            stock_actualizado.decrementa(n);
            order.insert(stock_actualizado);
        }
    } else {
        this->pedidoMedicam(id_num, n);
    }

    return stock_inicial;
}


void Farmacia::pedidoMedicam(int id_num, int n) {
    if (linkMedi == nullptr) {
        std::cout << "Error: Farmacia " << nombre
                  << " no esta conectada a MediExpress. Pedido cancelado." << std::endl;
        return;
    }
    linkMedi->suministrarFarmacia(*this, id_num, n);
}


void Farmacia::nuevoStock(PaMedicamento* pa, int n) {
    if (pa == nullptr) return;

    Stock dummy(pa->get_id_num());
    auto it = order.find(dummy);

    if (it != order.end()) {
        Stock stock_actualizado = *it;
        order.erase(it);
        stock_actualizado.incrementa(n);
        order.insert(stock_actualizado);
    } else {
        Stock nuevo_stock(pa->get_id_num(), n);
        order.insert(nuevo_stock);
    }
}


bool Farmacia::eliminarStock(int id_num) {
    Stock dummy(id_num);
    auto it = order.find(dummy);

    if (it != order.end()) {
        order.erase(it);
        return true;
    }
    return false;
}


std::vector<PaMedicamento*> Farmacia::buscaMedicamNombre(const std::string& nom) const {
    std::vector<PaMedicamento*> encontrados;
    if (!linkMedi) {
        return encontrados;
    }

    for (const Stock& s : order) {
        PaMedicamento* med = linkMedi->buscarCompuesto(s.getIdPaMed());

        if (med && med->get_nombre().find(nom) != std::string::npos) {
            encontrados.push_back(med);
        }
    }
    return encontrados;
}

int Farmacia::consultarStock(int id_num) const {
    Stock dummy(id_num);
    auto it = order.find(dummy);

    if (it != order.end()) {
        return it->getNumStock();
    }
    return 0;
}
