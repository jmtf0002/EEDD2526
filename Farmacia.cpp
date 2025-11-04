#include "Farmacia.h"
#include "MediExpress.h"  // Necesario para la definición completa de MediExpress
#include "PaMedicamento.h" // Necesario para get_id_num() y get_nombre()
#include <iostream>       // Para el std::cout

// --- IMPLEMENTACIÓN Constructores ---
Farmacia::Farmacia() : linkMedi(nullptr) {}

Farmacia::Farmacia(std::string cif, std::string provincia, std::string localidad,
                   std::string nombre, std::string direccion, std::string codPostal,
                   MediExpress* me)
    : cif(cif), provincia(provincia), localidad(localidad),
      nombre(nombre), direccion(direccion), codPostal(codPostal),
      linkMedi(me) {
}


PaMedicamento* Farmacia::buscaMedicam(int id_num) {
    for (unsigned i = 0; i < dispense.tamlog(); ++i) {
        if (dispense[i]->get_id_num() == id_num) {
            return dispense[i];
        }
    }
    return nullptr;
}

void Farmacia::dispensaMedicam(PaMedicamento* pa) {
    if (pa) {

        dispense.insertar(pa, dispense.tamlog());
    }
}

void Farmacia::pedidoMedicam(int id_num) {
    if (linkMedi == nullptr) {
        std::cout << "Error: Farmacia " << nombre
                  << " no esta conectada a MediExpress. Pedido cancelado." << std::endl;
        return;
    }
    linkMedi->suministrarFarmacia(*this, id_num);
}

VDinamico<PaMedicamento*> Farmacia::localizarMedicamentosPorNombre(const std::string& nombre) const {
    VDinamico<PaMedicamento*> encontrados;
    for (unsigned int i = 0; i < dispense.tamlog(); ++i) {
        if (dispense[i]->get_nombre().find(nombre) != std::string::npos) {
            encontrados.insertar(dispense[i], encontrados.tamlog());
        }
    }
    return encontrados;
}