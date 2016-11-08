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
	pila_t *pila_iter;
} abb_iter_t;

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
static abb_nodo_t * insertar_nodo(abb_nodo_t * nodo, abb_nodo_t * nuevo, abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
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
        void * aux = nodo->dato;
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
static abb_nodo_t * buscar_maximo(abb_nodo_t * actual, abb_nodo_t ** maximo)
{
    if (!actual) {
        *maximo = NULL;
        return NULL;
    } else if (actual->der) {
        /* no estoy en el máximo todavía */
        actual->der = buscar_maximo(actual->der, maximo);
        return actual;
    } else {
        /* actual->der == NULL, actual es el máximo */
        *maximo = actual;
        return actual->izq;
    }
}

/* Busca el nodo que debe borrar, lo borra y engancha el reemplazo con los hijos que tenía el nodo borrado */
static abb_nodo_t * buscar_nodo_borrar(abb_nodo_t * actual, abb_comparar_clave_t cmp, const char * clave, abb_nodo_t ** nodo_salida)
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
        abb_nodo_t * reemplazo;
        abb_nodo_t * reemplazo_izq;

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
	abb_nodo_t* nuevo;

    if (!clave) {
        return NULL; // La clave debe ser válida
    }
    nuevo = nodo_crear(clave, dato, NULL, NULL);
	if (!nuevo) {
        return false;
    }
    arbol->raiz = insertar_nodo(arbol->raiz, nuevo, arbol->cmp, arbol->destruir_dato);
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

    if (!clave) {
        return NULL; // La clave debe ser válida
    }
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

// Similar a abb_borrar, solo que en este caso no destruye el nodo, solo devuelve
// el valor, o NULL de no hallarse.
// Pre: El ABB fue creado.
// Post: Devuelve el dato almacenado, o devuelve NULL cuando no se cumplan ciertas
// condiciones (Por ej.: clave no pertenece)
void *abb_obtener(const abb_t *arbol, const char *clave)
{
	abb_nodo_t * nodo_salida;

    if (!clave) {
        return NULL; // La clave debe ser válida
    }
    nodo_salida = buscar_nodo(arbol->raiz, clave, arbol->cmp);
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
	abb_nodo_t * nodo_salida;

    if (!clave) {
        return NULL; // La clave debe ser válida
    }
    nodo_salida = buscar_nodo(arbol->raiz, clave, arbol->cmp);
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

static bool nodo_abb_in_order(abb_nodo_t *nodo_abb, bool visitar(const char *, void *, void *), void *extra)
{
    // Si el hijo izq no es NULL lo visita.
	// Si la visita del hijo izq es falsa termina.
	if (nodo_abb->izq){
		nodo_abb_in_order(nodo_abb->izq,visitar,extra);
    }
	// Visita el nodo actual.
	// Si la visita es falsa termina.
	if (!visitar(nodo_abb->clave,nodo_abb->dato,extra)){
		return false;
    }
	// Si el hijo der no es NULL lo visita.
	// Si la visita del hijo der es falsa termina.
	if (nodo_abb->der){
		nodo_abb_in_order(nodo_abb->der,visitar,extra);
	}
	return true;
}

// Pre: el arbol existe y se debe mandar una funcion visitar.
// Post: recorre cada uno de los elementos del arbol, segun el
// resultado de la funcion vistar.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra)
{
	if (arbol->raiz) {
		nodo_abb_in_order(arbol->raiz,visitar,extra);
	}
}

/* *****************************************************************
 *                 Primitivas del iterador externo                 *
 * *****************************************************************/

// Avanza lo mas hacia la izquierda posible del padre.
// Pre: la pila del iterador debe existir.
// Post: deja en el tope de la pila el elemento mas al a izq.
static pila_t* avanzar_izq(pila_t* pila_iter,abb_nodo_t* actual)
{
	if (actual) {
		pila_apilar(pila_iter,actual);
		return avanzar_izq(pila_iter,actual->izq);
	}
	return pila_iter;
}

// Crea el iterador del arbol y lo deja posicionado en el
// nodo actual.
// Pre: el arbol debe existir.
// Post: devuelte el iterador en la posicion actual. En caso de
// que el arbol este vacio, el actual apunta a NULL.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol)
{
	abb_iter_t *iter = malloc(sizeof(abb_iter_t));
	if (!iter) {
	    return NULL;
	}
	pila_t *pila = pila_crear();
	if (!pila) {
		free(iter);
		return NULL;
	}
	iter->pila_iter = pila;

	if (arbol->raiz) {
	    iter->pila_iter = avanzar_izq(iter->pila_iter,arbol->raiz);
    }
	return iter;
}

// Permite avanzar en el iterador.
// Pre: el iterador debe existir.
// Post: devuelve verdadero en caso de que pueda avanzar
// y falso en caso de que no.
bool abb_iter_in_avanzar(abb_iter_t *iter)
{
	if (abb_iter_in_al_final(iter))	{
		return false;
	}
	abb_nodo_t *desapilado = pila_desapilar(iter->pila_iter);

	if (desapilado->der){
		pila_apilar(iter->pila_iter,desapilado->der);
		abb_nodo_t* aux = desapilado->der;
		if (aux->izq){
			iter->pila_iter = avanzar_izq(iter->pila_iter,aux->izq);
		}
	}
	return true;
}

// Devuelve la clave actual del iterador.
// Pre: iterador debe existir.
// Post: devuelve la clave almacenada en la posicion actual
// del iterador. En caso de que este al final, devuelte NULL.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter)
{
    if (abb_iter_in_al_final(iter)){
		return NULL;
	}
	abb_nodo_t *tope = pila_ver_tope(iter->pila_iter);
	return tope->clave;

}

// Permite saber si el iterador se encuentra al final.
// Pre: el iterador debe existir.
// Post: devuelve verdadero en caso de que este al final,
// falso en caso de que no este al final.
bool abb_iter_in_al_final(const abb_iter_t *iter)
{
	if (pila_esta_vacia(iter->pila_iter)) {
	    return true;
	}
	return false;
}

// Destruye el iterador.
// Pre: el iterador debe existir.
void abb_iter_in_destruir(abb_iter_t* iter)
{
	pila_destruir(iter->pila_iter);
	free(iter);
}
