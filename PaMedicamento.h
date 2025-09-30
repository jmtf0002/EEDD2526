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




public:

    // Constructores
    PaMedicamento() {
        this->id_num = 0;
        this->id_alpha = "";
        this->nombre = "";
    }

    PaMedicamento(int id_num, const std::string &id_alpha, const std::string &nombre) {
        this->id_num = id_num;
        this->id_alpha = id_alpha;
        this->nombre = nombre;
    }

    // Getters y Setters
    int get_id_num() {
        return id_num;
    }

    void set_id_num(int id_num) {
        this->id_num = id_num;
    }

    std::string get_id_alpha() {
        return id_alpha;
    }

    void set_id_alpha(const std::string &id_alpha) {
        this->id_alpha = id_alpha;
    }

    std::string get_nombre() {
        return nombre;
    }

    void set_nombre(const std::string &nombre) {
        this->nombre = nombre;
    }

    // Comparadores
    bool operator<(PaMedicamento &med) {
        return id_num < med.get_id_num();
    }

    bool operator==(PaMedicamento &med) {
        return id_num == med.get_id_num();
    }

};

#endif // PAMEDICAMENTO_H
