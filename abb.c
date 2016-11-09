#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "abb.h"
#include "pila.h"

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

typedef struct abb_iter {
	pila_t *pila;
} abb_iter_t;

/* *****************************************************************
 *                    Funciones auxiliares                         *
 * *****************************************************************/

/* Crea un nodo para el ABB. Copia la clave. Si falla devuelve NULL */
static abb_nodo_t *nodo_crear(const char *clave, void *dato, abb_nodo_t *izq, abb_nodo_t *der)
{
	abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));

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
static void destruir_nodos(abb_nodo_t *nodo, void destruir_dato(void *))
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

/* Devuelve el nodo que tiene la clave igual a la clave dada, comparando con la función recibida
 * Si no lo encuentra devuelve NULL */
static abb_nodo_t *buscar_nodo(abb_nodo_t *nodo, const char *clave, abb_comparar_clave_t cmp)
{
	if (!nodo)
        return NULL;
	else if (cmp(clave, nodo->clave) == 0)
		return nodo;
	else if (cmp(clave, nodo->clave) < 0)
		return buscar_nodo(nodo->izq, clave, cmp);
	else
        return buscar_nodo(nodo->der, clave, cmp);
}

/* Inserta un nodo */
static abb_nodo_t *insertar_nodo(abb_nodo_t *nodo, abb_nodo_t *nuevo, abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
    if (!nodo) {
        return nuevo;
    } else if (cmp(nuevo->clave, nodo->clave) > 0) {
        nodo->der = insertar_nodo(nodo->der, nuevo, cmp, destruir_dato);
        return nodo;
    } else if (cmp(nuevo->clave, nodo->clave) < 0) {
        nodo->izq = insertar_nodo(nodo->izq, nuevo, cmp, destruir_dato);
        return nodo;
    } else {
        /* La clave pertenece al ABB, reemplazo el dato */
        void *aux = nodo->dato;
        nodo->dato = nuevo->dato;
        if (destruir_dato) {
            destruir_dato(aux);
        }
        /* No necesito las otras partes del nodo creado, las libero */
        free(nuevo->clave);
        free(nuevo);
        return nodo;
    }
}

/* Busca el nodo con la mayor clave en el subárbol dado, lo devuelve a través de maximo */
static abb_nodo_t *buscar_maximo(abb_nodo_t *actual, abb_nodo_t **maximo)
{
    if (!actual) {
        *maximo = NULL;
        return NULL;
    } else if (actual->der) {
        /* No estoy en el máximo todavía */
        actual->der = buscar_maximo(actual->der, maximo);
        return actual;
    } else {
        /* actual->der == NULL, actual es el máximo */
        *maximo = actual;
        return actual->izq;
    }
}

/* Busca el nodo que debe borrar, lo borra y engancha el reemplazo con los hijos que tenía el nodo borrado */
static abb_nodo_t *buscar_nodo_borrar(abb_nodo_t *actual, abb_comparar_clave_t cmp, const char *clave, abb_nodo_t **nodo_salida)
{
    if (!actual) {
        *nodo_salida = NULL;
        return NULL;
    } else if (cmp(clave, actual->clave) < 0) {
        actual->izq = buscar_nodo_borrar(actual->izq, cmp, clave, nodo_salida);
        return actual;
    } else if (cmp(clave, actual->clave) > 0) {
        actual->der = buscar_nodo_borrar(actual->der, cmp, clave, nodo_salida);
        return actual;
    } else {
        /* clave == actual->clave, actual es el nodo a borrar */
        abb_nodo_t *reemplazo;
        abb_nodo_t *reemplazo_izq;

        *nodo_salida = actual;
        if (actual->izq) {
            /* Tomamos como reemplazo al máximo del subárbol izquierdo */
            reemplazo_izq = buscar_maximo(actual->izq, &reemplazo);
        } else {
            /* Si no hay árbol izquierdo, unimos al padre con el subárbol derecho */
            return actual->der;
        }
        if (!reemplazo) {
            return NULL;
        }
        /* Si el reemplazo no es NULL (el borrado no es una hoja) */
        /* Debo "salvar" a sus hijos */
        reemplazo->izq = reemplazo_izq;
        reemplazo->der = actual->der;
        return reemplazo;
    }
}

/* Recorre los nodos en in-order recursivamente, comunicando con el valor de retorno en cada llamado si debe seguir la recursión */
static bool abb_nodo_in_order(abb_nodo_t *nodo, bool visitar(const char *, void *, void *), void *extra)
{
    if (!nodo)
        return true; // Recorrió todo, porque no hay nada para recorrer
    else if (!abb_nodo_in_order(nodo->izq,visitar,extra))
        return false; // Si el subárbol izquierdo recibió false, devuelve false
	else if (!visitar(nodo->clave,nodo->dato,extra))
        return false; // Visita el nodo actual, comunica a las llamadas previas que deben terminar
    else if (!abb_nodo_in_order(nodo->der,visitar,extra))
        return false; // Si el subárbol derecho recibió false, devuelve false
    else
        return true;
}

/* Apila todos los nodos en in-order en una pila válida dada */
static void apilar_nodos_in(pila_t *pila, abb_nodo_t *nodo)
{
    if (!nodo) {
        return;
    }
    /* El orden en el que hay que apilar es el inverso al in-order
     * por ser una pila. Primero todo el subarbol derecho y luego el izquierdo */
    apilar_nodos_in(pila, nodo->der);
    pila_apilar(pila, nodo);
    apilar_nodos_in(pila, nodo->izq);
}

/* *****************************************************************
 *                    Primitivas del ABB                           *
 * *****************************************************************/

abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
	abb_t *arbol = malloc(sizeof(abb_t));

	if (!arbol) {
        return NULL;
    }
	arbol->raiz = NULL;
	arbol->cantidad = 0;
	arbol->cmp = cmp;
	arbol->destruir_dato = destruir_dato;
	return arbol;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato)
{
	abb_nodo_t *nuevo;

    nuevo = nodo_crear(clave, dato, NULL, NULL);
	if (!nuevo) {
        return false;
    }
    arbol->raiz = insertar_nodo(arbol->raiz, nuevo, arbol->cmp, arbol->destruir_dato);
    ++(arbol->cantidad);
	return true;
}

void *abb_borrar(abb_t *arbol, const char *clave)
{
    abb_nodo_t *borrado;
    void *dato_salida;

	arbol->raiz = buscar_nodo_borrar(arbol->raiz, arbol->cmp, clave, &borrado);
    if (!borrado) {
        return NULL;
    }
    dato_salida = borrado->dato;
    --(arbol->cantidad);
    free(borrado->clave);
    free(borrado);
	return dato_salida;
}

void *abb_obtener(const abb_t *arbol, const char *clave)
{
	abb_nodo_t *nodo_salida;

    nodo_salida = buscar_nodo(arbol->raiz, clave, arbol->cmp);
    if (!nodo_salida)
        return NULL;
    else
        return nodo_salida->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave)
{
	abb_nodo_t *nodo_salida;

    nodo_salida = buscar_nodo(arbol->raiz, clave, arbol->cmp);
    if (!nodo_salida)
        return false;
    else
        return true;
}

size_t abb_cantidad(abb_t *arbol)
{
	return arbol->cantidad;
}

void abb_destruir(abb_t *arbol)
{
    if (!arbol) return;
	if (arbol->raiz) {
        destruir_nodos(arbol->raiz, arbol->destruir_dato);
    }
	free(arbol);
}

/* *****************************************************************
 *                 Primitivas del iterador interno                 *
 * *****************************************************************/

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra)
{
    abb_nodo_in_order(arbol->raiz,visitar,extra);
}

/* *****************************************************************
 *                 Primitivas del iterador externo                 *
 * *****************************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol)
{
	abb_iter_t *iter = malloc(sizeof(abb_iter_t));
    pila_t *pila;

	if (!iter) {
	    return NULL;
	}
	pila = pila_crear();
	if (!pila) {
		free(iter);
		return NULL;
	}
	iter->pila = pila;

    /* Apila todos los nodos en in-order */
    apilar_nodos_in(iter->pila, arbol->raiz);
	return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter)
{
	if (abb_iter_in_al_final(iter))	{
		return false;
	}
	pila_desapilar(iter->pila);
	return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter)
{
    abb_nodo_t *tope;

    if (abb_iter_in_al_final(iter)) {
		return NULL;
	}
	tope = pila_ver_tope(iter->pila);
	return tope->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter)
{
	return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t *iter)
{
    if (!iter) return;
	pila_destruir(iter->pila);
	free(iter);
}
