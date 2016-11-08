#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "abb.h"
#include "testing.h"

/* Pruebas para un abb vacio */
static void pruebas_abb_vacio()
{
    /* Declaro las variables a utilizar*/
    char * s = "Hola mundo";
    abb_t* abb = abb_crear(NULL, NULL);

    printf("INICIO DE PRUEBAS CON ABB VACIO\n");
    print_test("crear abb", abb);

    /* Inicio de pruebas */
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

    /* Guardo cuatro elementos y destruyo el abb */
    for (i = 0; i < 4; i++) {
        abb_guardar(abb, s[i], datos+i);
    }
    print_test("la cantidad de elementos es 4", abb_cantidad(abb) == 4);

    /* Destruyo el abb con sus 4 elementos */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_borrar_hoja()
{
    char * s[] = {"Barco", "Avion", "Casa"};
    int datos[] = {10, 20, 30};
    abb_t * abb = abb_crear(strcmp, NULL);

	printf("INICIO DE PRUEBAS BORRAR HOJA\n");
    print_test("crear abb", abb != NULL);

    /* Agrego 3 elementos, 2 son hoja */
    print_test("guardar Barco-10", abb_guardar(abb, s[0], datos+0));
    print_test("guardar Avion-20", abb_guardar(abb, s[1], datos+1));
    print_test("guardar Casa-30", abb_guardar(abb, s[2], datos+2));

    /* Borro las dos hojas y verifico que solo haya quedado su padre */
    print_test("borrar Casa", abb_borrar(abb, s[2]) == datos+2);
    print_test("borrar Avion", abb_borrar(abb, s[1]) == datos+1);
    print_test("la cantidad de elementos es 1", abb_cantidad(abb) == 1);
    print_test("pertenece Barco", abb_pertenece(abb, s[0]));
    print_test("NO pertenece Avion", !abb_pertenece(abb, s[1]));
    print_test("NO pertenece Casa", !abb_pertenece(abb, s[2]));

    /* Destruyo el abb con sus elementos restantes */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_borrar_un_hijo()
{
    char * s[] = {"Casa", "Barco", "Dromedario", "Avion", "Elefante"};
    int datos[] = {10, 20, 30, 40, 50};
    abb_t * abb = abb_crear(strcmp, NULL);

	printf("INICIO DE PRUEBAS BORRAR NODO CON UN HIJO\n");
    print_test("crear abb", abb != NULL);

    /* Agrego 5 elementos, 2 tienen un solo hijo */
    print_test("guardar Casa-10", abb_guardar(abb, s[0], datos+0));
    print_test("guardar Barco-20", abb_guardar(abb, s[1], datos+1));
    print_test("guardar Dromedario-30", abb_guardar(abb, s[2], datos+2));
    print_test("guardar Avion-40", abb_guardar(abb, s[3], datos+3));
    print_test("guardar Elefante-50", abb_guardar(abb, s[4], datos+4));

    /* Borro los dos elementos que tienen un solo hijo */
    print_test("borrar Barco", abb_borrar(abb, s[1]) == datos+1);
    print_test("borrar Dromedario", abb_borrar(abb, s[2]) == datos+2);

    /* Verifico que hayan quedado los otros 3 */
    print_test("la cantidad de elementos es 3", abb_cantidad(abb) == 3);
    print_test("pertenece Casa", abb_pertenece(abb, s[0]));
    print_test("pertenece Elefante", abb_pertenece(abb, s[4]));
    print_test("pertenece Avion", abb_pertenece(abb, s[3]));
    print_test("NO pertenece Barco", !abb_pertenece(abb, s[1]));
    print_test("NO pertenece Dromedario", !abb_pertenece(abb, s[2]));

    /* Destruyo el abb con sus elementos restantes */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_borrar_dos_hijos()
{
    char * s[] = {"Dron", "Bazar", "Fascinante", "Aereo", "Cazador", "Espeluznante", "Grafo"};
    int datos[] = {10, 20, 30, 40, 50, 60, 70};
    abb_t * abb = abb_crear(strcmp, NULL);

	printf("INICIO DE PRUEBAS BORRAR NODO CON DOS HIJOS\n");
    print_test("crear abb", abb != NULL);

    /* Agrego 7 elementos, 2 tienen un solo hijo */
    print_test("guardar Dron-10", abb_guardar(abb, s[0], datos+0));
    print_test("guardar Bazar-20", abb_guardar(abb, s[1], datos+1));
    print_test("guardar Fascinante-30", abb_guardar(abb, s[2], datos+2));
    print_test("guardar Aereo-40", abb_guardar(abb, s[3], datos+3));
    print_test("guardar Cazador-50", abb_guardar(abb, s[4], datos+4));
    print_test("guardar Espeluznante-60", abb_guardar(abb, s[5], datos+5));
    print_test("guardar Grafo-70", abb_guardar(abb, s[6], datos+6));

    /* Borro los dos elementos que tienen dos hijos */
    print_test("borrar Bazar", abb_borrar(abb, s[1]) == datos+1);
    print_test("borrar Fascinante", abb_borrar(abb, s[2]) == datos+2);

    /* Verifico que hayan quedado los otros 5 */
    print_test("la cantidad de elementos es 5", abb_cantidad(abb) == 5);
    print_test("pertenece Dron", abb_pertenece(abb, s[0]));
    print_test("pertenece Aereo", abb_pertenece(abb, s[4]));
    print_test("pertenece Cazador", abb_pertenece(abb, s[3]));
    print_test("pertenece Espeluznante", abb_pertenece(abb, s[3]));
    print_test("pertenece Grafo", abb_pertenece(abb, s[3]));
    print_test("NO pertenece Bazar", !abb_pertenece(abb, s[1]));
    print_test("NO pertenece Fascinante", !abb_pertenece(abb, s[2]));

    /* Borro la raiz */
    print_test("borrar Dron", abb_borrar(abb, s[0]) == datos+0);

    /* Verifico que hayan quedado los otros 4 */
    print_test("la cantidad de elementos es 4", abb_cantidad(abb) == 4);
    print_test("pertenece Aereo", abb_pertenece(abb, s[4]));
    print_test("pertenece Cazador", abb_pertenece(abb, s[3]));
    print_test("pertenece Espeluznante", abb_pertenece(abb, s[3]));
    print_test("pertenece Grafo", abb_pertenece(abb, s[3]));
    print_test("NO pertenece Dron", !abb_pertenece(abb, s[0]));

    /* Destruyo el abb con sus elementos restantes */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_muchos_elementos()
{
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
    /* Elimino 25 elementos del abb */
    for (i = 0; i < 25; i++) {
        free(abb_borrar(abb, s[i]));
    }
    print_test("la cantidad de elementos es 25", abb_cantidad(abb) == 25);

    /* Elimino los 25 elementos restantes al destruir el abb */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

static void pruebas_abb_iter_externo_vacio()
{
	abb_t* abb = abb_crear(strcmp, NULL);
	abb_iter_t* iter;

	printf("INICIO DE PRUEBAS ITERADOR EXTERNO CON UN ABB VACIO\n");

    /* Chequeo que el abb se haya creado y creo el iterador */
    print_test("crear abb", abb != NULL);
    iter = abb_iter_in_crear(abb);

    /* Inicio de pruebas */
	print_test("el iterador fue creado", iter);
	print_test("avanzar con el iterador es false", !abb_iter_in_avanzar(iter));
	print_test("ver actual con el iterador es NULL", !abb_iter_in_ver_actual(iter));
	print_test("el iterador al final", abb_iter_in_al_final(iter));

    /* Destruyo el iterador */
	abb_iter_in_destruir(iter);
}

static void pruebas_abb_iter_externo_algunos_elementos()
{
	char * claves[] = {"d","a","f","b","i","h","g","e","c"};
	int datos[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    abb_t * abb = abb_crear(NULL, NULL);
    abb_iter_t * iter;

	printf("INICIO DE PRUEBAS ITERADOR DE UN ABB CON VARIOS ELEMENTOS\n");
    print_test("crear abb", abb != NULL);

	/* Guardo 8 elementos*/
    print_test("guardar d-1", abb_guardar(abb, claves[0], datos+0));
    print_test("guardar a-2", abb_guardar(abb, claves[1], datos+1));
    print_test("guardar f-3", abb_guardar(abb, claves[2], datos+2));
    print_test("guardar b-4", abb_guardar(abb, claves[3], datos+3));
    print_test("guardar i-5", abb_guardar(abb, claves[4], datos+4));
    print_test("guardar h-6", abb_guardar(abb, claves[5], datos+5));
    print_test("guardar g-7", abb_guardar(abb, claves[6], datos+6));
    print_test("guardar e-8", abb_guardar(abb, claves[7], datos+7));
    print_test("guardar c-9", abb_guardar(abb, claves[8], datos+8));

    /* Creo el iterador */
    iter = abb_iter_in_crear(abb);

    /* Inicio de pruebas con el iterador */
	print_test("El iterador fue creado", iter != NULL);
	print_test("El iterador no esta al final",!abb_iter_in_al_final(iter));
    print_test("la clave es \"a\"", strcmp(abb_iter_in_ver_actual(iter), claves[1]));
	print_test("El iterador no esta al final", !abb_iter_in_al_final(iter));

    /* Recorro el abb con el iterador */
	print_test("Primer clave es 'a'", strcmp(abb_iter_in_ver_actual(iter), claves[1]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("Segunda clave es 'b'", strcmp(abb_iter_in_ver_actual(iter), claves[3]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("Tercera clave es 'c'", strcmp(abb_iter_in_ver_actual(iter), claves[8]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("Cuarta clave es 'd'", strcmp(abb_iter_in_ver_actual(iter), claves[0]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("El iterador no esta al final", !abb_iter_in_al_final(iter));
	print_test("Quinta clave es 'e'", strcmp(abb_iter_in_ver_actual(iter), claves[7]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("Sexta clave es 'f'", strcmp(abb_iter_in_ver_actual(iter), claves[2]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("Septima clave es 'g'", strcmp(abb_iter_in_ver_actual(iter), claves[6]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
	print_test("Octava clave es 'h'", strcmp(abb_iter_in_ver_actual(iter), claves[5]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));
    print_test("Octava clave es 'i'", strcmp(abb_iter_in_ver_actual(iter), claves[4]) == 0);
	print_test("Avanzo con el iterador", abb_iter_in_avanzar(iter));

    /* Chequeo que el iterador está al final */
	print_test("El iterador esta al final",abb_iter_in_al_final(iter));

    /* Destruyo el abb y el iterador */
	abb_iter_in_destruir(iter);
    abb_destruir(abb);
}

/* pruebas para el iter interno del ABB */
static bool imprimir_clave(const char *clave, void *dato, void *extra)
{
	if (clave) {
		printf("%s\n",clave);
	    return true;
	}
	return false;
}

static void pruebas_abb_iter_interno()
{
    char * s[] = {"Dron", "Bazar", "Fascinante", "Aereo", "Cazador", "Espeluznante",
                  "Grafo", "Casa", "Barco", "Dromedario", "Avion", "Elefante"};
    int datos[] = {10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65};
    abb_t * abb = abb_crear(strcmp, NULL);

    printf("INICIO DE PRUEBAS CON ITERADOR INTERNO\n");
    print_test("crear abb", abb != NULL);

    /* Agrego 12 elementos */
    print_test("guardar Dron-10", abb_guardar(abb, s[0], datos+0));
    print_test("guardar Bazar-15", abb_guardar(abb, s[1], datos+1));
    print_test("guardar Fascinante-20", abb_guardar(abb, s[2], datos+2));
    print_test("guardar Aereo-25", abb_guardar(abb, s[3], datos+3));
    print_test("guardar Cazador-30", abb_guardar(abb, s[4], datos+4));
    print_test("guardar Espeluznante-35", abb_guardar(abb, s[5], datos+5));
    print_test("guardar Grafo-40", abb_guardar(abb, s[6], datos+6));
    print_test("guardar Casa-45", abb_guardar(abb, s[7], datos+7));
    print_test("guardar Barco-50", abb_guardar(abb, s[8], datos+8));
    print_test("guardar Dromedario-55", abb_guardar(abb, s[9], datos+9));
    print_test("guardar Avion-60", abb_guardar(abb, s[10], datos+11));
    print_test("guardar Elefante-65", abb_guardar(abb, s[11], datos+12));

    /* Recorro los nodos en inorden y se imprime su dato */
    printf("RECORRIDO INORDER\n");
    abb_in_order(abb, imprimir_clave, NULL);

    /* Destruyo el abb */
    abb_destruir(abb);
    print_test("el abb fue destruido", true);
}

void pruebas_abb_alumno()
{
    pruebas_abb_vacio();
    pruebas_abb_algunos_elementos();
    pruebas_abb_borrar_hoja();
    pruebas_abb_borrar_un_hijo();
    pruebas_abb_borrar_dos_hijos();
    pruebas_abb_muchos_elementos();
    pruebas_abb_iter_externo_vacio();
    pruebas_abb_iter_externo_algunos_elementos();
    pruebas_abb_iter_interno();
}
