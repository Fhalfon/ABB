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
	else if (cmp(raiz->clave,clave) == 0)
		return raiz;
	else if (cmp(raiz->clave,clave) > 0)
		return buscar_nodo(raiz->izq, clave, cmp);
	else
        return buscar_nodo(raiz->der, clave, cmp);
}

// La idea de la funcion es buscar el nodo en donde tenga que insertar
// sea a derecha o izquierda el nodo que me pasan como dato
// devuelvo un puntero para insertarlo alli.
// Uso const por que NO debo modificarlo solo buscar.
static bool buscar_ubicacion_nodo(abb_nodo_t* raiz,abb_nodo_t* nodo, const char *clave,  abb_t* arbol)
{
	if (arbol->raiz == NULL ) {
		arbol->raiz = nodo;
		arbol->cantidad++;
		return true;
	}

	if (arbol->cmp(raiz->clave,clave) == 0){
		if (arbol->destruir_dato != NULL) arbol->destruir_dato(raiz->dato);
		raiz->dato = nodo->dato;
		free(nodo->clave);
		free(nodo);
		return true;
	}

	if (arbol->cmp(raiz->clave,clave) > 0){
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

/* Inserta un nodo */
abb_nodo_t * insertar_nodo(abb_nodo_t * nodo, abb_nodo_t * nuevo, abb_comparar_clave_t cmp)
{
    if (!nodo) {
        return nuevo;
    } else if (cmp(nodo->clave, nuevo->clave) >= 0) {   // Las claves iguales se ubican hacia la derecha
        nodo->der = insertar_nodo(nodo->der, nuevo, cmp);
        return nodo->der;
    } else {
        nodo->izq = insertar_nodo(nodo->izq, nuevo, cmp);
        return nodo->izq;
    }
}

/// FUNCIONES NUEVAS


// Funcion que borra el caso de que un nodo sea hoja sin hijos.
// Post: devuelte el dato del nodo
static void *borrar_caso_1(abb_nodo_t *actual, abb_nodo_t *anterior, abb_t *arbol){

	void *dato = actual->dato;

	if (anterior == NULL) arbol->raiz = NULL;
	else{
		if (anterior->izq == actual) anterior->izq = NULL;
		if (anterior->der == actual) anterior->der = NULL;
	}
	arbol->cantidad--;
	free(actual->clave);
	free(actual);
	return dato;
}

// Funcion que borra en el caso de que el nodo sea una hoja con 1 solo hijo.
// Post: devuelte el dato del nodo
static void *borrar_caso_2(abb_nodo_t *actual, abb_nodo_t *anterior, abb_t *arbol){


	void *dato = actual->dato;
	if (anterior == NULL){
		if (actual->izq != NULL) arbol->raiz = actual->izq;
		if (actual->der != NULL) arbol->raiz = actual->der;
	}
	else{
		if (anterior->izq == actual){
			if (actual->izq != NULL) anterior->izq = actual->izq;
			if (actual->der != NULL) anterior->izq = actual->der;
		}

		if (anterior->der == actual){
			if (actual->izq != NULL) anterior->der = actual->izq;
			if (actual->der != NULL) anterior->der = actual->der;
		}
	}
	arbol->cantidad--;
	free(actual->clave);
	free(actual);
	return dato;
}


// Funcion que busca el minimo valor a partir de una hoja.
// Post: devuelte el dato del nodo
static void *buscar_reemplazante(abb_nodo_t* actual){

	if (actual->izq != NULL) return buscar_reemplazante(actual->izq);
	return actual;
}

// Funcion que borra en el caso de que el nodo tenga 2 hijos.
// Post: devuelte el dato del nodo
void* borrar_caso_3(abb_nodo_t *actual, abb_t *arbol){

	void* dato = actual->dato;
	abb_nodo_t* reemplazante = actual->der;
	reemplazante = buscar_reemplazante(reemplazante);

	char* clave_aux = malloc((strlen(reemplazante->clave) + 1)*sizeof(char));
	if (clave_aux == NULL)
	{
		free(reemplazante);
		return NULL;
	}
	strcpy(clave_aux,reemplazante->clave);

	void* dato_aux = abb_borrar(arbol,clave_aux);

	free(actual->clave);
	actual->clave = clave_aux;
	actual->dato = dato_aux;
	return dato;
}

// Funcion que elije que tipo de caso es (sin hijo, 1 hijo, 2 hijos)
// Post: devuelte el dato del nodo
static void *borrar_nodo(abb_nodo_t *actual, abb_nodo_t *anterior, abb_t *arbol){

	if ( (actual->izq == NULL) && (actual->der == NULL) ){
		return borrar_caso_1(actual,anterior,arbol);
	}
	if ( (actual->izq != NULL) && (actual->der != NULL) ){
		return borrar_caso_3(actual,arbol);
	}
	return borrar_caso_2(actual,anterior,arbol);

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
	abb_nodo_t* nuevo = nodo_crear(clave, dato, NULL, NULL);
	if (!nuevo) return false;
    arbol->raiz = insertar_nodo(arbol->raiz, nuevo, arbol->cmp);
    ++(arbol->cantidad);
	return true;
}

// Similar a abb_borrar, solo que en este caso no destruye el nodo, solo devuelve
// el valor, o NULL de no hallarse.
// Pre: El ABB fue creado.
// Post: Devuelve el dato almacenado, o devuelve NULL cuando no se cumplan ciertas
// condiciones (Por ej.: clave no pertenece)
void *abb_obtener(const abb_t *arbol, const char *clave){

	if (arbol->raiz == NULL) return NULL;
	abb_nodo_t* raiz = arbol->raiz;
	abb_nodo_t* aux = buscar_nodo(raiz,clave,arbol->cmp);
	if (aux == NULL) return NULL;

	return aux->dato;
}

// Averigua si existe un nodo en el ABB con la clave provista.
// Pre: El ABB fue creado.
// Post: Devuelve true de encontrar el nodo, o false en caso contrario.
bool abb_pertenece(const abb_t *arbol, const char *clave){

	if (arbol->raiz == NULL) return NULL;
	abb_nodo_t* raiz = arbol->raiz;
	abb_nodo_t* aux = buscar_nodo(raiz,clave,arbol->cmp);
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

///////////////////////
// Funcion auxiliar para buscar el nodo a borrar.
// Pre: se deve mandar como actual el nodo a partir del cual se quiere buscar
// y en anterior, el anterior a este. En caso de que el actual sea la raiz
// anterior debe ser un abb_nodo_t* anterior == NULL.
// Post: el dato mandado en actual pasa a ser el que se quiere borrar y
// el anterior es el que apunta al actual.
static void *abb_buscar_borrar(abb_nodo_t *actual, abb_nodo_t *anterior, abb_t *arbol, const char *clave){
	// Caso Borde: esta vacio.
	if (actual == NULL) return NULL;
	// Caso Base.
	// Devuelvo el actual sin cambios.
	if (actual->clave == NULL) return NULL;
	if (arbol->cmp(actual->clave,clave) == 0)
        return borrar_nodo(actual,anterior,arbol);


// Se supone que si la funcion cmp devuelve algo mayor a 0 es
// porque el segundo operando es mayor.
	if (arbol->cmp(actual->clave,clave) > 0) return abb_buscar_borrar(actual->izq,actual,arbol,clave);
	else return abb_buscar_borrar(actual->der,actual,arbol,clave);
}


// Devuelve un dato almacenado en el ABB, y destruye el nodo que lo contiene.
// Pre: El ABB fue creado.
// Post: Devuelve el dato almacenado y destruye el nodo que lo contenía, o devuelve
// NULL cuando no se cumplan ciertas condiciones (Por ej.: clave no pertenece)
void *abb_borrar(abb_t *arbol, const char *clave){
	if (arbol == NULL) return NULL;
	abb_nodo_t *anterior = NULL;
	abb_nodo_t *actual = arbol->raiz;
// Caso borde de que este vacio.
	if (actual == NULL) return NULL;
	return abb_buscar_borrar(actual,anterior,arbol,clave);

}

/* *****************************************************************
 *                 Primitivas del iterador interno                 *
 * *****************************************************************/
/* *****************************************************************
 *                 Primitivas del iterador externo                 *
 * *****************************************************************/
