#include "pila.h"
#include <stdlib.h>
#define TAM_INICIAL 16       // Capacidad inicial de la pila

/* Definición del struct pila proporcionado por la cátedra. */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* Funcion propuesta por Alejandro Levinas */
static bool pila_redimensionar(pila_t* pila, size_t tam_nuevo) {
    if (!pila || tam_nuevo < TAM_INICIAL) return false;
    void* datos_nuevo = realloc(pila->datos, (tam_nuevo * sizeof(void*)));
    if (!datos_nuevo) return false;
    pila->datos = datos_nuevo;
    pila->capacidad = tam_nuevo;
    return true;
}

static bool debeAchicarse(pila_t * pila) {
    return (4*pila->cantidad <= pila->capacidad && pila->capacidad >= 2*TAM_INICIAL);
}

pila_t* pila_crear(void) {
    pila_t * nueva = malloc(sizeof(*nueva));
    if (nueva != NULL) {
        nueva->datos = (void **) malloc(sizeof(void*)*TAM_INICIAL);
        if (nueva->datos == NULL) {
            free(nueva);
            return NULL;
        }
        nueva->cantidad = 0;
        nueva->capacidad = (size_t) TAM_INICIAL;
    }
    return nueva;
}

void pila_destruir(pila_t *pila) {
    if (pila != NULL) {
        free(pila->datos);
        free(pila);
    }
    return;
}

bool pila_esta_vacia(const pila_t *pila) {
    return (bool) (pila->cantidad == 0);
}

bool pila_apilar(pila_t *pila, void* valor) {
    if (pila->cantidad == pila->capacidad && !pila_redimensionar(pila, 2*(pila->capacidad))) {
        return false;
    }
    pila->datos[(pila->cantidad)++] = valor;
    return true;
}

void* pila_ver_tope(const pila_t *pila) {
    if (pila_esta_vacia(pila)) {
        return NULL;
    } else {
        return pila->datos[pila->cantidad-1];
    }
}

void* pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila) || (debeAchicarse(pila) && !pila_redimensionar(pila, (pila->capacidad)/2)) ) {
        return NULL;
    } else {
        return pila->datos[--(pila->cantidad)];
    }
}
