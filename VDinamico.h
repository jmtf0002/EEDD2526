//
// Integrantes de la pareja:
//Javier Martinez Gonzalez - jmg00144@red.ujaen.es
//Maria Lucia Gomez Gutierrez - mlgg0019@red.ujaen.es
//

#ifndef VDINAMICO_H
#define VDINAMICO_H

#include <stdexcept>  // Libreria usada para las excepciones
#include <algorithm>  // Libreria usada para la función sort
#include <iostream>



template <typename T>
class VDinamico {
private:

    T* mem;            // Vector
    unsigned int tamal = 0;
    unsigned int tamaf = 1;  // Tamaño del vector


public:



    // Constructor por defecto
    VDinamico();

    //Constructor parametrizado
    VDinamico(unsigned int tamal);

    // Constructor copia
    VDinamico(const VDinamico<T>& origen);

    // Constructor de copia parcial
    VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos);

    // Operador de asignación
    VDinamico<T>& operator=(const VDinamico<T>& origen);

    // Operador [] para acceso lectura/escritura
    T& operator[](unsigned int index);

    // Ordenar el vector de menor a mayor utilizando la función sort de algorithm
    void ordenar();

    //Ordenar el vector de menor a mayor utilizando el algoritmo Quicksort
    void quicksort(int primero, int ultimo);

    //Insertar un dato en una posición dada:
    void insertar(const T& dato, unsigned int pos = UINT_MAX);

    //borrae un dato:
    T borrar(unsigned int pos = UINT_MAX);

    int busquedaBin(T &dato);

    unsigned int tamlog();

    // Destructor
    ~VDinamico();
};


//Constructor por defecto
template <typename T>
VDinamico<T>::VDinamico() {
    mem = new T[tamaf];
    std::cout << "Espacio creado para " << 1 << " dato." << std::endl;
}

//Constructor parametrizado
template <typename T>
VDinamico<T>::VDinamico(unsigned int tamlog) {
    tamal = tamlog;
    int aux;
    for(int i=1; i<=tamal; i=i*2)
    {
        aux = i;
    }
    tamaf = aux*2;
    mem = new T[tamaf];std::cout << "Espacio creado para " << tamaf << " datos." << std::endl;
}

// Constructor copia
template <typename T>
VDinamico<T>::VDinamico(const VDinamico<T>& origen) {
    tamaf = origen.tamaf;
    tamal = origen.tamal;
    mem = new T[tamaf];
    for (unsigned int i = 0; i < tamal; ++i) {
        mem[i] = origen.mem[i];
    }
    std::cout << "Copia completa creada." << std::endl;
}

// Constructor de copia parcial
template <typename T>
VDinamico<T>::VDinamico(const VDinamico<T>& origen, unsigned int posicionInicial, unsigned int numElementos) {
    tamal = numElementos;
    //La excepción salta si el tamaño del vector que se quiere generar es superior al inicial
    if (posicionInicial + numElementos > origen.tamaf) {
        throw std::out_of_range("Rango fuera de los límites.");
    }
    int aux;
    for(int i=1; i<=numElementos; i=i*2)
      {
      aux = i;
      }
      numElementos = aux*2;

    mem = new T[numElementos];
    for (unsigned int i = 0; i < numElementos; ++i) {
        mem[i] = origen.mem[posicionInicial + i];
    }
    std::cout << "Copia parcial creada." << numElementos << std::endl;
}

// Operador de asignación
template <typename T>
VDinamico<T>& VDinamico<T>::operator=(const VDinamico<T>& origen) {
    if (this != &origen) { // Evitar autoasignación
        delete[] mem;
        tamaf = origen.tamaf;
        tamal = origen.tamal;
        mem = new T[tamaf];
        for (unsigned int i = 0; i < tamal; ++i) {
            mem[i] = origen.mem[i];
        }
        std::cout << "Asignación completada." << std::endl;
        return *this;
    }else{
        throw std::string("El vector introducido es el mismo");
    }

}

// Operador [] para acceso a lectura/escritura de un dato
template <typename T>
T& VDinamico<T>::operator[](unsigned int pos) {
    if (pos >= tamal) {
        throw std::out_of_range("Índice fuera del rango.");
    }
    return mem[pos];
}

// Ordenar el vector usando la función sort de <algorithm>
template <typename T>
void VDinamico<T>::ordenar() {

    std::sort(mem, mem + tamal); // La función sort
}





template <typename T>
void VDinamico<T>::insertar(const T& dato, unsigned int pos) {
    if(pos > tamal) {
        throw std::out_of_range("Posicion fuera del rango");
    }

    // Si se inserta al final
    if(pos == UINT_MAX) pos = tamal;

    // Expandir memoria si hace falta
    if(tamaf <= tamal + 1) {
        T* vaux = new T[tamaf * 2];
        for(unsigned int i = 0; i < tamal; i++)
            vaux[i] = mem[i];
        delete[] mem;
        mem = vaux;
        tamaf *= 2;
    }

    // Mover elementos si no es al final
    for(int i = tamal - 1; i >= (int)pos; --i) {
        mem[i + 1] = mem[i];
    }

    mem[pos] = dato;
    tamal++;
}


//Borrar
template<typename T>
T VDinamico<T>::borrar(unsigned int pos) {
    T vaux;
    T *vaux2;
    if(pos==UINT_MAX){
        vaux=mem[tamal-1];
        tamal--;
        if((tamal*3)<tamaf){
            tamaf=tamaf/2;
            vaux2=new T [tamaf];
            for(int i=0;i<tamal;i++) {
                vaux2[i] = mem[i];
            }
            delete []mem;
            mem=vaux2;
        }
    }else {
        vaux = mem[pos];
        for(unsigned i=pos;i<tamal-1;i++){
            mem[i]=mem[i+1];
        }
        tamal--;
        if ((tamal*3)<tamaf) {
            tamaf = tamaf / 2;
            vaux2 = new T[tamaf];
            for (int i = 0; i < tamal; i++) {
                vaux2[i] = mem[i];
            }
            delete[] mem;
            mem = vaux2;
        }
    }
    return vaux;
}


template<class T>
int VDinamico<T>::busquedaBin(T &dato)  {
    int inf = 0;
    int sup = tamal - 1;
    int curIn;
    while (inf <= sup) {
        curIn = (inf + sup) / 2;
        if (mem[curIn] == dato)
            return curIn;
        else
            if (mem[curIn] < dato) {
                inf = curIn + 1;
            }else sup = curIn - 1;
    }
    return -1;
}

template<typename T>
unsigned int VDinamico<T>::tamlog() {
    return tamal;
}


// Destructor
template <typename T>
VDinamico<T>::~VDinamico() {
    delete[] mem;
    std::cout << "Memoria liberada." << std::endl;
}

#endif //VDINAMICO_H
