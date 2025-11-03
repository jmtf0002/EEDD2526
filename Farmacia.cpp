#include "Farmacia.h"
#include "MediExpress.h"  // Necesario para la definición completa de MediExpress
#include "PaMedicamento.h" // Necesario para get_id_num() y get_nombre()

// --- Constructor por defecto ---
// (Si es simple, se puede quedar en el .h, pero por coherencia lo pongo aquí)
Farmacia::Farmacia() : linkMedi(nullptr) {}

// --- Constructor completo (7 argumentos) ---
Farmacia::Farmacia(std::string cif, std::string provincia, std::string localidad,
                   std::string nombre, std::string direccion, std::string codPostal,
                   MediExpress* me)
    : cif(cif), provincia(provincia), localidad(localidad),
      nombre(nombre), direccion(direccion), codPostal(codPostal),
      linkMedi(me) {
    // 'stock' (VDinamico) se inicializa vacío automáticamente
}

/**
 * @brief Busca un medicamento en el stock local por ID.
 */
PaMedicamento* Farmacia::buscaMedicam(int id_num) {
    for (unsigned i = 0; i < stock.tamlog(); ++i) {
        if (stock[i]->get_id_num() == id_num) {
            return stock[i];
        }
    }
    return nullptr;
}

/**
 * @brief Añade un medicamento al stock. (Llamado por MediExpress)
 */
void Farmacia::dispensaMedicam(PaMedicamento* pa) {
    if (pa) {

        stock.insertar(pa, stock.tamlog());
    }
}

/**
 * @brief Pide un medicamento a MediExpress.
 */
void Farmacia::pedidoMedicam(int id_num) {
    if (linkMedi == nullptr) {
        std::cerr << "ERROR: Farmacia " << nombre
                  << " no esta conectada a MediExpress. Pedido cancelado." << std::endl;
        return;
    }


    linkMedi->suministrarFarmacia(*this, id_num);
}

/**
 * @brief Localiza medicamentos en el stock local por nombre (parcial).
 * (Este era el método que faltaba para la "Prueba de Parejas")
 */
VDinamico<PaMedicamento*> Farmacia::localizarMedicamentosPorNombre(const std::string& nombre) const {
    VDinamico<PaMedicamento*> encontrados;
    for (unsigned int i = 0; i < stock.tamlog(); ++i) {
        // Asume que PaMedicamento tiene get_nombre()
        if (stock[i]->get_nombre().find(nombre) != std::string::npos) {
            encontrados.insertar(stock[i], encontrados.tamlog());
        }
    }
    return encontrados;
}