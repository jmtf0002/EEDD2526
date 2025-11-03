#include "Farmacia.h"
#include "MediExpress.h"  // <-- Necesario para llamar a suministrarFarmacia
#include <iostream>       // Para el std::cout

/**
 * @brief Realiza un pedido de un medicamento a MediExpress.
 * Este método utiliza el puntero 'linkMedi' para llamar al método
 * 'suministrarFarmacia' de la central de MediExpress.
 * @param id_num El ID numérico del medicamento a pedir.
 */
void Farmacia::pedidoMedicam(int id_num) {
    
    // 1. Comprobación de seguridad (Guard Clause)
    // Nos aseguramos de que la farmacia esté conectada a la central.
    if (linkMedi == nullptr) {
        std::cout << "ERROR: Farmacia " << nombre 
                  << " no esta conectada a MediExpress. Pedido cancelado." << std::endl;
        return;
    }

    // 2. Mensaje de log (opcional, pero útil)
  //  std::cout << "Farmacia " << nombre << ": Realizando pedido a MediExpress del med ID: "
  //            << id_num << "." << std::endl;

    // 3. La llamada a MediExpress
    // Se pasa "*this" para que MediExpress sepa a QUÉ farmacia
    // debe entregarle el medicamento.
    linkMedi->suministrarFarmacia(*this, id_num);
}

/**
 * @brief Localiza medicamentos en el stock local por nombre (parcial).
 */
VDinamico<PaMedicamento*> Farmacia::localizarMedicamentosPorNombre(const std::string& nombre) const {
    VDinamico<PaMedicamento*> encontrados;
    for (unsigned int i = 0; i < stock.tamlog(); ++i) {
        // Asume que PaMedicamento tiene get_nombre()
        // std::string::npos significa "no encontrado"
        if (stock[i]->get_nombre().find(nombre) != std::string::npos) {
            encontrados.insertar(stock[i], encontrados.tamlog());
        }
    }
    return encontrados;
}