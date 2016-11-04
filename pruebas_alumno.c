#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "abb.h"
#include "testing.h"

/* Pruebas para un abb vacio */
static void pruebas_abb_vacio()
{
    printf("INICIO DE PRUEBAS CON ABB VACIO\n");

    /* Declaro las variables a utilizar*/
    char * s = "Hola mundo";
    abb_t* abb = abb_crear(NULL, NULL);

    /* Inicio de pruebas */
    print_test("crear abb", abb);
    print_test("pertenece devuelve NULL", !abb_pertenece(abb, s));
    print_test("obtener devuelve NULL", abb_obtener(abb, s) == NULL);
    print_test("borrar devuelve NULL", abb_borrar(abb, s) == NULL);
    print_test("la cantidad de elementos es cero", abb_cantidad(abb) == 0);

    /* Si la clave es NULL devuelve NULL */
    print_test("guardar NULL devuelve false", !abb_guardar(abb, NULL, NULL));
    print_test("pertenece devuelve NULL", abb_obtener(abb, NULL) == NULL);
    print_test("obtener devuelve NULL", abb_obtener(abb, NULL) == NULL);
    print_test("borrar devuelve NULL", abb_borrar(abb, NULL) == NULL);
    print_test("la cantidad de elementos es cero", abb_cantidad(abb) == 0);

    /* Destruyo el abb */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_algunos_elementos()
{
    int i;
    char * s[] = {"Spam", "Eggs", "Monty", "Python"};
    int datos[4] = {1, 2, 3, 4};
    abb_t * abb = abb_crear(strcmp, NULL);

    printf("INICIO DE PRUEBAS CON ALGUNOS ELEMENTOS\n");
    print_test("crear abb", abb != NULL);

    /* Guardo y borro pares clave-dato */
    print_test("guardar Spam-1", abb_guardar(abb, s[0], datos+0));
    print_test("guardar Eggs-2", abb_guardar(abb, s[1], datos+1));
    print_test("la cantidad de elementos es 2", abb_cantidad(abb) == 2);
    print_test("pertenece Spam", abb_pertenece(abb, s[0]));
    print_test("pertenece Eggs", abb_pertenece(abb, s[1]));
    print_test("obtener dato Spam", abb_obtener(abb, s[0]) == datos+0);
    print_test("obtener dato Eggs", abb_obtener(abb, s[1]) == datos+1);
    print_test("borrar Spam", abb_borrar(abb, s[0]) == datos+0);
    print_test("borrar Eggs", abb_borrar(abb, s[1]) == datos+1);

    /* Ahora está vacío */
    print_test("el abb está vacío", abb_cantidad(abb) == 0);
    print_test("NO pertenece Spam", !abb_pertenece(abb, s[0]));
    print_test("NO pertenece Eggs", !abb_pertenece(abb, s[1]));
    print_test("borrar Spam falla", abb_borrar(abb, s[0]) == NULL);
    print_test("borrar Eggs falla", abb_borrar(abb, s[1]) == NULL);

    /* Guardo cuatro elementos y destruyo el árbol */
    for (i = 0; i < 4; i++) {
        abb_guardar(abb, s[i], datos+i);
    }
    print_test("la cantidad de elementos es 4", abb_cantidad(abb) == 4);
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_muchos_elementos() {
    int i;
    bool error_flag = false;
    /* 50 strings generadas con el generador de RANDOM.ORG */
    char * s[] = {"KapWthhMZN", "wJafuuNGGs", "xeHwDBbqKE", "YlIgBRtNMR", "HDklPmhEhH",
                  "ezXEGBbemZ", "EsMQFQhiEl", "dKSPAmdTFW", "GFUcNdHRhO", "TBWvVlZCAs",
                  "zLwbHFjwVZ", "LLjwmbpwOV", "xXpCoxmPPo", "CnTlRlVlGI", "BAzetXsLkn",
                  "LZEBQUbQvm", "SoUYQCMJHy", "CuAkAaNbAu", "KZzIdpXbfR", "WgoUQDLoUQ",
                  "UQvYqOHbIQ", "odUwNXrLEU", "wvDneotFfG", "maRwyNPaKV", "TIuFzZVcbQ",
                  "tzVTxRGgia", "DkMplBXOsi", "FzwlcEMhGQ", "ROGxjtntfg", "XKrHElvYBY",
                  "pDylGxNZxy", "CrEdTyHkSK", "rwNjxHUzvo", "sVSqYZmczc", "pVPdryuUui",
                  "ddJjnVyAhC", "ggIUpTpIsS", "csOnJrXVWI", "IUWQtSodcI", "fEZItKnRhr",
                  "sYtnFgmLRA", "JvKoEiCylA", "bPdacioJft", "nCVmGtTAEq", "oKryKVAfwD",
                  "XfBHxUWnaO", "kslTYhHawV", "FSaZOaYHCQ", "OnxFVFFxfL", "wHmlOiqFsQ"};
    abb_t * abb = abb_crear(strcmp, free);

    printf("INICIO DE PRUEBAS CON MUCHOS ELEMENTOS\n");

    print_test("crear abb", abb != NULL);
    /* Guardo 50 pares string-dato, donde dato es un puntero genérico devuelto por malloc */
    for (i = 0; i < 50; i++) {
        abb_guardar(abb, s[i], malloc(sizeof(int)));
    }
    print_test("se agregaron 50 elementos", abb_cantidad(abb) == 50);
    for (i = 0; i < 50; i++) {
        if (!abb_pertenece(abb, s[0])) {
            error_flag = true;
        }
    }
    print_test("los 50 elementos pertenecen al abb", !error_flag);

    for (i = 0; i < 15; i++) {
        free(abb_borrar(abb, s[i]));
    }
    print_test("la cantidad de elementos es 35", abb_cantidad(abb) == 35);

    /* Elimino los 25 elementos restantes al destruir el árbol */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

/*
static void pruebas_lista_iterador_externo() {
    int vec[] = {1, 2, 3};
    lista_t * lista = lista_crear();
    lista_iter_t * iter = lista_iter_crear(lista);
    printf("INICIO DE PRUEBAS CON ITERADOR\n");

    print_test("crear lista", lista != NULL);
    print_test("crear iterador", iter != NULL);
    print_test("iter esta al final", lista_iter_al_final(iter));
    print_test("iter avanzar devuelve false", !lista_iter_avanzar(iter));

    print_test("insertar 1", lista_iter_insertar(iter, vec));
    print_test("1 esta en la posicion actual", lista_iter_ver_actual(iter) == vec);
    print_test("borrar 1", lista_iter_borrar(iter) == vec);
    print_test("iter esta al final", lista_iter_al_final(iter));
    print_test("insertar 1", lista_iter_insertar(iter, vec));
    print_test("1 esta en la posicion actual", lista_iter_ver_actual(iter) == vec);
    print_test("insertar 2", lista_iter_insertar(iter, vec+1));
    print_test("2 esta en la posicion actual", lista_iter_ver_actual(iter) == vec+1);
    print_test("avanzar", lista_iter_avanzar(iter));
    print_test("1 esta en la posicion actual", lista_iter_ver_actual(iter) == vec);
    print_test("avanzar", lista_iter_avanzar(iter));
    print_test("avanzar devuelve false", !lista_iter_avanzar(iter));
    print_test("insertar 3", lista_iter_insertar(iter, vec+2));
    print_test("borrar 3", lista_iter_borrar(iter) == vec+2);
    print_test("iter esta al final", lista_iter_al_final(iter));
    print_test("el iterador fue destruido", true);
    lista_iter_destruir(iter);

    iter = lista_iter_crear(lista);
    print_test("crear iterador", iter != NULL);
    print_test("borrar 2", lista_iter_borrar(iter) == vec+1);
    print_test("borrar 1", lista_iter_borrar(iter) == vec);
    print_test("borrar devuelve NULL", lista_iter_borrar(iter) == NULL);
    print_test("insertar NULL", lista_iter_insertar(iter, NULL));
    print_test("ver actual da NULL", lista_iter_ver_actual(iter) == NULL);
    print_test("iter no esta al final", !lista_iter_al_final(iter));
    print_test("borrar NULL", lista_iter_borrar(iter) == NULL);
    print_test("iter esta al final", lista_iter_al_final(iter));
    print_test("el largo es cero", lista_largo(lista) == 0);
    print_test("el iterador y la lista fueron destruidos", true);
    lista_iter_destruir(iter);
    lista_destruir(lista, NULL);
}
*/

void pruebas_abb_alumno() {
    pruebas_abb_vacio();
    pruebas_abb_algunos_elementos();
    // pruebas_abb_muchos_elementos();
}
