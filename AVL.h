#ifndef AVL_H
#define AVL_H

#include "VDinamico.h"

template<typename T>
class Nodo {
public:
    Nodo<T> *izq, *der;
    T dato;
    char bal; // -1, 0 , 1 para un árbol avl
    Nodo(T &ele) : izq(0), der(0), bal(0), dato(ele) {}
};


template<typename T>
class AVL {
    Nodo<T> *raiz;
private:
    //Operacion de insercion en el arbol
    int inserta(Nodo<T> *&c, T &dato);

    //Rotacion a derechas dado un nodo
    void rotDecha(Nodo<T> *&p);

    //Rotacion a izquierdas dado un nodo
    void rotIzqda(Nodo<T> *&p);

    void copiaAVL(Nodo<T> *&q, Nodo<T> *p);//Necesario para el copia y el =

    void destruyeArbol(Nodo<T> *&p); //Necesario para el destuctor y el =

    Nodo<T> *buscaClave(const T &dato, Nodo<T> *p);

    void inorden(Nodo<T> *p, int nivel, VDinamico<T *> &v);

    void preorden(Nodo<T> *p, int nivel, VDinamico<T *> &v);

    unsigned int numElementosPr(Nodo<T> *p);

    void alturaPr(Nodo<T> *p, int nivel, int &result);


public:
    //Constructor por defecto
    AVL() : raiz(0) {}

    //Constructor de copia
    AVL(const AVL<T> &origen) { copiaAVL(raiz, origen.raiz); } //LLAMA A CopiaAVL

    //Operador de asignacion
    AVL<T> &operator=(const AVL<T> &orig);

    //Operacion de insercion del dato
    bool insercion(T &dato){ return inserta(raiz,dato); };

    //Operacion de busqueda recursiva
    T *buscaRec(T &dato);

    //Operacion de busqueda iterativa
    T *buscaIt(T &dato);

    //Recorrido en Inorden
    VDinamico<T *> recorreInorden();

    //Recorrido en Preorden
    VDinamico<T *> recorrePreorden();

    //Numero de elementos del AVL
    unsigned int numElementos() { return numElementosPr(raiz); }

    //Altura del AVL
    unsigned int altura();

    //Destructor
    ~AVL() { destruyeArbol(raiz); }

    void borrar() { destruyeArbol(raiz); }

    bool inserta2(const T &dato);

};

/**
  * @brief Metodo para realizar una rotación hacia la izquierda en el subárbol sobre el
  * nodo p para restaurar el equilibrio del árbol AVL.
  * @param p Referencia al nodo raíz del subárbol que se va a rotar.
  */
template<typename T>
void AVL<T>::rotIzqda(Nodo<T> *&p) {
    Nodo<T> *q = p, *r;
    p = r = q->der;
    q->der = r->izq;
    r->izq = q;
    q->bal++;
    if (r->bal < 0) q->bal += -r->bal;
    r->bal++;
    if (q->bal > 0) r->bal += q->bal;
}

/**
  * @brief Metodo para realizar una rotación hacia la derecha en el subárbol sobre el
  * nodo p para restaurar el equilibrio del árbol AVL
  * @param p Referencia al nodo raíz del subárbol que se va a rotar.
  */
template<typename T>
void AVL<T>::rotDecha(Nodo<T> *&p) {
    Nodo<T> *q = p, *l;
    p = l = q->izq;
    q->izq = l->der;
    l->der = q;
    q->bal--;
    if (l->bal > 0) q->bal -= l->bal;
    l->bal--;
    if (q->bal < 0) l->bal -= -q->bal;
}

/**
  * @brief Metodo recursivo para insertar un dato en el árbol AVL, llamándose para insertar un nuevo dato
  * en el árbol y balancear el árbol si es necesario. Se actualiza
  * el balance de los nodos y se realizan rotaciones en caso de
  * desbalance.
  * @param c Referencia al nodo raíz del subárbol donde se insertará el dato.
  * @param dato Dato a insertar en el árbol.
  * @return int Indica si se realizó un cambio en la altura del árbol
  * (0 si no hubo cambio, 1 si se incrementó).
  */
template<typename T>
int AVL<T>::inserta(Nodo<T> *&c, T &dato) {
    Nodo<T> *p = c;
    int deltaH = 0;
    if (!p) {
        p = new Nodo<T>(dato);
        c = p;
        deltaH = 1;
    } else if (dato > p->dato) {
        if (inserta(p->der, dato)) {
            p->bal--;
            if (p->bal == -1) deltaH = 1;
            else if (p->bal == -2) {
                if (p->der->bal == 1) rotDecha(p->der);
                rotIzqda(c);
            }
        }
    } else if (dato < p->dato) {
        if (inserta(p->izq, dato)) {
            p->bal++;
            if (p->bal == 1) deltaH = 1;
            else if (p->bal == 2) {
                if (p->izq->bal == -1) rotIzqda(p->izq);
                rotDecha(c);
            }
        }
    }
    return deltaH;
}


/**
  * @brief Constructor de copia del árbol AVL
  * @param q Referencia al nodo raíz del árbol destino.
  * @param p Puntero al nodo raíz del árbol fuente que se va a copiar.
  */
template<typename T>
void AVL<T>::copiaAVL(Nodo<T> *&q, Nodo<T> *p) {
    if (p) {
        q = new Nodo<T>(p->dato);
        q->bal = p->bal;  // Copia el valor de bal para mantener el balance
        copiaAVL(q->izq, p->izq);
        copiaAVL(q->der, p->der);
    } else {
        q = nullptr;
    }
}

/**
  * @brief Operador de asignacion del arbol AVL
  * @param orig Referencia al árbol AVL que se va a copiar.
  * @return AVL<T>& Devuelve una referencia al árbol actual.
  */
template<typename T>
AVL<T> &AVL<T>::operator=(const AVL<T> &orig) {
    if (this != &orig) {
        destruyeArbol(raiz);
        copiaAVL(raiz, orig.raiz);
    }
    return *this;
}

/**
  * @brief Destructor del árbol AVL, llamandose recursivamente para liberar la memoria
  * de todos los nodos en el árbol.
  * @param p Referencia al nodo raíz del árbol que se va a destruir.
  */
template<typename T>
void AVL<T>::destruyeArbol(Nodo<T> *&p) {
    if (p) {
        destruyeArbol(p->izq);
        destruyeArbol(p->der);
        delete p;
        p = 0;
    }
}

/**
  * @brief Metodo para devolver un dato en el árbol AVL mediante busqueda recursiva.
  * @param dato Dato a buscar en el árbol.
  * @return T* Puntero al dato encontrado o nullptr si no se encuentra.
  */
template<typename T>
T *AVL<T>::buscaRec(T &dato) {
    Nodo<T> *p = buscaClave(dato, raiz);
    T *result;
    if (p) {
        result = &(p->dato);
        return result;
    }
    return 0;
}


/**
  * @brief Metodo para buscar un dato en el árbol AVL de forma recursiva.
  * @param dato Dato a buscar en el árbol.
  * @param p Nodo actual donde se realiza la búsqueda.
  * @return Nodo<T>* Puntero al nodo que contiene el dato encontrado o nullptr si no se encuentra.
  */
template<typename T>
Nodo<T> *AVL<T>::buscaClave(const T &dato, Nodo<T> *p) {
    if (!p)
        return 0;
    else {
        T d = dato;
        if (d < p->dato)
            return buscaClave(dato, p->izq);
        else if (p->dato < dato)
            return buscaClave(dato, p->der);
        else
            return p;
    }
}

/**
  *@brief Metodo para buscar un dato en el arbol AVL mediante busqueda iterativa
  * @param p Nodo actual en el recorrido.
  * @param nivel Nivel de profundidad actual en el árbol.
  * @param v Referencia a un vector dinámico donde se almacenan los datos.
  */
template<typename T>
T *AVL<T>::buscaIt(T &dato) {
    Nodo<T> *p = raiz;
    T *result;
    while (p) {
        T d = dato;
        if (d < p->dato)
            p = p->izq;
        else if (p->dato < d)
            p = p->der;
        else {
            result = &(p->dato);
            return result;
        }
    }
    return 0;
}


/**
  * @brief Metodo recursivo para realizar un recorrido Inorden del árbol AVL.
  * @param p Nodo actual en el recorrido.
  * @param nivel Nivel de profundidad actual en el árbol.
  * @param v Referencia a un vector dinámico donde se almacenan los datos.
  */
template<typename T>
void AVL<T>::inorden(Nodo<T> *p, int nivel, VDinamico<T *> &v) {
    if (p) {
        inorden(p->izq, nivel + 1, v);
        v.insertar(&(p->dato));
        inorden(p->der, nivel + 1, v);
    }
}

/**
  * @brief Metodo para realizar un recorrido Inorden y devolver un vector dinámico con los datos.
  * @return VDinamico<T*> Vector dinámico que contiene los datos del árbol.
  */
template<typename T>
VDinamico<T *> AVL<T>::recorreInorden() {
    VDinamico<T *> v;
    inorden(raiz, 0, v);
    return v;
}

/**
  * @brief Metodo recursivo para contar el número de elementos en el árbol AVL.
  * @param p Nodo actual desde el cual se cuenta.
  * @return unsigned int Número total de elementos en el árbol.
  */
template<typename T>
unsigned int AVL<T>::numElementosPr(Nodo<T> *p) {
    if (p)
        return numElementosPr(p->izq) + numElementosPr(p->der) + 1;
    else
        return 0;
}


/**
  * @brief Metodo recursivo para calcular la altura del árbol AVL desde un nodo determinado
  * @param p Nodo actual desde el cual se calcula la altura.
  * @param nivel Nivel actual de profundidad en el árbol.
  * @param result Referencia a la variable que contendrá el resultado final.
  */
template<typename T>
void AVL<T>::alturaPr(Nodo<T> *p, int nivel, int &result) {
    if (p) {
        alturaPr(p->izq, nivel + 1, result);
        if (nivel > result)
            result = nivel;
        alturaPr(p->der, nivel + 1, result);
    }
}

/**
  * @brief Metodo para obtener la altura del árbol AVL.
  * @return result siendo la altura total del árbol.
  */
template<typename T>
unsigned int AVL<T>::altura() {
    int result = 0;
    alturaPr(raiz, 0, result);
    return result;
}


/**
  * @brief Metodo recursivo para realizar un recorrido en preorden del árbol AVL.
  * @param p Nodo actual en el recorrido.
  * @param nivel Nivel de profundidad actual en el árbol.
  * @param v Referencia a un vector dinámico donde se almacenan los datos.
  */
template<typename T>
void AVL<T>::preorden(Nodo<T> *p, int nivel, VDinamico<T *> &v) {
    if (p) {
        v.insertar(&(p->dato));
        preorden(p->izq, nivel + 1, v);
        preorden(p->der, nivel + 1, v);
    }
}

/**
  * @brief Metodo para realizar un recorrido preorden y devolver un vector dinámico con los datos.
  * @return VDinamico<T*> Vector dinámico que contiene los datos del árbol.
  */
template<typename T>
VDinamico<T *> AVL<T>::recorrePreorden() {
    VDinamico<T *> v;
    preorden(raiz, 0, v);
    return v;
}

#endif //AVL_H
