#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

#include "ListaEnlazada.h"
#include "VDinamico.h"
#include "PaMedicamento.h"
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



  // El constructor carga y enlaza
    MediExpress sistema("data/pa_medicamentos.csv", "data/laboratorios.csv");
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Sistema inicializado. Realizando consultas..." << std::endl;
    std::cout << "------------------------------------------\n" << std::endl;

    // 1. Buscar y mostrar laboratorios en Granada
    std::cout << "1. Laboratorios ubicados en Granada:" << std::endl;
    ListaEnlazada<Laboratorio*> labs_granada = sistema.buscarLabCiudad("Granada");
    imprimirLaboratorios(labs_granada);
    std::cout << "\n------------------------------------------\n" << std::endl;

    // 2. Indicar cuántos laboratorios hay en Jaén
    std::cout << "2. Numero de laboratorios en Jaen:" << std::endl;
    ListaEnlazada<Laboratorio*> labs_jaen = sistema.buscarLabCiudad("Jaen");
    std::cout << "   >> Se han encontrado " << labs_jaen.tam() << " laboratorios." << std::endl;
    std::cout << "\n------------------------------------------\n" << std::endl;

    // 3. Indicar cuántos laboratorios hay en Madrid y mostrar los 10 primeros
    std::cout << "3. Laboratorios en Madrid:" << std::endl;
    ListaEnlazada<Laboratorio*> labs_madrid = sistema.buscarLabCiudad("Madrid");
    std::cout << "   >> Se han encontrado " << labs_madrid.tam() << " laboratorios en total." << std::endl;
    std::cout << "   Mostrando los 10 primeros:" << std::endl;
    imprimirLaboratorios(labs_madrid, 10);
    std::cout << "\n------------------------------------------\n" << std::endl;


    // 4. Laboratorios que suministran "ACEITES"
    std::cout << "4. Laboratorios que suministran productos 'ACEITES':" << std::endl;
    ListaEnlazada<Laboratorio*> labs_aceites = sistema.buscarLabsPorCompuesto("ACEITE");
    imprimirLaboratorios(labs_aceites);



    std::cout << "\n------------------------------------------\n" << std::endl;

    // 5. Asignar medicamentos sin laboratorio a los de Madrid
    std::cout << "5. Reasignacion de medicamentos sin suministrador:" << std::endl;
    std::cout << "   - Medicamentos sin laboratorio (antes): " << sistema.contarMedicamentosSinLab() << std::endl;
    sistema.asignarMedsSinLabAMadrid();
    std::cout << "   - Medicamentos sin laboratorio (despues): " << sistema.contarMedicamentosSinLab() << std::endl;

    if (sistema.contarMedicamentosSinLab() == 0) {
        std::cout << "\n>>  Proceso de reasignacion completado con exito." << std::endl;
    }

    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Iniciando eliminacion de laboratorios..." << std::endl;

    int laboratorios_totales_antes = sistema.totalLaboratorios();
    int medicamentos_sin_lab_antes = sistema.contarMedicamentosSinLab();

    std::cout << " - Laboratorios totales (antes): " << laboratorios_totales_antes << std::endl;
    std::cout << " - Medicamentos sin laboratorio (antes): " << medicamentos_sin_lab_antes << std::endl;

    // Eliminar laboratorios de Bruselas
    int eliminados = sistema.eliminarLabsPorCiudad("Bruselas");

    std::cout << "\n>> Se han eliminado " << eliminados << " laboratorios de Bruselas." << std::endl;

    int laboratorios_totales_despues = sistema.totalLaboratorios();
    int medicamentos_sin_lab_despues = sistema.contarMedicamentosSinLab();

    std::cout << " - Laboratorios totales (despues): " << laboratorios_totales_despues << std::endl;
    std::cout << " - Medicamentos sin laboratorio (despues): " << medicamentos_sin_lab_despues << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    return 0;}
