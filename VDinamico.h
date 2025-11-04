#ifndef VDINAMICO_H
#define VDINAMICO_H

#include <stdexcept>
#include <algorithm>
#include <iostream>

template <typename T>
class VDinamico {
private:
    T* mem;
    unsigned int tamal = 0;
    unsigned int tamaf = 1;

public:

    VDinamico() {
        mem = new T[tamaf];
    }

    VDinamico(unsigned int tamlog) {
        tamal = tamlog;
        unsigned int aux = 1;
        while (aux < tamal) {
            aux *= 2;
        }
        tamaf = aux;
        mem = new T[tamaf];
    }

    VDinamico(const VDinamico<T>& origen) {
        tamaf = origen.tamaf;
        tamal = origen.tamal;
        mem = new T[tamaf];
        for (unsigned int i = 0; i < tamal; ++i) {
            mem[i] = origen.mem[i];
        }
    }

    VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos) {
        if (posicionInicial + numElementos > origen.tamal) {
            throw std::out_of_range("Rango fuera de los límites.");
        }
        tamal = numElementos;
        unsigned int aux = 1;
        while (aux < tamal) {
            aux *= 2;
        }
        tamaf = aux;
        mem = new T[tamaf];
        for (unsigned int i = 0; i < numElementos; ++i) {
            mem[i] = origen.mem[posicionInicial + i];
        }
    }

    ~VDinamico() {
        delete[] mem;
    }


    VDinamico<T>& operator=(const VDinamico<T>& origen) {
        if (this != &origen) {
            delete[] mem;
            tamaf = origen.tamaf;
            tamal = origen.tamal;
            mem = new T[tamaf];
            for (unsigned int i = 0; i < tamal; ++i) {
                mem[i] = origen.mem[i];
            }
        }
        return *this;
    }

    T& operator[](unsigned int pos) {
        if (pos >= tamal) {
            throw std::out_of_range("Índice fuera del rango.");
        }
        return mem[pos];
    }

    const T& operator[](unsigned int pos) const {
        if (pos >= tamal) {
            throw std::out_of_range("Índice fuera del rango.");
        }
        return mem[pos];
    }


    unsigned int tamlog() const {
        return tamal;
    }

    void insertar(const T& dato, unsigned int pos = -1) {
        if (pos == (unsigned int)-1) {
            pos = tamal;
        }
        if (pos > tamal) {
            throw std::out_of_range("Posicion fuera del rango");
        }
        if (tamal >= tamaf) {
            tamaf = (tamaf == 0) ? 1 : tamaf * 2;
            T* vaux = new T[tamaf];
            for (unsigned int i = 0; i < tamal; i++) {
                vaux[i] = mem[i];
            }
            delete[] mem;
            mem = vaux;
        }
        for (int i = tamal; i > (int)pos; --i) {
            mem[i] = mem[i - 1];
        }
        mem[pos] = dato;
        tamal++;
    }

    T borrar(unsigned int pos = -1) {
        if (tamal == 0) {
            throw std::out_of_range("El vector está vacío.");
        }
        if (pos == (unsigned int)-1) {
            pos = tamal - 1;
        }
        if (pos >= tamal) {
            throw std::out_of_range("Posicion fuera del rango");
        }

        T vaux = mem[pos];
        for (unsigned i = pos; i < tamal - 1; i++) {
            mem[i] = mem[i + 1];
        }
        tamal--;

        if (tamal > 0 && tamal * 3 < tamaf) {
            tamaf /= 2;
            T* vaux2 = new T[tamaf];
            for (int i = 0; i < tamal; i++) {
                vaux2[i] = mem[i];
            }
            delete[] mem;
            mem = vaux2;
        }
        return vaux;
    }

    void ordenar() {
        std::sort(mem, mem + tamal);
    }
    
    int busquedaBin(const T &dato) const {
        int inf = 0;
        int sup = tamal - 1;
        int curIn;
        while (inf <= sup) {
            curIn = (inf + sup) / 2;
            if (mem[curIn] == dato)
                return curIn;
            else if (mem[curIn] < dato) {
                inf = curIn + 1;
            } else {
                sup = curIn - 1;
            }
        }
        return -1;
    }

};

#endif //VDINAMICO_H