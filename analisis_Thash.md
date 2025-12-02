# Análisis de tablas de dispersión

* *Javier Martínez González - jmg00144@red.ujaen.es*
* *Jose María Torraleja Franco - jmtf0002@red.ujaen.es*

***

## Tamaño de tabla: 4871 (para lambda = 0.68)
| función | máximo de colisiones | supera 10 colisiones | factor de carga | promedio de colisiones |
|:---------|:--------------------:|:--------------------:|:---------------:|:----------------------:|
| Cuadratica | 43 | 87 | 0.679532 | 1.36737 |
| Doble 1 | 13 | 3 | 0.679532 | 0.593656 |
| Doble 2 | 14 | 3 | 0.679532 | 0.57281 |

## Tamaño de tabla: 5099 (para lambda = 0.65)
| función | máximo de colisiones | supera 10 colisiones | factor de carga | promedio de colisiones |
|:---------|:--------------------:|:--------------------:|:---------------:|:----------------------:|
| Cuadratica | 38 | 89 | 0.649147 | 1.39607 |
| Doble 1 | 12 | 1 | 0.649147 | 0.544713 |
| **Doble 2** | **10** | **0** | **0.649147** | **0.539275** |

***

## Justificación de la configuración elegida
Hemos optado por utilizar la segunda función de doble dispersión, **Doble 2**, en combinación con el factor de carga $\mathbf{\lambda=0.65}$.

Esta configuración es la más eficiente de las seis pruebas realizadas porque presenta:
* El **menor promedio de colisiones (0.539275)**.
* El **menor máximo de colisiones (10)**.
* **Cero operaciones que superan las 10 colisiones** (`supera 10 colisiones: 0`).

Estos resultados indican una distribución de datos más uniforme y una mayor eficiencia general, lo que es crucial para garantizar un rendimiento óptimo de la tabla hash.

***

## Comparación de tiempos dados en milisegundos

Tiempo en realizar las búsquedas en la Tabla Hash: **0.000 segs.**
Tiempo en realizar las búsquedas en la Lista: **0.062 segs.**

**Análisis de tiempos:**

El tiempo de búsqueda en la **Tabla Hash (0.000 segs.)** es significativamente más rápido que el tiempo de búsqueda en la **Lista (0.062 segs.)**.

Esta diferencia valida que la estructura de datos de tabla hash supera ampliamente en eficiencia a la lista para operaciones de búsqueda, ya que permite una complejidad temporal de $O(1)$ en promedio. La función de dispersión y la configuración elegida han sido adecuadas para asegurar un tiempo de acceso casi nulo.