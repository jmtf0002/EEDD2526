#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "ListaEnlazada.h"
#include "VDinamico.h"
#include "MediExpress.h"

/**  @author Javier Martínez González jmg00144@red.ujaen.es
     @author Jose María Torraleja Franco jmtf0002@red.ujaen.es */




std::vector<std::string> parsearFilaCSV(const std::string& linea) {
    std::vector<std::string> campos;
    std::stringstream ss(linea);
    std::string campo;

    while (ss.peek() != EOF) {
        if (ss.peek() == '"') {
            ss.get(); // Consumir la comilla inicial
            std::string campo_entre_comillas;

            while (ss.peek() != EOF) {
                char c = ss.get();
                if (c == '"') {
                    if (ss.peek() == '"') {
                        ss.get(); // Consumir la segunda comilla
                        campo_entre_comillas += '"'; // Y añadir una sola al resultado
                    } else {
                        break; // Es la comilla de cierre
                    }
                } else {
                    campo_entre_comillas += c;
                }
            }
            campos.push_back(campo_entre_comillas);
            if (ss.peek() == ';') { ss.get(); }
        } else {
            std::getline(ss, campo, ';');
            campos.push_back(campo);
        }
    }
    if (!linea.empty() && linea.back() == ';') {
        campos.push_back("");
    }
    return campos;
}

void imprimirLaboratorios(const ListaEnlazada<Laboratorio*>& lista, int limite = -1) {
    auto it = lista.iteradorInicio();
    int contador = 0;

    if (lista.tam() == 0) {
        std::cout << "   (No se encontraron resultados)" << std::endl;
        return;
    }

    while (it.haySiguiente()) {
        if (limite != -1 && contador >= limite) break;

        Laboratorio* lab_ptr = it.dato();
        std::cout << "   - ID: " << lab_ptr->getId()
                  << ", Nombre: " << lab_ptr->getNombreLaboratorio()
                  << ", Localidad: " << lab_ptr->getLocalidad() << std::endl;

        it.siguiente();
        contador++;
    }
}
int main(int argc, const char * argv[]) {


    return 0;}
