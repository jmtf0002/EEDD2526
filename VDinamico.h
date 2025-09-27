#ifndef VDINAMICO_H
#define VDINAMICO_H

#include <algorithm>
#include <stdexcept>
#include <climits>
#include <cstddef>

// VDinamico<T> - Interfaz
// - Implementa un vector dinámico que mantiene el tamaño físico como potencia de 2.
// - Proporciona: constructor por defecto, constructor con tamaño lógico, copia total,
//   copia parcial (desde posición y con número de elementos), operador=, operadores [],
//   insertar, borrar, ordenar, tamlog() y destructor.
// - Lanza std::out_of_range en accesos/operaciones fuera de rango.

template<typename T>
class VDinamico {
public:
    // constructores/destructor
    VDinamico();                             // físico = 1, lógico = 0
    explicit VDinamico(unsigned int tamlog); // inicializa tamlog
    VDinamico(const VDinamico<T>& origen);   // copia total
    VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos); // copia parcial
    ~VDinamico();

    // operador de asignación
    VDinamico<T>& operator=(const VDinamico<T>& otro);

    // operadores de acceso
    T& operator[](unsigned int pos);
    const T& operator[](unsigned int pos) const;

    // insertar (por defecto al final)
    void insertar(const T& dato, unsigned int pos = UINT_MAX);

    // borrar (por defecto último). Devuelve el elemento borrado.
    T borrar(unsigned int pos = UINT_MAX);

    // ordenar (requiere operator< en T)
    void ordenar();

    // tamaño lógico
    unsigned int tamlog() const;

private:
    T* datos;
    unsigned int m_tamlog; // tamaño lógico
    unsigned int m_tamfis; // tamaño físico (potencia de 2)

    static unsigned int siguientePotencia2(unsigned int n);
    void reservar(unsigned int nuevaFis);
};

// Implementación (definiciones de plantilla). Separamos en un fichero .cpp que debe
// ser incluido al final de este encabezado para que las definiciones estén disponibles
// en tiempo de compilación.

#include "VDinamico.cpp"

#endif // VDINAMICO_H
