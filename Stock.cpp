#include "Stock.h"

// Constructor para búsquedas (objeto "dummy")
Stock::Stock(int id) : id_PaMed(id), num_stock(0) {
    // Este constructor se usa para crear un objeto temporal para buscar en el set.
}

// Constructor principal
Stock::Stock(int id, int stock) : id_PaMed(id), num_stock(stock) {
}

// Getters
int Stock::getIdPaMed() const {
    return id_PaMed;
}

int Stock::getNumStock() const {
    return num_stock;
}

// Métodos para modificar el stock
void Stock::incrementa(int n) {
    this->num_stock += n;
}

void Stock::decrementa(int n) {

    this->num_stock -= n;
}

// Operador de comparación para el std::set 
bool Stock::operator<(const Stock& other) const {
    // El orden en el set se basa únicamente en el ID del medicamento.
    return this->id_PaMed < other.id_PaMed;
}