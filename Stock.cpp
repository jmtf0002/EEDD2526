#include "Stock.h"

Stock::Stock(int id) : id_PaMed(id), num_stock(0) {
}

Stock::Stock(int id, int stock) : id_PaMed(id), num_stock(stock) {
}

int Stock::getIdPaMed() const {
    return id_PaMed;
}

int Stock::getNumStock() const {
    return num_stock;
}

void Stock::incrementa(int n) {
    this->num_stock += n;
}

void Stock::decrementa(int n) {

    this->num_stock -= n;
}

bool Stock::operator<(const Stock& other) const {
    return this->id_PaMed < other.id_PaMed;
}