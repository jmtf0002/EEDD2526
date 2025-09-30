// VDinamico.h  (versión corregida)
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
//   insertar, borrar, ordenar, tamLog() y destructor.
// - Lanza std::out_of_range en accesos/operaciones fuera de rango.

template<typename T>
class VDinamico {
private:
  unsigned int tamal, tamaf;
  T *v;
public:
  VDinamico();
  VDinamico(unsigned int tamlog);
  VDinamico(const VDinamico<T>& origen);
  VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos);
  VDinamico<T>& operator=(const VDinamico<T>& otro);

  T& operator[](unsigned pos);
  const T& operator[](unsigned pos) const;

  void insertar(const T& dato, unsigned int pos = UINT_MAX);
  T borrar (unsigned int pos = UINT_MAX);
  void ordenar();
  unsigned int tamLog() const { return tamal; }

  ~VDinamico();
};

// Implementación (definiciones de plantilla). Separamos en un fichero .cpp que debe
// ser incluido al final de este encabezado para que las definiciones estén disponibles
// en tiempo de compilación.


#endif // VDINAMICO_H


// -----------------------------------------------------------------------------
// VDinamico.cpp
// Implementación de las funciones plantilla.
// Este fichero está pensado para ser incluido por VDinamico.h al final del header.
// -----------------------------------------------------------------------------

#include <cmath> // pow (no necesario, pero por claridad)

// Utilidad: siguiente potencia de 2 mayor o igual que n (mínimo 1)
static unsigned int siguiente_pot2(unsigned int n) {
    unsigned int p = 1;
    while (p < n) p <<= 1;
    return p;
}

// Constructor por defecto
template<typename T>
VDinamico<T>::VDinamico() : tamal(0), tamaf(1), v(new T[1]) {}

// Constructor con tamaño lógico
template<typename T>
VDinamico<T>::VDinamico(unsigned int tamlog) : tamal(tamlog) {
    tamaf = siguiente_pot2(tamlog == 0 ? 1u : tamlog);
    v = new T[tamaf];
}

// Constructor copia (completo)
template<typename T>
VDinamico<T>::VDinamico(const VDinamico<T>& origen) : tamal(origen.tamal) {
    tamaf = siguiente_pot2(tamal == 0 ? 1u : tamal);
    v = new T[tamaf];
    for (unsigned int i = 0; i < tamal; ++i) v[i] = origen.v[i];
}

// Constructor copia parcial
template<typename T>
VDinamico<T>::VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos) {
    if (posicionInicial > origen.tamal) throw std::out_of_range("posicionInicial fuera de rango");
    if (posicionInicial + numElementos > origen.tamal) throw std::out_of_range("rango (posicionInicial+numElementos) fuera de tamal");
    tamal = numElementos;
    tamaf = siguiente_pot2(tamal == 0 ? 1u : tamal);
    v = new T[tamaf];
    for (unsigned int i = 0; i < tamal; ++i) v[i] = origen.v[posicionInicial + i];
}

// Operador de asignación
template<typename T>
VDinamico<T>& VDinamico<T>::operator=(const VDinamico<T>& otro) {
    if (this == &otro) return *this;
    // liberar memoria actual
    delete[] v;
    tamal = otro.tamal;
    tamaf = siguiente_pot2(tamal == 0 ? 1u : tamal);
    v = new T[tamaf];
    for (unsigned int i = 0; i < tamal; ++i) v[i] = otro.v[i];
    return *this;
}

// Operadores []
template<typename T>
T& VDinamico<T>::operator[](unsigned pos) {
    if (pos >= tamal) throw std::out_of_range("operator[]: pos fuera de rango");
    return v[pos];
}

template<typename T>
const T& VDinamico<T>::operator[](unsigned pos) const {
    if (pos >= tamal) throw std::out_of_range("operator[] const: pos fuera de rango");
    return v[pos];
}

// Insertar
template<typename T>
void VDinamico<T>::insertar(const T& dato, unsigned int pos) {
    if (pos == UINT_MAX) pos = tamal; // insertar al final
    if (pos > tamal) throw std::out_of_range("insertar: pos fuera de rango");

    // si no hay espacio, duplicar tamaf
    if (tamal + 1 > tamaf) {
        unsigned int nueva_tamaf = tamaf == 0 ? 1 : tamaf << 1;
        T* nuevo = new T[nueva_tamaf];
        // copiar antes de pos
        for (unsigned int i = 0; i < pos; ++i) nuevo[i] = v[i];
        // insertar
        nuevo[pos] = dato;
        // copiar resto
        for (unsigned int i = pos; i < tamal; ++i) nuevo[i+1] = v[i];
        delete[] v;
        v = nuevo;
        tamaf = nueva_tamaf;
    } else {
        // desplazar hacia la derecha desde el final hasta pos
        for (unsigned int i = tamal; i > pos; --i) v[i] = v[i-1];
        v[pos] = dato;
    }
    ++tamal;
}

// Borrar
template<typename T>
T VDinamico<T>::borrar(unsigned int pos) {
    if (tamal == 0) throw std::out_of_range("borrar: vector vacio");
    if (pos == UINT_MAX) pos = tamal - 1; // borrar ultimo
    if (pos >= tamal) throw std::out_of_range("borrar: pos fuera de rango");

    T valor = v[pos];
    // desplazar a la izquierda
    for (unsigned int i = pos; i + 1 < tamal; ++i) v[i] = v[i+1];
    --tamal;

    // reducir tamaf si corresponde (mantener potencia de 2 >= max(1, tamal))
    unsigned int requerido = siguiente_pot2(tamal == 0 ? 1u : tamal);
    if (requerido < tamaf) {
        T* nuevo = new T[requerido];
        for (unsigned int i = 0; i < tamal; ++i) nuevo[i] = v[i];
        delete[] v;
        v = nuevo;
        tamaf = requerido;
    }

    return valor;
}

// Ordenar
template<typename T>
void VDinamico<T>::ordenar() {
    std::sort(v, v + tamal);
}

// Destructor
template<typename T>
VDinamico<T>::~VDinamico() {
    delete[] v;
}

// Fin de VDinamico.cpp
