#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "abb.h"

/* *****************************************************************
 *            Definición de las estructuras de datos               *
 * *****************************************************************/

typedef struct nodo_abb {
	char *clave;
	void *dato;
	struct nodo_abb *izq;
	struct nodo_abb *der;
} abb_nodo_t;

typedef struct abb{
	abb_nodo_t *raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
} abb_t;

/* *****************************************************************
 *                    Funciones auxiliares                         *
 * *****************************************************************/

/* Crea un nodo para el ABB. Copia la clave. Si falla devuelve NULL */
static abb_nodo_t* nodo_crear(const char * clave, void * dato, abb_nodo_t * izq, abb_nodo_t * der)
{
	abb_nodo_t* nodo = malloc(sizeof(abb_nodo_t));

	if (!nodo) {
		return NULL;
    }
	nodo->clave = strdup(clave);
    if (!(nodo->clave)) {
        free(nodo);
        return NULL;
    }
	nodo->dato = dato;
	nodo->izq = izq;
	nodo->der = der;
	return nodo;
}

/* Destruye el nodo recibido y sus hijos. Aplica destruir_dato al dato si es distinto de NULL */
static void destruir_nodos(abb_nodo_t* nodo, void destruir_dato(void *))
{
    if (!nodo) {
        return;
    }
    destruir_nodos(nodo->izq, destruir_dato);
    destruir_nodos(nodo->der, destruir_dato);
    if (destruir_dato) {
        destruir_dato(nodo->dato);
    }
    free(nodo->clave);
    free(nodo);
}

static abb_nodo_t* buscar_nodo(abb_nodo_t* raiz, const char* clave, abb_comparar_clave_t cmp)
{
	if (!raiz)
        return NULL;
	else if (cmp(clave,raiz->clave) == 0)
		return raiz;
	else if (cmp(clave,raiz->clave) < 0)
		return buscar_nodo(raiz->izq, clave, cmp);
	else
        return buscar_nodo(raiz->der, clave, cmp);
}

/* Inserta un nodo */
static abb_nodo_t * insertar_nodo(abb_nodo_t * nodo, abb_nodo_t * nuevo, abb_comparar_clave_t cmp)
{
    if (!nodo) {
        return nuevo;
    } else if (cmp(nuevo->clave, nodo->clave) >= 0) {   // Las claves iguales se ubican hacia la derecha
        nodo->der = insertar_nodo(nodo->der, nuevo, cmp);
        return nodo->der;
    } else {
        nodo->izq = insertar_nodo(nodo->izq, nuevo, cmp);
        return nodo->izq;
    }
}

/* Busca el nodo que va a reemplazar al nodo borrado */
static abb_nodo_t * buscar_reemplazante(abb_nodo_t * actual, abb_nodo_t * reemplazo)
{
    if (!actual) {
        reemplazo = NULL;
        return NULL;
    } else if (actual->der) {
        /* no estoy en el máximo del subárbol izquierdo */
        actual->der = buscar_reemplazante(actual->der, reemplazo);
        return actual;
    } else {
        /* actual->der == NULL, actual es el máximo del subárbol izquierdo */
        /* actual es el nodo reemplazo del borrado */
        reemplazo = actual;
        return actual->izq;
    }
}

/* Busca el nodo que debe borrar, lo borra y engancha el reemplazo con los hijos que tenía el nodo borrado */
static abb_nodo_t * buscar_nodo_borrar(abb_nodo_t * actual, abb_comparar_clave_t cmp, const char * clave, abb_nodo_t * nodo_salida)
{
    if (!actual) {
        nodo_salida = NULL;
        return NULL;
    } else if (cmp(clave, actual->clave) < 0) {
        actual->izq = buscar_nodo_borrar(actual->izq, cmp, clave, nodo_salida);
        return actual;
    } else if (cmp(clave, actual->clave) > 0) {
        actual->der = buscar_nodo_borrar(actual->der, cmp, clave, nodo_salida);
        return actual;
    } else {
        abb_nodo_t * reemplazo;
        abb_nodo_t * reemplazo_izq;

        nodo_salida = actual;
        reemplazo_izq = buscar_reemplazante(actual->izq, reemplazo);
        if (reemplazo) {
            /* Si el reemplazo no es NULL (el borrado no es una hoja) */
            /* Debo "salvar" a sus hijos */
            reemplazo->izq = reemplazo_izq;
            reemplazo->der = actual->der;
        }
        return reemplazo;
    }
}

/* *****************************************************************
 *                    Primitivas del ABB                           *
 * *****************************************************************/

// Crea el ABB en caso de que no lo pueda crear devuelve NULL
// Pre: se deben pasar las funciones cmp destruir_dato. Necesariamente
// la funcion cmp no puede ser NULL.
// Post: devuelve un ABB con su funcion de comparar, destruccion
// y con raiz NULL. En caso de que no pueda crearla devuelve NULL.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
	abb_t* arbol = malloc(sizeof(abb_t));

	if (!arbol) {
        return NULL;
    }
	arbol->raiz = NULL;
	arbol->cantidad = 0;
	arbol->cmp = cmp;
	arbol->destruir_dato = destruir_dato;
	return arbol;
}


// Almacena un dato en el ABB. Si no se encuentra la clave, se crea un nodo, sino
// se reemplaza el valor previo.
// Pre: El ABB fue creado.
// Post: Devuelve true al almacenar con éxito, o false en caso de error.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato)
{
    if (!clave) return NULL; // La clave debe ser válida
	abb_nodo_t* nuevo = nodo_crear(clave, dato, NULL, NULL);
	if (!nuevo) return false;
    arbol->raiz = insertar_nodo(arbol->raiz, nuevo, arbol->cmp);
    ++(arbol->cantidad);
	return true;
}

// Devuelve un dato almacenado en el ABB, y destruye el nodo que lo contiene.
// Pre: El ABB fue creado.
// Post: Devuelve el dato almacenado y destruye el nodo que lo contenía, o devuelve
// NULL cuando no se cumplan ciertas condiciones (Por ej.: clave no pertenece)
void *abb_borrar(abb_t *arbol, const char *clave)
{
    abb_nodo_t * borrado;
    void * dato_salida;

    if (!clave) return NULL; // La clave debe ser válida
	arbol->raiz = buscar_nodo_borrar(arbol->raiz, arbol->cmp, clave, borrado);
    if (!borrado) {
        return NULL;
    }
    dato_salida = borrado->dato;
    --(arbol->cantidad);
    free(borrado->clave);
    free(borrado);
	return dato_salida;
}

// Similar a abb_borrar, solo que en este caso no destruye el nodo, solo devuelve
// el valor, o NULL de no hallarse.
// Pre: El ABB fue creado.
// Post: Devuelve el dato almacenado, o devuelve NULL cuando no se cumplan ciertas
// condiciones (Por ej.: clave no pertenece)
void *abb_obtener(const abb_t *arbol, const char *clave)
{
    if (!clave) return NULL; // La clave debe ser válida
	abb_nodo_t * nodo_salida = buscar_nodo(arbol->raiz, clave, arbol->cmp);
    if (!nodo_salida)
        return NULL;
    else
        return nodo_salida->dato;
}

// Averigua si existe un nodo en el ABB con la clave provista.
// Pre: El ABB fue creado.
// Post: Devuelve true de encontrar el nodo, o false en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave)
{
    if (!clave) return NULL; // La clave debe ser válida
	abb_nodo_t * nodo_salida = buscar_nodo(arbol->raiz, clave, arbol->cmp);
    if (!nodo_salida)
        return false;
    else
        return true;
}

// Funcion que permite saber la cantidad de elemtos del arbol.
// Post: devuelve la cantidad de elemento del arbol.
size_t abb_cantidad(abb_t *arbol)
{
	return arbol->cantidad;
}

// Destruye el arbol previamente creado.
// Pre: el arbol debe existir.
// Post: destruye el arbol.
void abb_destruir(abb_t *arbol)
{
	if (arbol->raiz) {
        destruir_nodos(arbol->raiz, arbol->destruir_dato);
    }
	free(arbol);
}

/* *****************************************************************
 *                 Primitivas del iterador interno                 *
 * *****************************************************************/
/* *****************************************************************
 *                 Primitivas del iterador externo                 *
 * *****************************************************************/
