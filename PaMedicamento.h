#ifndef PAMEDICAMENTO_H
#define PAMEDICAMENTO_H

#include <string>
#include <ostream>
#include "Laboratorio.h"

class PaMedicamento {
private:
    int id_num;
    std::string id_alpha;
    std::string nombre;
    Laboratorio *laboratorioAsignado;

public:
    PaMedicamento() : id_num(0), id_alpha(""), nombre(""), laboratorioAsignado(nullptr) {}

    PaMedicamento(int id_num, const std::string &id_alpha, const std::string &nombre)
        : id_num(id_num), id_alpha(id_alpha), nombre(nombre), laboratorioAsignado(nullptr) {}

    int get_id_num() const {
        return id_num;
    }
    void set_id_num(int id_num) {
        this->id_num = id_num;
    }

    std::string get_id_alpha() const {
        return id_alpha;
    }
    void set_id_alpha(const std::string &id_alpha) {
        this->id_alpha = id_alpha;
    }

    std::string get_nombre() const {
        return nombre;
    }
    void set_nombre(const std::string &nombre) {
        this->nombre = nombre;
    }

    void servidoPor(Laboratorio *lab) {
        this->laboratorioAsignado = lab;
    }
    Laboratorio* getLaboratorio() const {
        return laboratorioAsignado;
    }

    bool operator<(const PaMedicamento &med) const {
        return id_num < med.get_id_num();
    }

    bool operator==(const PaMedicamento &med) const {
        return id_num == med.get_id_num();
    }
};

#endif // PAMEDICAMENTO_H