#include "PaMedicamento.h"
#include <stdexcept>
#include <cstdlib>
#include <limits>

// Implementación de PaMedicamento

PaMedicamento::PaMedicamento()
    : id_num(0), id_alpha(), nombre() {}

PaMedicamento::PaMedicamento(int idNum, const std::string& idAlpha, const std::string& nombre)
    : id_num(idNum), id_alpha(idAlpha), nombre(nombre) {}

PaMedicamento::PaMedicamento(const std::string& idNumStr, const std::string& idAlpha, const std::string& nombre)
    : id_num(0), id_alpha(idAlpha), nombre(nombre) {
    setIdNum(idNumStr); // puede lanzar std::invalid_argument
}

int PaMedicamento::getIdNum() const noexcept { return id_num; }
const std::string& PaMedicamento::getIdAlpha() const noexcept { return id_alpha; }
const std::string& PaMedicamento::getNombre() const noexcept { return nombre; }

void PaMedicamento::setIdNum(int id) noexcept { id_num = id; }

void PaMedicamento::setIdNum(const std::string& idStr) {
    try {
        size_t pos = 0;
        long v = std::stol(idStr, &pos);
        if (pos != idStr.size()) throw std::invalid_argument("id_num con formato incorrecto");
        if (v < static_cast<long>(std::numeric_limits<int>::min()) || v > static_cast<long>(std::numeric_limits<int>::max()))
            throw std::out_of_range("id_num fuera de rango int");
        id_num = static_cast<int>(v);
    } catch (const std::invalid_argument& e) {
        throw std::invalid_argument(std::string("id_num no es un entero valido: ") + e.what());
    } catch (const std::out_of_range& e) {
        throw;
    }
}

void PaMedicamento::setIdAlpha(const std::string& s) { id_alpha = s; }
void PaMedicamento::setNombre(const std::string& s) { nombre = s; }

bool PaMedicamento::operator<(const PaMedicamento& other) const noexcept {
    return id_num < other.id_num;
}

bool PaMedicamento::operator==(const PaMedicamento& other) const noexcept {
    return id_num == other.id_num && id_alpha == other.id_alpha && nombre == other.nombre;
}

std::ostream& operator<<(std::ostream& os, const PaMedicamento& p) {
    os << "Id_num=" << p.id_num << " id_alpha='" << p.id_alpha << "' Nombre='" << p.nombre << "'";
    return os;
}
