

#ifndef MALLAREGULAR_H
#define MALLAREGULAR_H
#include <cmath>
#include <vector>
#include <list>

using namespace std;

template<class T>
class MallaRegular {
private:
    class Casilla{
    list<T> puntos;
    public:
        Casilla(): puntos(){}
        void insertar(const T &dato){puntos.push_back(dato);}
        T *buscar(const T &dato){
            typename list<T>::iterator it;
            it=puntos.begin();
            for(;it!=puntos.end();it++){
                if(*it==dato)
                    return &(*it);
            }
            return 0;
        };
        bool borrar(const T &dato){
            typename list<T>::iterator it;
            it=puntos.begin();
            for(;it!=puntos.end();it++) {
                if (*it == dato) {
                    puntos.erase(it);
                    return true;
                }
            }
            return false;
        };
        unsigned get_Tam(){return puntos.size();}
        typename list<T>::iterator begin(){
            return puntos.begin();
        }
        typename list<T>::iterator end(){
            return puntos.end();
        }
    };
    float xMin, yMin, xMax, yMax;
    float tamaCasillaX,tamaCasillaY;
    unsigned taml;
    vector<vector<Casilla>> mr;
    Casilla *obtenerCasilla(float x,float y);

public:
    MallaRegular(){};
    MallaRegular(float aXMin,float aYMin, float aXMax, float aYMax, int nDiv);
    void insertar(float x,float y,const T &dato);
    T *buscar(float x,float y, const T &dato);
    bool borrar(float x, float y, const T &dato);

    vector<T> buscarRadio(float xcentro,float ycentro,float radio);
    unsigned numElementos(){return taml;};
    unsigned maxElementosPorCelda();
    float promedioElementosPorCelda();

};


template<typename T>
MallaRegular<T>::MallaRegular(float aXMin, float aYMin, float aXMax, float aYMax, int nDiv):xMin(aXMin),yMin(aXMin),xMax(aXMax),yMax(aYMax){
    tamaCasillaX=(xMax-xMin)/nDiv;
    tamaCasillaY=(yMax-yMin)/nDiv;
    mr.insert(mr.begin(),nDiv,vector<Casilla>(nDiv));
}


template<typename T>
typename MallaRegular<T>::Casilla *MallaRegular<T>::obtenerCasilla(float x, float y) {
    int i=(x-xMin)/tamaCasillaX;
    int j=(y-yMin)/tamaCasillaY;
    return &mr[i][j];
}


template<typename T>
void MallaRegular<T>::insertar(float x, float y, const T &dato) {
    Casilla *c= obtenerCasilla(x,y);
    c->insertar(dato);
    taml++;
}


template<typename T>
bool MallaRegular<T>::borrar(float x, float y, const T &dato) {
    Casilla *c= obtenerCasilla(x,y);
    return c->borrar(dato);
}


template<typename T>
vector<T> MallaRegular<T>::buscarRadio(float xcentro, float ycentro, float radio) {

    vector<T> vRango;

    double lonMin,lonMax,latMin,latMax;

    lonMin=xcentro-radio;
    lonMax=xcentro+radio;
    latMin=ycentro-radio;
    latMax=ycentro+radio;
    int t=0;

    int max=0;
    for(double i=lonMin;i<=lonMax;i=i+tamaCasillaX){
        for (double j=latMin;j<=latMax;j=j=tamaCasillaY){
            typename list<T>::iterator itAux;
            if(j>=yMin && j<=yMax && i>=xMin && i<=yMax){
                Casilla *c= obtenerCasilla(i,j);
                for(itAux=c->begin() ;itAux!=c->end();itAux++){
                    float dist;
                    dist= std::distance(&xcentro,&ycentro);
                    if(dist<=radio){
                        t++;
                        vRango.push_back(*itAux);                    }
                }
            }
        }
    }
    return vRango;
}


template<typename T>
unsigned MallaRegular<T>::maxElementosPorCelda() {
    unsigned maximo=0;
    for(float i=xMin;i<xMax;i=i+tamaCasillaX){
        for(float j=yMin;j<yMax;j=j+tamaCasillaY){
        Casilla *c= obtenerCasilla(i,j);
        int tam=c->get_Tam();
        if (tam>maximo)
            maximo=tam;
        }
    }
    return maximo;
}


template<typename T>
float MallaRegular<T>::promedioElementosPorCelda() {
    int NoVacio=0;
    for(float i=xMin;i<xMax;i=i+tamaCasillaX){
        for(float j=yMin;j<yMax;j=j+tamaCasillaY){
            Casilla *c= obtenerCasilla(i,j);
            if(c->get_Tam()!=0)
                NoVacio++;
        }
    }
    float ret=numElementos()/NoVacio;
    return ret;
}

#endif MALLAREGULAR_H