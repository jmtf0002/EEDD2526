#ifndef STOCK_H
#define STOCK_H

/**
 * @class Stock
 * @brief Almacena la cantidad (stock) de un medicamento específico (por ID) en una farmacia.
 *
 * Esta clase es el núcleo de la gestión de inventario para la Práctica 4.
 * Incluye operadores de comparación para ser usada en contenedores STL como std::set.
 */
class Stock {
private:
    int id_PaMed;
    int num_stock;

public:
    /**
     * @brief Constructor para búsquedas.
     * Crea un objeto Stock "vacío" solo con el ID.
     * Se usa para buscar y borrar en el std::set
     * @param id ID del medicamento a buscar.
     */
    Stock(int id);

    /**
     * @brief Constructor principal.
     * Crea un objeto Stock con un ID y una cantidad inicial.
     * @param id ID del medicamento.
     * @param stock Cantidad inicial de stock.
     */
    Stock(int id, int stock);

    /**
     * @brief Incrementa el número de unidades del stock.
     * @param n Cantidad a añadir.
     */
    void incrementa(int n);

    /**
     * @brief Decrementa el número de unidades del stock.
     * @param n Cantidad a restar.
     */
    void decrementa(int n);

    // --- Getters ---

    /**
     * @brief Obtiene el ID del medicamento asociado.
     * @return ID del PaMedicamento.
     */
    int getIdPaMed() const;

    /**
     * @brief Obtiene el stock actual.
     * @return Número de unidades disponibles.
     */
    int getNumStock() const;

    /**
     * @brief Operador de comparación (menor que).
     * Compara dos objetos Stock basándose únicamente en su id_PaMed.
     * Requerido para que std::set funcione correctamente.
     * @param other El otro objeto Stock con el que comparar.
     * @return true si este ID es menor que el de 'other', false en caso contrario.
     */
    bool operator<(const Stock& other) const;
};

#endif // STOCK_H