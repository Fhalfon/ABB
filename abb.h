#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

/* *****************************************************************
 *                 Definición estructura de datos                  *
 * *****************************************************************/

typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb_iter abb_iter_t;

/* *****************************************************************
 *                 Primitivas del ABB                              *
 * *****************************************************************/

// Crea el ABB. En caso de que no lo pueda crear devuelve NULL
// Pre: la funcion cmp no puede ser NULL.
// Post: devuelve un ABB vacío con su funcion de comparar y de destrucción de dato
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Almacena un dato en el ABB. Si ya se encuentra la clave, se reemplaza
// con el dato nuevo y se libera el viejo.
// Pre: el ABB fue creado, clave es distinto de NULL.
// Post: devuelve true al almacenar con éxito, o false en caso de error.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra la clave almacenada en el ABB y devuelve el dato almacenado.
// Pre: el ABB fue creado, clave es distinto de NULL.
// Post: devuelve el dato almacenado o NULL si la clave no pertenece.
void *abb_borrar(abb_t *arbol, const char *clave);

// Devuelve el dato almacenado con la clave recibida.
// Pre: el ABB fue creado, clave es distinto de NULL.
// Post: devuelve el dato almacenado, o devuelve NULL si la clave no pertenece.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Devuelve true si la clave provista pertenece al ABB.
// Pre: el ABB fue creado, clave es distinto de NULL.
// Post: devuelve true de encontrar el nodo, o false en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Pre: el ABB fue creado.
// Post: devuelve la cantidad de elementos del ABB.
size_t abb_cantidad(abb_t *arbol);

// Destruye el ABB
// Post: el ABB fue destruido.
void abb_destruir(abb_t *arbol);

/* *****************************************************************
 *                 Primitivas del iterador interno                 *
 * *****************************************************************/

// Pre: el ABB fue creado y visitar debe ser válida
// Post: recorre cada uno de los elementos del ABB
// hasta que se termine o hasta que visitar devuelva false.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* *****************************************************************
 *                 Primitivas del iterador externo                 *
 * *****************************************************************/

// Crea un iterador in-order del ABB.
// Pre: el ABB fue creado.
// Post: devuelve un iterador situado en el primer elemento.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza el iterador al siguiente elemento del ABB según in-order.
// Devuelve verdadero en caso de que pueda avanzar y falso en caso contrario.
// Pre: el iterador fue creado.
// Post: el iterador está situado un elemento más adelante.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave del elemento donde está situado el iterador.
// Pre: el iterador fue creado.
// Post: devuelve la clave de la posición actual o NULL si el iterador está al final.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Permite saber si el iterador se encuentra al final.
// Pre: el iterador fue creado.
// Post: devuelve verdadero en caso de que esté al final, falso en caso contrario.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador.
// Post: el iterador fue destruido.
void abb_iter_in_destruir(abb_iter_t* iter);

/* *****************************************************************
 *                 Pruebas de la implementación                    *
 * *****************************************************************/

// Ejecuta pruebas unitarias sobre la implementación.
void pruebas_abb_alumno(void);

#endif // ABB_H
