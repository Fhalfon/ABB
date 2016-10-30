#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "abb.h"

#define IGUALES 0

/* *****************************************************************
 *            Definición de las estructuras de datos               *
 * *****************************************************************/

typedef struct nodo_abb {
	char *clave;
	void *dato;
	struct nodo_abb *izq;
	struct nodo_abb *der;
} nodo_abb_t;

typedef struct abb{
	nodo_abb_t *raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
} abb_t;

/* *****************************************************************
 *                    Funciones auxiliares                         *
 * *****************************************************************/

/* Crea un nodo para el ABB. Copia la clave. Si falla devuelve NULL */
static nodo_abb_t* nodo_crear(const char * clave, void * dato, nodo_abb_t * izq, nodo_abb_t * der)
{
	nodo_abb_t* nodo = malloc(sizeof(nodo_abb_t));

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
static void destruir_nodos(nodo_abb_t* nodo, void destruir_dato(void *))
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

static nodo_abb_t* buscar_nodo(nodo_abb_t* raiz, const char* clave, const abb_t* arbol){

	if (raiz == NULL) return NULL;

	if (arbol->cmp(raiz->clave,clave) == IGUALES){
		return raiz; // CASO BASE
	}

	if ((raiz->der == NULL) && (raiz->izq == NULL)){
		return NULL; // NO ESTA
	}

	if (arbol->cmp(raiz->clave,clave) > IGUALES){
		raiz = raiz->izq;
		return buscar_nodo(raiz,clave,arbol);
	}

	raiz = raiz->der;
	return buscar_nodo(raiz,clave,arbol);

}

// La idea de la funcion es buscar el nodo en donde tenga que insertar
// sea a derecha o izquierda el nodo que me pasan como dato
// devuelvo un puntero para insertarlo alli.
// Uso const por que NO debo modificarlo solo buscar.
static bool buscar_ubicacion_nodo(nodo_abb_t* raiz,nodo_abb_t* nodo, const char *clave,  abb_t* arbol){

	if (arbol->raiz == NULL ) {
		arbol->raiz = nodo;
		arbol->cantidad++;
		return true;
	}


	if (arbol->cmp(raiz->clave,clave) == IGUALES){
		if (arbol->destruir_dato != NULL) arbol->destruir_dato(raiz->dato);
		raiz->dato = nodo->dato;
		free(nodo->clave);
		free(nodo);
		return true;
	}

	if (arbol->cmp(raiz->clave,clave) > IGUALES){
		if (raiz->izq == NULL ){
			raiz->izq = nodo;
			arbol->cantidad++;
			return true;
		}
		return buscar_ubicacion_nodo(raiz->izq,nodo,clave,arbol);
	}
	if (raiz->der == NULL){
		raiz->der = nodo;
		arbol->cantidad++;
		return true;
	}
	return buscar_ubicacion_nodo(raiz->der,nodo,clave,arbol);

	return false;
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
// se reemplaza el valor pevio.
// Pre: El ABB fue creado.
// Post: Devuelve true al almacenar con éxito, o false en caso de error.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato){

	if (arbol == NULL) return false;

	nodo_abb_t* nodo = nodo_crear(clave, dato, NULL, NULL);
	if (!nodo) return false;

// Hice que busque la posicion para guardarlo al final. Agrega la cantidad.
	return buscar_ubicacion_nodo(arbol->raiz,nodo,clave,arbol);

}

// Similar a abb_borrar, solo que en este caso no destruye el nodo, solo devuelve
// el valor, o NULL de no hallarse.
// Pre: El ABB fue creado.
// Post: Devuelve el dato almacenado, o devuelve NULL cuando no se cumplan ciertas
// condiciones (Por ej.: clave no pertenece)
void *abb_obtener(const abb_t *arbol, const char *clave){

	if (arbol->raiz == NULL) return NULL;
	nodo_abb_t* raiz = arbol->raiz;
	nodo_abb_t* aux = buscar_nodo(raiz,clave,arbol);
	if (aux == NULL) return NULL;

	return aux->dato;
}

// Averigua si existe un nodo en el ABB con la clave provista.
// Pre: El ABB fue creado.
// Post: Devuelve true de encontrar el nodo, o false en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave){

	if (arbol->raiz == NULL) return NULL;
	nodo_abb_t* raiz = arbol->raiz;
	nodo_abb_t* aux = buscar_nodo(raiz,clave,arbol);
	if (aux == NULL) return false;
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
	if (arbol->raiz)
        destruir_nodos(arbol->raiz, arbol->destruir_dato);
	free(arbol);
}

/* *****************************************************************
 *                 Primitivas del iterador interno                 *
 * *****************************************************************/
/* *****************************************************************
 *                 Primitivas del iterador externo                 *
 * *****************************************************************/
