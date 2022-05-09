#ifndef LISTA_H
#define LISTA_H

#include <stdlib.h>
#include <stdbool.h>

struct lista;
typedef struct lista lista_t;

struct lista_iter;
typedef struct lista_iter lista_iter_t;


// Crea una lista
// Post: devuelve una lista vacia
lista_t *lista_crear(void);

// Verifica si la lista esta vacia
// devuelve true si la lista no tiene elementos y false en caso contrario
// Pre: la lista fue creada
bool lista_esta_vacia(const lista_t *lista);

// Agrega un elemento en la primera posicion de la lista
// Pre: la lista esta creada
// Pos: se agrego un nuevo elemento a la lista, este elemento es el primero
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un elemento en la ultima posicion de la lista
// Pre: la lista esta creada
// Pos: se agrego un nuevo elemento a la lista, este elemento es el ultimo
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la cola no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Devuelve el largo de la lista y 0 si esta vacia
// Pre: la lista fue creada
// Post: Devuelve el largo de la lista
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

void pruebas_lista_estudiante(void);


// Crea un iterador de lista
// Post: Se crea un iterador que apunta al inicio de la lista
// Si el iterador esta en el inicio, el anterior apunta a NULL.
lista_iter_t *lista_iter_crear(lista_t *lista);

// Avanza al siguiente elemento
// Pre: el iterador fue creado y apunta algun elemento de la lista o esta al final de la misma
// Post: el iterador apunta al siguiete elemento o al final de la lista
// Devuelve true si avanzo al siguiente elemento, false en caso contrario
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el dato de el iterador actual
// Pre: EL iterador fue creado y apunta algun elemento de la lista o esta al final de la misma
// Post: Devuelve el dato del elemento actual
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Pregunta si el iterador esta al final
// Pre: EL iterador fue creado y apunta algun elemento de la lista o esta al final de la misma
// Post: Devuelve true si el iterador esta al final o false en caso contrario
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterado
// Pre: EL iterador fue creado y apunta algun elemento de la lista o esta alfinal de la misma
// Post: Destruye el iterador
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un elemento en la lista
// Pre: EL iterador fue creado y apunta algun elemento de la lista o esta al final de la misma
// Post: se agrego un elemento a la lista.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Elimina un elemento de la lista y devuelve el dato por copia
// Pre: EL iterador fue creado y apunta algun elemento de la lista o esta al final de la misma
// Post: la lista tiene un elemento menos 
void *lista_iter_borrar(lista_iter_t *iter);

// Recorre la lista hasta que visitar sea false y va aplicando la funcion a cada dato,
// utiliza el parametro extra como condicion de corte
// Pre: La lista fue creada
// Post: Se le aplico la funcion visitar a la cantidad de objetos deseados
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

#endif // LISTA_H
