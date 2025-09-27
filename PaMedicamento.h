#ifndef PAMEDICAMENTO_H
#define PAMEDICAMENTO_H

#include <string>
#include <ostream>

// Clase PaMedicamento
// - Atributos: id_num (int), id_alpha (string), nombre (string)
// - Constructores, operadores, getters/setters necesarios para la práctica
// - Ordenación por id_num (operator<)

class PaMedicamento {
private:
    int id_num;
    std::string id_alpha;
    std::string nombre;

public:
    // constructores
    PaMedicamento();
    PaMedicamento(int idNum, const std::string& idAlpha, const std::string& nombre);
    // construct from string id (throws std::invalid_argument on bad format)
    PaMedicamento(const std::string& idNumStr, const std::string& idAlpha, const std::string& nombre);
    PaMedicamento(const PaMedicamento& other) = default;
    PaMedicamento& operator=(const PaMedicamento& other) = default;

    // getters
    int getIdNum() const noexcept;
    const std::string& getIdAlpha() const noexcept;
    const std::string& getNombre() const noexcept;

    // setters
    void setIdNum(int id) noexcept;
    // parse string; lanzará std::invalid_argument si la cadena no es un entero válido
    void setIdNum(const std::string& idStr);
    void setIdAlpha(const std::string& s);
    void setNombre(const std::string& s);

    // comparadores
    bool operator<(const PaMedicamento& other) const noexcept; // orden por id_num
    bool operator==(const PaMedicamento& other) const noexcept;

    // salida por stream (útil para mostrar datos)
    friend std::ostream& operator<<(std::ostream& os, const PaMedicamento& p);
};

#endif // PAMEDICAMENTO_H
