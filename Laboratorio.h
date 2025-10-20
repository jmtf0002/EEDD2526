//
// Created by Josema on 20/10/2025.
//

#ifndef LABORATORIO_H
#define LABORATORIO_H

#include <iostream>

class Laboratorio {
  private:
    int id;
    std::string nombreLaboratorio;
    std::string direccion;
    std::string codPostal;
    std::string localidad;
  public:
    Laboratorio(int id, std::string nombreLaboratorio, std::string direccion, std::string codPostal, std::string localidad){
      this->id = id;
      this->nombreLaboratorio = nombreLaboratorio;
      this->direccion = direccion;
      this->codPostal = codPostal;
      this->localidad = localidad;
    }

    int getId(){
      return id;
    }
    std::string getNombreLaboratorio(){
      return nombreLaboratorio;
    }
    std::string getDireccion(){
      return direccion;
    }
    std::string getCodPostal(){
      return codPostal;
    }
    std::string getLocalidad(){
      return localidad;
    }


};



#endif //LABORATORIO_H
