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

/**  @author Javier Martínez González mgg00000@red.ujaen.es
     @author Jose María Torraleja Franco jmtf0002@red.ujaen.es */


#include <vector> // Necesario para std::vector
#include <vector> // Necesario para std::vector
#include <string>   // Necesario para std::string
#include <sstream>  // Necesario para std::stringstream

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

        // it.dato() devuelve un Laboratorio*, por lo que usamos ->
        Laboratorio* lab_ptr = it.dato();
        std::cout << "   - ID: " << lab_ptr->getId()
                  << ", Nombre: " << lab_ptr->getNombreLaboratorio()
                  << ", Localidad: " << lab_ptr->getLocalidad() << std::endl;

        it.siguiente();
        contador++;
    }
}
int main(int argc, const char * argv[]) {
    ListaEnlazada<int> lista;

    // 1) Insertar al final 101..200
    for (int i = 101; i <= 200; ++i) {
        int v = i;
        lista.insertarFinal(v);
    }

    std::cout << "Después de insertar 101..200 al final:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 2) Insertar por el comienzo 98..1 (decreciente)
    for (int i = 98; i >= 1; --i) {
        int v = i;
        lista.insertarInicio(v);
    }

    std::cout << "Después de insertar 98..1 al inicio:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 3) Insertar 100 delante del 101
    {
        auto it = lista.iteradorInicio();
        bool found = false;
        while (it.haySiguiente()) {
            if (it.dato() == 101) { found = true; break; }
            it.siguiente();
        }
        if (found) {
            int val100 = 100;
            lista.insertarDelante(it, val100);
        } else {
            std::cerr << "No se encontró 101 para insertar delante.\n";
        }
    }

    std::cout << "Después de insertar 100 delante de 101:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 4) Insertar 99 detrás del 98
    {
        auto it = lista.iteradorInicio();
        bool found = false;
        while (it.haySiguiente()) {
            if (it.dato() == 98) { found = true; break; }
            it.siguiente();
        }
        if (found) {
            int val99 = 99;
            lista.insertarDetras(it, val99);
        } else {
            std::cerr << "No se encontró 98 para insertar detrás.\n";
        }
    }

    std::cout << "Después de insertar 99 detrás de 98:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 5) Borrar los 10 primeros y los 10 últimos
    for (int i = 0; i < 10; ++i) lista.borrarInicio();
    for (int i = 0; i < 10; ++i) lista.borrarFinal();

    std::cout << "Después de borrar 10 primeros y 10 últimos:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }

    // 6) Borrar todos los múltiplos de 10
    {
        auto it = lista.iteradorInicio();
        while (it.haySiguiente()) {
            int val = it.dato();
            if (val % 10 == 0) {
                ListaEnlazada<int>::Iterador toDel = it;
                it.siguiente();
                lista.borrar(toDel);
            } else {
                it.siguiente();
            }
        }
    }

    std::cout << "Después de borrar múltiplos de 10:\n";
    {
        auto it = lista.iteradorInicio();
        for (int i = 0; i < lista.tam(); ++i) {
            std::cout << it.dato() << " ";
            it.siguiente();
        }
        std::cout << "\n\n";
    }


  // El constructor carga y enlaza todo automáticamente usando punteros
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


    // 4. Laboratorios que suministran "ACEITES" (sin duplicados)
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
        std::cout << "\n>> ✅ Proceso de reasignacion completado con exito." << std::endl;
    } else {
        std::cout << "\n>> ❌ ATENCION: Aun quedan medicamentos sin asignar." << std::endl;
    }

    return 0; // Al salir de
}
