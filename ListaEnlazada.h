#ifndef LISTAENLAZADA_H
#define LISTAENLAZADA_H

#include <iostream>

template<class T>
class ListaEnlazada {

    template<class X>
    class Nodo {
    public:
        X dato;
        Nodo *ant, *sig;

        Nodo(const X &aDato, Nodo *aAnt, Nodo *aSig) : dato(aDato), ant(aAnt), sig(aSig) {}
    };

    Nodo<T> *cabecera, *cola;
    int tama = 0;

public:
    class Iterador {
        Nodo<T> *nodo;

        friend class ListaEnlazada<T>;

    public:
        Iterador(Nodo<T> *aNodo) : nodo(aNodo) {}

        bool hayAnterior() { return nodo != 0; }

        bool haySiguiente() const { return nodo != 0; }

        void anterior() { if (nodo) nodo = nodo->ant; }

        void siguiente() { if (nodo) nodo = nodo->sig; }

        T &dato() { return nodo->dato; }
    };

    ListaEnlazada() : cabecera(0), cola(0) {}

    ListaEnlazada(const ListaEnlazada &l);

    ListaEnlazada &operator=(ListaEnlazada &l);

    Iterador iteradorInicio() const { return Iterador(cabecera); }

    Iterador iteradorFinal() { return Iterador(cola); }

    void insertarInicio(T &dato);

    void insertarFinal(const T &dato);

    void insertar(Iterador &p, T &dato);

    void insertarDelante(Iterador &p, T &dato);

    void insertarDetras(Iterador &p, T &dato);

    void borrarInicio();

    void borrarFinal();

    void borrar(Iterador &p);

    T &inicio() { return cabecera->dato; }

    T &final() { return cola->dato; }

    int tam() const { return tama; }

    ListaEnlazada<T> concatena(const ListaEnlazada<T> &p);

    ListaEnlazada<T> operator+(ListaEnlazada &p);

    ~ListaEnlazada();

};

// Constructor de copia
template<typename T>
ListaEnlazada<T>::ListaEnlazada(const ListaEnlazada &l) : cabecera(0), cola(0) {
    Nodo<T> *aux = l.cabecera;

    while (aux != nullptr) {
        insertarFinal(aux->dato);
        aux = aux->sig;
    }
}

// Inserción al inicio de la lista
template<typename T>
void ListaEnlazada<T>::insertarInicio(T &dato) {
    Nodo<T> *nuevo;
    Nodo<T> *eaux = 0;
    nuevo = new Nodo<T>(dato, eaux, cabecera);
    // Caso especial: si la lista estaba vacía, poner la cola apuntando al nodo
    if (cola == 0)
        cola = nuevo;
    if (cabecera != 0)
        cabecera->ant = nuevo;
    cabecera = nuevo;
    ++tama;

}

// Inserción al final de la lista
template<typename T>
void ListaEnlazada<T>::insertarFinal(const T &dato) {
    Nodo<T> *nuevo;
    Nodo<T> *eaux = 0;
    nuevo = new Nodo<T>(dato, cola, eaux);
    if (cabecera == 0)  // Caso especial: lista vacía
        cabecera = nuevo;
    if (cola != 0)
        cola->sig = nuevo;

    cola = nuevo;
    ++tama;
}

//Insercion de un dato en la posicion anterior apuntada por un iterador
template<typename T>
void ListaEnlazada<T>::insertar(Iterador &p, T &dato) {
    Nodo<T> *nuevo = new Nodo<T>(dato, p.nodo->ant, p.nodo);  // Acceder correctamente al nodo
    if (p.nodo->ant != nullptr) { // Verificar si el nodo anterior no es nulo
        p.nodo->ant->sig = nuevo; // Conectar el nodo anterior al nuevo
    }
    p.nodo->ant = nuevo; // Actualizar el puntero del nodo actual
    ++tama;
}

template<typename T>
void ListaEnlazada<T>::insertarDelante(Iterador &p, T &dato) {

insertar(p, dato);
}

template<typename T>
void ListaEnlazada<T>::insertarDetras(Iterador &p, T &dato) {
    p.siguiente();
    insertar(p, dato);
}

template<typename T>
ListaEnlazada<T> &ListaEnlazada<T>::operator=(ListaEnlazada &l) {
    if (this != &l) { // Evita la autoasignación
        Nodo<T> *actual = cabecera;
        while (actual != nullptr) {
            Nodo<T> *siguiente = actual->sig;
            delete actual;  // Liberar el nodo actual
            actual = siguiente;
        }
        cabecera = nullptr;
        cola = nullptr;

        ListaEnlazada(l);
        std::cout << "Asignacion completada." << std::endl;
        return *this;
    } else {
        throw std::string("La lista enlazada es la misma");
    }

}

template<typename T>
void ListaEnlazada<T>::borrarInicio() {
    Nodo<T> *borrado = cabecera;

    cabecera = cabecera->sig;
    delete borrado;
    if (cabecera != 0)
        cabecera->ant = 0;
    else
        cola = 0;
    --tama;
}

template<typename T>
void ListaEnlazada<T>::borrarFinal() {
    Nodo<T> *anterior = 0;
    if (cabecera != cola) {
        anterior = cola->ant;
    }
    delete cola;
    cola = anterior;
    if (anterior != 0)
        anterior->sig = 0;
    else
        cabecera = 0;
    --tama;
}

template<typename T>
void ListaEnlazada<T>::borrar(Iterador &p) {
    if (p.nodo == cabecera) {
        Nodo<T> *borrado = cabecera;
        cabecera = cabecera->sig;
        delete borrado;
        if (cabecera == 0)
            cola = 0;
    } else {
        Nodo<T> *anterior = 0;
        Nodo<T> *siguiente = 0;
        if (cabecera != cola) {
            anterior = p.nodo->ant;
            siguiente = p.nodo->sig;

        }
        anterior->sig = siguiente;
        if (siguiente)
            siguiente->ant = anterior;
        delete p.nodo;
    }
    --tama;
}

template<typename T>
ListaEnlazada<T> ListaEnlazada<T>::concatena(const ListaEnlazada<T> &p) {
    // Hacemos una copia de la lista actual
    ListaEnlazada<T> toret = *this;

    // Recorremos la lista 'p' y añadimos cada elemento al final de 'toret'
    Nodo<T> *aux = p.cabecera;
    while (aux) {
        toret.insertarFinal(aux->dato);
        aux = aux->sig;
    }

    return toret;
}

template<typename T>
ListaEnlazada<T> ListaEnlazada<T>::operator+(ListaEnlazada &p) {
    // Hacemos una copia de la lista actual
    ListaEnlazada<T> toret = *this;

    // Recorremos la lista 'p' y añadimos cada elemento al final de 'toret'
    Nodo<T> *aux = p.cabecera;
    while (aux) {
        toret.insertarFinal(aux->dato);
        aux = aux->sig;
    }

    return toret;
}


// Destructor de la lista enlazada
template<typename T>
ListaEnlazada<T>::~ListaEnlazada() {
    Nodo<T> *actual = cabecera;
    while (actual != nullptr) {
        Nodo<T> *siguiente = actual->sig;
        delete actual;  // Liberar el nodo actual
        actual = siguiente;
    }
    cabecera = nullptr;
    cola = nullptr;
}


#endif //LISTAENLAZADA_H
