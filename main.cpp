#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include "MediExpress.h"
#include "Usuario.h"
#include "img.h"


// Definición de archivos
const std::string ARCHIVO_MEDS = "data/pa_medicamentos.csv";
const std::string ARCHIVO_LABS = "data/laboratorios.csv";
const std::string ARCHIVO_FARMA = "data/farmacias-coord.csv"; // CSV Nuevo con coordenadas
const std::string ARCHIVO_USERS = "data/usuarios.csv";

int main() {
    // -------------------------------------------------------------------------
    // INICIALIZACIÓN
    // -------------------------------------------------------------------------
    MediExpress medi(ARCHIVO_MEDS, ARCHIVO_LABS, ARCHIVO_FARMA, ARCHIVO_USERS);
    std::cout << std::endl;

    // -------------------------------------------------------------------------
    // EJERCICIO 1: JAEN
    // -------------------------------------------------------------------------
    std::cout << "EJERCICIO 1: COMPRAR MEDICAMENTOS EN JAEN" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::vector<Usuario*> usersJaen;
    for (auto& par : medi.getUsuarios()) {
        if (par.second->getProvincia() == "Jaen") {
            usersJaen.push_back(par.second);
        }
    }
    std::cout << "El numero de usuarios de Jaen es: " << usersJaen.size() << std::endl << std::endl;

    std::vector<std::string> listaMedicamentos = {
        "MAGNESIO CLORURO HEXAHIDRATO",
        "LIDOCAINA HIDROCLORURO",
        "MENTA PIPERITA"
    };

   int contador = 0;
    for (Usuario* u : usersJaen) {
        std::string nombreMed = listaMedicamentos[contador % 3];
        contador++;

        std::vector<Farmacia*> cercanas = u->getFarmaciaCercana(medi.getMalla(), 1);
        if (cercanas.empty()) continue;
        Farmacia* fCercana = cercanas[0];

        std::vector<PaMedicamento*> medsEncontrados = medi.buscarCompuesto(nombreMed);
        if (medsEncontrados.empty()) continue;
        PaMedicamento* medAComprar = medsEncontrados[0];

        std::cout << u->getNombre() << " (E" << u->getId() << ")" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "La farmacia mas cercana al usuario " << u->getId()
                  << " (" << u->getLon() << ", " << u->getLat() << ") es la que se encuentra en "
                  << fCercana->getLocalidad() << " (" << fCercana->getX() << ", " << fCercana->getY() << ")" << std::endl;

        // --- CAMBIO AQUÍ: MOSTRAR STOCK ANTES Y DESPUÉS ---

        // 1. Consultar el stock ANTES de intentar comprar
        int stockAntes = fCercana->consultarStock(medAComprar->get_id_num());

        // 2. Realizar la compra
        u->comprarMedicam(medAComprar, 1, fCercana);

        // 3. Consultar el stock DESPUÉS de comprar
        int stockDespues = fCercana->consultarStock(medAComprar->get_id_num());

        // Lógica de impresión usando los valores reales consultados
        if (stockAntes > 0) {
            std::cout << "El usuario " << u->getId() << " quiere comprar " << nombreMed
                      << " y se le ofrece " << medAComprar->get_nombre() << std::endl;

            // Aquí mostramos ambos valores para que quede claro
            std::cout << "El usuario compra una unidad de " << medAComprar->get_nombre()
                      << " (Stock previo: " << stockAntes << ")"
                      << " quedando en stock: " << stockDespues << " unidades" << std::endl;
        } else {
            std::cout << "La farmacia no dispensa " << nombreMed << "." << std::endl;
            std::cout << "Se procede a suministrar la farmacia con 10 unidades de " << nombreMed << std::endl;
            medi.suministrarFarmacia(*fCercana, medAComprar->get_id_num(), 10);
        }
        std::cout << std::endl;
    }

    // -------------------------------------------------------------------------
    // EJERCICIO 2: SEVILLA
    // -------------------------------------------------------------------------
    std::cout << "EJERCICIO 2: COMPRAR MEDICAMENTOS EN SEVILLA" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::vector<Usuario*> usersSevilla;
    for (auto& par : medi.getUsuarios()) {
        if (par.second->getProvincia() == "Sevilla") {
            usersSevilla.push_back(par.second);
        }
    }
    std::cout << "El numero de usuarios de Sevilla es: " << usersSevilla.size() << std::endl << std::endl;

    for (Usuario* u : usersSevilla) {
        std::vector<Farmacia*> cercanas = u->getFarmaciaCercana(medi.getMalla(), 1);
        if (cercanas.empty()) continue;
        Farmacia* fCercana = cercanas[0];

        std::vector<PaMedicamento*> posiblesMeds = medi.buscarCompuesto("MAGNESIO");

        bool compraExitosa = false;
        PaMedicamento* medComprado = nullptr;

        for (PaMedicamento* med : posiblesMeds) {
            if (fCercana->consultarStock(med->get_id_num()) > 0) {
                u->comprarMedicam(med, 1, fCercana);
                medComprado = med;
                compraExitosa = true;
                break;
            }
        }

        std::cout << u->getNombre() << " (E" << u->getId() << ")" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "La farmacia mas cercana al usuario " << u->getId()
                  << " (" << u->getLon() << ", " << u->getLat() << ") es la que se encuentra en "
                  << fCercana->getLocalidad() << " (" << fCercana->getX() << ", " << fCercana->getY() << ")" << std::endl;

        if (compraExitosa) {
            int stock = fCercana->consultarStock(medComprado->get_id_num());
            std::cout << "El usuario " << u->getId() << " quiere comprar MAGNESIO y se le ofrece "
                      << medComprado->get_nombre() << std::endl;
            std::cout << "El usuario compra una unidad de " << medComprado->get_nombre()
                      << " quedando en stock: " << stock << std::endl;
        } else {
            std::cout << "La farmacia no dispensa ningun tipo de MAGNESIO." << std::endl;
            std::cout << "Se procede a suministrar la farmacia con 10 unidades de MAGNESIO OXIDO" << std::endl;

            std::vector<PaMedicamento*> magOxido = medi.buscarCompuesto("MAGNESIO OXIDO");
            if (!magOxido.empty()) {
                medi.suministrarFarmacia(*fCercana, magOxido[0]->get_id_num(), 10);
            }
        }
        std::cout << std::endl;
    }

    // -------------------------------------------------------------------------
    // EJERCICIO 3: BORRAR BISMUTO EN MADRID
    // -------------------------------------------------------------------------
    std::cout << "EJERCICIO 3: BORRAR BISMUTO EN MADRID" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::vector<PaMedicamento*> medsBismuto = medi.buscarCompuesto("BISMUTO");
    std::vector<Usuario*> usersMadrid;
    for (auto& par : medi.getUsuarios()) {
        if (par.second->getProvincia() == "MADRID") usersMadrid.push_back(par.second);
    }

    for (Usuario* u : usersMadrid) {
        std::vector<Farmacia*> top3 = u->getFarmaciaCercana(medi.getMalla(), 3);
        bool compro = false;
        for (Farmacia* f : top3) {
            if (f->getLocalidad().find("MADRID") != std::string::npos) {
                for (PaMedicamento* med : medsBismuto) {
                    if (f->consultarStock(med->get_id_num()) > 0) {
                        std::cout << "El usuario " << u->getId() << " quiere comprar BISMUTO" << std::endl;
                        std::cout << "Entra a la farmacia " << f->getNombre()
                                  << " en " << f->getLocalidad() << std::endl;

                        std::cout << "La farmacia tiene " << f->consultarStock(med->get_id_num())
                                  << " unidades en stock de " << med->get_nombre() << std::endl;

                        u->comprarMedicam(med, 1, f);

                        std::cout << "El usuario compra una unidad de " << med->get_nombre() << std::endl;
                        std::cout << "Unidades en stock de " << med->get_nombre() << ": "
                                  << f->consultarStock(med->get_id_num()) << std::endl << std::endl;

                        compro = true;
                        break;
                    }
                }
            }
            if (compro) break;
        }
    }

    std::cout << "Se va a proceder a eliminar el BISMUTO..." << std::endl;
    for (PaMedicamento* med : medsBismuto) {
        std::string nombre = med->get_nombre();
        if (medi.eliminarMedicamento(med->get_id_num())) {
            std::cout << "El medicamento " << nombre << " se ha eliminado" << std::endl;
        }
    }
    std::cout << std::endl;

    // -------------------------------------------------------------------------
    // EJERCICIO 4: PAREJAS (NUEVA FARMACIA)
    // -------------------------------------------------------------------------
    std::cout << "EJERCICIO 4: PAREJAS" << std::endl;
    std::cout << "==========================================" << std::endl;

    std::cout << "Farmacias que hay en Jaen:" << std::endl;

    // CREAR NUEVA FARMACIA CON 9 ARGUMENTOS
    Farmacia* nuevaF = new Farmacia("12345678A", "JAEN", "JAEN", "FARMACIA NUEVA",
                                    "PASEO DE ESPANA 35", "23009", "37.78710", "-3.79104", &medi);

    medi.getMalla()->insertar(nuevaF->getX(), nuevaF->getY(), nuevaF);

    std::vector<PaMedicamento*> magnesios = medi.buscarCompuesto("MAGNESIO");
    for (PaMedicamento* med : magnesios) {
        nuevaF->nuevoStock(med, 10);
    }

    std::cout << "Medicamentos que dispensa la nueva farmacia de Jaen:" << std::endl;
    for (PaMedicamento* med : magnesios) {
        std::cout << med->get_nombre() << " - stock: " << nuevaF->consultarStock(med->get_id_num()) << std::endl;
    }
    std::cout << std::endl;

    Usuario* masCercano = nullptr;
    double distMin = 99999999.0;
    for (auto& par : medi.getUsuarios()) {
        Usuario* u = par.second;
        double dx = u->getLon() - nuevaF->getX();
        double dy = u->getLat() - nuevaF->getY();
        double dist = std::sqrt(dx*dx + dy*dy);
        if (dist < distMin) {
            distMin = dist;
            masCercano = u;
        }
    }

    if (masCercano) {
        std::cout << "El usuario mas cercano a la nueva farmacia de Jaen es: " << masCercano->getId() << std::endl << std::endl;

        std::vector<PaMedicamento*> magOx = medi.buscarCompuesto("MAGNESIO OXIDO");
        if (!magOx.empty()) {
            masCercano->comprarMedicam(magOx[0], 3, nuevaF);
        }
    }

    std::cout << "Stock de la nueva farmacia de Jaen despues de comprar:" << std::endl;
    for (PaMedicamento* med : magnesios) {
        std::cout << med->get_nombre() << " - stock: " << nuevaF->consultarStock(med->get_id_num()) << std::endl;
    }

    delete nuevaF;

    // -------------------------------------------------------------------------
    // EJERCICIO VOLUNTARIO: VISUALIZACIÓN 2D
    // -------------------------------------------------------------------------
    std::cout << "EJERCICIO VOLUNTARIO: Generando mapa de visualizacion..." << std::endl;
    std::cout << "======================================================" << std::endl;

    // 1. Calcular los límites geográficos  de todas las farmacias
    double minLat = 90.0, maxLat = -90.0;
    double minLon = 180.0, maxLon = -180.0;


    auto& todasFarmacias = medi.getFarmacias();

    if (todasFarmacias.empty()) {
        std::cout << "No hay farmacias para visualizar." << std::endl;
    } else {
        for (auto& par : todasFarmacias) {
            Farmacia& f = par.second;
            // Farmacia usa getX() para longitud y getY() para latitud
            double lat = f.getY();
            double lon = f.getX();

            if (lat < minLat) minLat = lat;
            if (lat > maxLat) maxLat = lat;
            if (lon < minLon) minLon = lon;
            if (lon > maxLon) maxLon = lon;
        }

        // 2. Configuración de la imagen
        int ancho = 600;
        int alto = 600;
        RGBColor blanco(255, 255, 255);
        Img img(alto, ancho, blanco); // (filas, columnas)

        // 3. Calcular factores de conversión (Píxeles por Grado)

        double rangoLat = maxLat - minLat;
        double rangoLon = maxLon - minLon;

        // Evitar división por cero si solo hay un punto
        if (rangoLat == 0) rangoLat = 1.0;
        if (rangoLon == 0) rangoLon = 1.0;

        double pixelPorGradoLat = (double)(alto - 1) / rangoLat;
        double pixelPorGradoLon = (double)(ancho - 1) / rangoLon;

        // 4. Dibujar Farmacias (En AZUL)

        for (auto& par : todasFarmacias) {
            Farmacia& f = par.second;

            // Transformación de coordenadas:

            int px = (int)((f.getX() - minLon) * pixelPorGradoLon);
            int py = alto - 1 - (int)((f.getY() - minLat) * pixelPorGradoLat);

            // Verificamos límites por seguridad
            if (px >= 0 && px < ancho && py >= 0 && py < alto) {
                img.pintarPixel(px, py, 0, 0, 255);

            }
        }

        // 5. Dibujar Usuarios (En ROJO)

        for (auto& par : medi.getUsuarios()) {
            Usuario* u = par.second;


            int px = (int)((u->getLon() - minLon) * pixelPorGradoLon);
            int py = alto - 1 - (int)((u->getLat() - minLat) * pixelPorGradoLat);

            if (px >= 0 && px < ancho && py >= 0 && py < alto) {

                img.pintarPixelGrande(px, py, 255, 0, 0);
            }
        }

        // 6. Guardar la imagen
        try {
            std::string nombreFichero = "mapaResultado.ppm";
            img.guardar(nombreFichero);
            std::cout << "Imagen guardada correctamente como: " << nombreFichero << std::endl;
            std::cout << "Dimensiones: " << ancho << "x" << alto << " pixeles." << std::endl;
            std::cout << "Rango Latitud: [" << minLat << ", " << maxLat << "]" << std::endl;
            std::cout << "Rango Longitud: [" << minLon << ", " << maxLon << "]" << std::endl;
        } catch (ErrorEscrituraFichero& e) {
            std::cout << "Error critico al guardar el fichero de imagen." << std::endl;
        }
    }
    return 0;
}