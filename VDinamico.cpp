// Fichero: VDinamico.cpp
// Contiene las definiciones de los métodos de la plantilla VDinamico<T>.
// NOTA: Este fichero está pensado para ser incluido al final de VDinamico.h

#include <utility>
#include <algorithm>
#include "VDinamico.h"

template<typename T>
unsigned int VDinamico<T>::siguientePotencia2(unsigned int n) {
    if (n == 0u) return 1u;
    unsigned int p = 1u;
    while (p < n) {
        if (p > (UINT_MAX >> 1)) { p = UINT_MAX; break; }
        p <<= 1u;
    }
    return p;
}

template<typename T>
void VDinamico<T>::reservar(unsigned int nuevaFis) {
    if (nuevaFis < 1u) nuevaFis = 1u;
    T* nuevo = new T[nuevaFis];
    unsigned int tocopy = (m_tamlog < nuevaFis) ? m_tamlog : nuevaFis;
    for (unsigned int i = 0; i < tocopy; ++i) nuevo[i] = datos[i];
    delete[] datos;
    datos = nuevo;
    m_tamfis = nuevaFis;
}

template<typename T>
VDinamico<T>::VDinamico(): datos(nullptr), m_tamlog(0), m_tamfis(1) {
    datos = new T[m_tamfis];
}

template<typename T>
VDinamico<T>::VDinamico(unsigned int tamlog): datos(nullptr), m_tamlog(tamlog) {
    m_tamfis = siguientePotencia2(tamlog);
    if (m_tamfis < 1u) m_tamfis = 1u;
    datos = new T[m_tamfis];
}

template<typename T>
VDinamico<T>::VDinamico(const VDinamico<T>& origen): datos(nullptr), m_tamlog(origen.m_tamlog), m_tamfis(origen.m_tamfis) {
    datos = new T[m_tamfis];
    for (unsigned int i = 0; i < m_tamlog; ++i) datos[i] = origen.datos[i];
}

template<typename T>
VDinamico<T>::VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos) {
    if (posicionInicial > origen.m_tamlog) throw std::out_of_range("posicionInicial fuera de rango");
    if (posicionInicial + numElementos > origen.m_tamlog) throw std::out_of_range("numElementos fuera de rango");

    m_tamlog = numElementos;
    m_tamfis = siguientePotencia2(numElementos);
    if (m_tamfis < 1u) m_tamfis = 1u;
    datos = new T[m_tamfis];
    for (unsigned int i = 0; i < m_tamlog; ++i) datos[i] = origen.datos[posicionInicial + i];
}

template<typename T>
VDinamico<T>::~VDinamico() {
    delete[] datos;
}


template<typename T>
VDinamico<T>& VDinamico<T>::operator=(const VDinamico<T>& otro) {
    if (this == &otro) return *this;
    T* nuevo = new T[otro.m_tamfis];
    for (unsigned int i = 0; i < otro.m_tamlog; ++i) nuevo[i] = otro.datos[i];

    delete[] datos;
    datos = nuevo;
    m_tamlog = otro.m_tamlog;
    m_tamfis = otro.m_tamfis;
    return *this;
}


template<typename T>
T& VDinamico<T>::operator[](unsigned int pos) {
    if (pos >= m_tamlog) throw std::out_of_range("indice fuera de rango");
    return datos[pos];
}

template<typename T>
const T& VDinamico<T>::operator[](unsigned int pos) const {
    if (pos >= m_tamlog) throw std::out_of_range("indice fuera de rango");
    return datos[pos];
}


template<typename T>
void VDinamico<T>::insertar(const T& dato, unsigned int pos) {
    if (pos == UINT_MAX) pos = m_tamlog; // insertar al final
    if (pos > m_tamlog) throw std::out_of_range("posicion de insercion fuera de rango");

    if (m_tamlog + 1 > m_tamfis) {
        unsigned int nuevaFis = (m_tamfis == 0u) ? 1u : (m_tamfis << 1u);
        if (nuevaFis == 0u) nuevaFis = 1u;
        reservar(nuevaFis);
    }

    for (unsigned int i = m_tamlog; i > pos; --i) datos[i] = datos[i-1];
    datos[pos] = dato;
    ++m_tamlog;
}


template<typename T>
T VDinamico<T>::borrar(unsigned int pos) {
    if (m_tamlog == 0) throw std::out_of_range("vector vacio");
    if (pos == UINT_MAX) pos = m_tamlog - 1;
    if (pos >= m_tamlog) throw std::out_of_range("posicion de borrado fuera de rango");

    T valor = datos[pos];
    for (unsigned int i = pos; i + 1 < m_tamlog; ++i) datos[i] = datos[i+1];
    --m_tamlog;

    if (m_tamlog > 0 && m_tamlog <= (m_tamfis >> 2u)) {
        unsigned int nuevaFis = m_tamfis >> 1u;
        if (nuevaFis < 1u) nuevaFis = 1u;
        if (nuevaFis < m_tamlog) nuevaFis = siguientePotencia2(m_tamlog);
        reservar(nuevaFis);
    }

    return valor;
}


template<typename T>
void VDinamico<T>::ordenar() {
    std::sort(datos, datos + m_tamlog);
}

template<typename T>
unsigned int VDinamico<T>::tamlog() const {
    return m_tamlog;
}
