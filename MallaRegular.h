#ifndef MALLAREGULAR_H
#define MALLAREGULAR_H

#include <cmath>
#include <vector>
#include <list>
#include <algorithm> // Necesario para sort
#include <iostream>

using namespace std;

template<class T>
class MallaRegular {
private:
    // Clase interna Casilla
    class Casilla {
        list<T> puntos;
    public:
        Casilla() : puntos() {}
        void insertar(const T &dato) { puntos.push_back(dato); }
        
        T *buscar(const T &dato) {
            typename list<T>::iterator it = puntos.begin();
            for (; it != puntos.end(); it++) {
                if (*it == dato) return &(*it);
            }
            return 0;
        }

        bool borrar(const T &dato) {
            typename list<T>::iterator it = puntos.begin();
            for (; it != puntos.end(); it++) {
                if (*it == dato) {
                    puntos.erase(it);
                    return true;
                }
            }
            return false;
        }

        unsigned get_Tam() { return puntos.size(); }
        typename list<T>::iterator begin() { return puntos.begin(); }
        typename list<T>::iterator end() { return puntos.end(); }
    };

    float xMin, yMin, xMax, yMax;
    float tamaCasillaX, tamaCasillaY;
    unsigned taml; // Número total de elementos
    int nDiv;      // Número de divisiones por eje
    vector<vector<Casilla>> mr; // Matriz de casillas

    Casilla *obtenerCasilla(float x, float y);

public:
    MallaRegular() : xMin(0), yMin(0), xMax(0), yMax(0), taml(0), nDiv(0) {};
    
    // Constructor actualizado
    MallaRegular(float aXMin, float aYMin, float aXMax, float aYMax, int aNDiv);

    void insertar(float x, float y, const T &dato);
    T *buscar(float x, float y, const T &dato);
    bool borrar(float x, float y, const T &dato);

    // --- NUEVO MÉTODO REQUERIDO POR LA PRÁCTICA ---
    vector<T> buscarCercana(float xcentro, float ycentro, int n = 1);
    // ---------------------------------------------

    unsigned numElementos() { return taml; }
    unsigned maxElementosPorCelda();
    float promedioElementosPorCelda();
};

// --- IMPLEMENTACIÓN DE LOS MÉTODOS DEL TEMPLATE ---

template<typename T>
MallaRegular<T>::MallaRegular(float aXMin, float aYMin, float aXMax, float aYMax, int aNDiv)
    : xMin(aXMin), yMin(aYMin), xMax(aXMax), yMax(aYMax), nDiv(aNDiv), taml(0) {
    
    tamaCasillaX = (xMax - xMin) / nDiv;
    tamaCasillaY = (yMax - yMin) / nDiv;
    
    // Inicializar el vector de vectores (matriz)
    mr.resize(nDiv);
    for (int i = 0; i < nDiv; ++i) {
        mr[i].resize(nDiv);
    }
}

template<typename T>
typename MallaRegular<T>::Casilla *MallaRegular<T>::obtenerCasilla(float x, float y) {
    int i = (x - xMin) / tamaCasillaX;
    int j = (y - yMin) / tamaCasillaY;
    
    // Control de límites para evitar segfaults en bordes máximos
    if (i >= nDiv) i = nDiv - 1;
    if (j >= nDiv) j = nDiv - 1;
    if (i < 0) i = 0;
    if (j < 0) j = 0;

    return &mr[i][j];
}

template<typename T>
void MallaRegular<T>::insertar(float x, float y, const T &dato) {
    Casilla *c = obtenerCasilla(x, y);
    c->insertar(dato);
    taml++;
}

template<typename T>
bool MallaRegular<T>::borrar(float x, float y, const T &dato) {
    Casilla *c = obtenerCasilla(x, y);
    if (c->borrar(dato)) {
        taml--;
        return true;
    }
    return false;
}

// Implementación de buscarCercana (Algoritmo: Recolectar todo + Ordenar)
// Nota: Para sets de datos enormes se usa búsqueda en espiral, pero para esta práctica
// y asegurar precisión absoluta en "la más cercana", recolectar y ordenar es robusto.
template<typename T>
vector<T> MallaRegular<T>::buscarCercana(float xcentro, float ycentro, int n) {
    vector<pair<double, T>> candidatos;
    vector<T> resultado;

    // Recorremos todas las casillas
    for (int i = 0; i < nDiv; ++i) {
        for (int j = 0; j < nDiv; ++j) {
            Casilla& c = mr[i][j];
            if (c.get_Tam() > 0) {
                for (auto it = c.begin(); it != c.end(); ++it) {
                    T elem = *it;
                    // Distancia Euclidiana
                    // Asumimos que T tiene métodos getX() y getY() (como Farmacia)
                    double dx = elem->getX() - xcentro;
                    double dy = elem->getY() - ycentro;
                    double dist = std::sqrt(dx*dx + dy*dy);
                    candidatos.push_back({dist, elem});
                }
            }
        }
    }

    // Ordenamos por distancia de menor a mayor
    std::sort(candidatos.begin(), candidatos.end(), 
        [](const pair<double, T>& a, const pair<double, T>& b) {
            return a.first < b.first; 
        });

    // Devolvemos los n primeros
    for (int k = 0; k < n && k < candidatos.size(); ++k) {
        resultado.push_back(candidatos[k].second);
    }
    return resultado;
}

template<typename T>
unsigned MallaRegular<T>::maxElementosPorCelda() {
    unsigned maximo = 0;
    for (int i = 0; i < nDiv; ++i) {
        for (int j = 0; j < nDiv; ++j) {
            unsigned tam = mr[i][j].get_Tam();
            if (tam > maximo) maximo = tam;
        }
    }
    return maximo;
}

template<typename T>
float MallaRegular<T>::promedioElementosPorCelda() {
    int celdasOcupadas = 0;
    for (int i = 0; i < nDiv; ++i) {
        for (int j = 0; j < nDiv; ++j) {
            if (mr[i][j].get_Tam() > 0) celdasOcupadas++;
        }
    }
    if (celdasOcupadas == 0) return 0.0;
    // Según el formato del txt output parece calcularse sobre total elementos / celdas ocupadas
    // o total elementos / total celdas.
    // El PDF dice "conocer la carga... decidir el tamaño".
    // "Media ocupacion: 0.00109..." en el txt sugiere densidad baja.
    // Vamos a devolver promedio general: total elementos / total celdas (o celdas ocupadas)
    // Usualmente es: numElementos / numCeldasTotal
    return (float)taml / (nDiv * nDiv); 
}

#endif