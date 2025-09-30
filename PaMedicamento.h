#ifndef PAMEDICAMENTO_H
#define PAMEDICAMENTO_H

#include <string>
#include <ostream>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <algorithm>

// Clase PaMedicamento
// - Atributos: id_num (int), id_alpha (string), nombre (string)
// - Constructores, getters/setters, comparadores y salida por stream.
// - setIdNum(const std::string&) lanza std::invalid_argument si la cadena no es un entero válido.

class PaMedicamento {
private:
    int id_num;
    std::string id_alpha;
    std::string nombre;

    // helper: trim both ends
    static inline std::string trim(const std::string& s) {
        size_t i = 0, j = s.size();
        while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) --j;
        return s.substr(i, j - i);
    }

public:
    // constructores
    PaMedicamento() noexcept : id_num(0), id_alpha(), nombre() {}
    PaMedicamento(int idNum, const std::string& idAlpha, const std::string& nombre) noexcept
        : id_num(idNum), id_alpha(idAlpha), nombre(nombre) {}
    // construct from string id (throws std::invalid_argument on bad format)
    PaMedicamento(const std::string& idNumStr, const std::string& idAlpha, const std::string& nombre)
        : id_num(0), id_alpha(idAlpha), nombre(nombre) {
        setIdNum(idNumStr);
    }

    PaMedicamento(const PaMedicamento& other) = default;
    PaMedicamento& operator=(const PaMedicamento& other) = default;

    // getters
    int getIdNum() const noexcept { return id_num; }
    const std::string& getIdAlpha() const noexcept { return id_alpha; }
    const std::string& getNombre() const noexcept { return nombre; }

    // setters
    void setIdNum(int id) noexcept { id_num = id; }

    // parse string; lanzará std::invalid_argument si la cadena no es un entero válido
    void setIdNum(const std::string& idStr) {
        std::string s = trim(idStr);
        if (s.empty()) throw std::invalid_argument("id vacío");
        try {
            size_t idx = 0;
            long val = std::stol(s, &idx);
            if (idx != s.size()) throw std::invalid_argument("id contiene caracteres no numéricos");
            // opcional: comprobar rangos si es necesario
            id_num = static_cast<int>(val);
        } catch (const std::invalid_argument&) {
            throw; // re-lanzar tal cual
        } catch (const std::out_of_range& e) {
            throw std::invalid_argument(std::string("id fuera de rango: ") + e.what());
        } catch (const std::exception& e) {
            throw std::invalid_argument(std::string("error parseando id: ") + e.what());
        }
    }

    void setIdAlpha(const std::string& s) { id_alpha = s; }
    void setNombre(const std::string& s) { nombre = s; }

    // comparadores
    bool operator<(const PaMedicamento& other) const noexcept { return id_num < other.id_num; }
    bool operator==(const PaMedicamento& other) const noexcept {
        return id_num == other.id_num && id_alpha == other.id_alpha && nombre == other.nombre;
    }

    // salida por stream
    friend std::ostream& operator<<(std::ostream& os, const PaMedicamento& p) {
        os << p.id_num << ';' << p.id_alpha << ';' << p.nombre;
        return os;
    }
};

#endif // PAMEDICAMENTO_H
