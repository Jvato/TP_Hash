#include "hash.h"
#include "lista.h"
#include <string.h>
#include <stdio.h>
#define TAMANIO_INICIAL 100
#define FACTOR_CARGA_MIN 2.0
#define FACTOR_CARGA_MAX 3.0
#define POTENCIA_AUMENTAR_MEMORIA 2

typedef struct nodo nodo_t;

struct lista {
	nodo_t * prim;
	nodo_t * ult;
	size_t largo;
};

struct nodo {
	void * dato;
	nodo_t * prox;
};

typedef struct campo{
    char* clave;
    void* dato;
}campo_t;

struct hash{
    size_t cantidad;
    size_t capacidad;
    lista_t** tabla;
    hash_destruir_dato_t funcion_destruccion;
};

struct hash_iter {
	lista_iter_t * lista_iter;
	size_t posc;
	hash_t * hash;
};

int  _siguiente_elemento_valido(const hash_t * hash, int inicio) {
	int i;
	for (i = inicio; i < hash->capacidad; i++) {
		if (hash->tabla[i] != NULL && lista_esta_vacia(hash->tabla[i]) == false)
			return i;
	}
	return -1;
}

hash_iter_t *hash_iter_crear(const hash_t *hash) {
	if (hash->cantidad == 0)
		return NULL;
	int posc;
	lista_iter_t * lista_iter;
	posc = _siguiente_elemento_valido(hash,0);
	if (posc == -1)
		return NULL;
	lista_iter = lista_iter_crear(hash->tabla[posc]);	
	hash_iter_t *iter = malloc(sizeof(hash_iter_t));
	if (iter == NULL) {
		lista_iter_destruir(lista_iter);
		return NULL;
	}
	iter->hash = (hash_t *)hash;
	iter->lista_iter = lista_iter;
	iter->posc = (size_t)posc;
	return iter;
}


bool hash_iter_avanzar(hash_iter_t *iter) {
	lista_iter_t * lista_iter_nuevo;
	int posc;
	if (hash_iter_al_final(iter) == true)
		return false;
	if (lista_iter_avanzar(iter->lista_iter) == false)
		return false;
	if (lista_iter_al_final(iter->lista_iter) == true) {
		posc = _siguiente_elemento_valido(iter->hash,(int)iter->posc);
		if (posc == -1)
			return true;
		lista_iter_destruir(iter->lista_iter);
		lista_iter_nuevo = lista_iter_crear(iter->hash->tabla[posc]);
		if (lista_iter_nuevo == NULL)
			return false;
		iter->lista_iter = lista_iter_nuevo;
		iter->posc = (size_t)posc;
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	if (hash_iter_al_final(iter))
		return NULL;
	campo_t * campo = lista_iter_ver_actual(iter->lista_iter);
	char * cadena = malloc(strlen(campo->clave)+1);
	strcpy(cadena,campo->clave);
	return cadena;
}

bool hash_iter_al_final(const hash_iter_t *iter) {
	return lista_iter_al_final(iter->lista_iter);
}

void hash_iter_destruir(hash_iter_t* iter) {
	lista_iter_destruir(iter->lista_iter);
	free(iter);
}



// esta funcion de hasheo se llama FNV y la sacamos de este link
// https://www.programmingalgorithms.com/algorithm/fnv-hash/c/
// se le hizo unos cambios minimos, como cambiar el tipo del dato que devuelve a size_t 
// tambien se casteo (size_t)(*str) para que compile y se uso el operador % en el return para que 
// el numero devuelto entre en el rango de la tabla

size_t FNVHash(const char* str, size_t length) {
	const unsigned int fnv_prime = 0x811C9DC5;
	size_t hash = 0;
	size_t i = 0;

	for (i = 0; i < length; str++, i++)
	{
		hash *= fnv_prime;
		hash = hash^(size_t)(*str);
	}
	
	return hash % length;
}

void _inicializar_vector(lista_t **tabla, size_t capacidad) {
	for (size_t i = 0; i < capacidad; i++)
		tabla[i] = NULL;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if(hash == NULL)	return NULL;

    hash->funcion_destruccion = destruir_dato;
    hash->cantidad = 0;
    hash->capacidad = TAMANIO_INICIAL;
    hash->tabla = malloc(sizeof(lista_t) * hash->capacidad);
    if(hash->tabla == NULL){
        free(hash);
        return NULL;
    }
	for(size_t i = 0; i < TAMANIO_INICIAL; i++){
		hash->tabla[i] = NULL;
	}
    return hash;
}

campo_t * crear_campo(const char * clave, void *dato) {
	campo_t * campo = malloc(sizeof(campo_t));
	if (campo == NULL)
		return NULL;
	campo->clave = (char *)clave;
	campo->dato = dato;
	return campo;
}


bool _redimensionar_hash(hash_t * hash) {
	lista_t **tabla_nueva;
	if ((float)(hash->cantidad / hash->capacidad) > FACTOR_CARGA_MIN && (float)(hash->cantidad / hash->capacidad) < FACTOR_CARGA_MAX) {
		size_t nueva_capacidad = POTENCIA_AUMENTAR_MEMORIA*hash->capacidad;
		tabla_nueva = calloc(nueva_capacidad,sizeof(lista_t *));
		hash_iter_t * iter = hash_iter_crear(hash);
		if (iter == NULL) {
			free(tabla_nueva);
			return false;
		}
		while (hash_iter_al_final(iter) == false) {
			const char * cadena = hash_iter_ver_actual(iter);
			size_t indice = FNVHash(cadena,nueva_capacidad);
			if (lista_insertar_ultimo(tabla_nueva[indice], "ESTO ES PARA QUE COMPILE") == true){

			}
		}
		// ME FALTA COMPLETAR ESTA PARTE; DEJAME LAS FUNCIONES HASTA HASH GUARDAR
	}	
	return false; //ESTO ES PARA QUE COMPILE
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	//if (_redimensionar_hash(hash) == false)    return false;
    size_t indice = FNVHash(clave, hash->capacidad);
    campo_t* campo = malloc(sizeof(campo_t));
    if(campo == NULL){
        return false;
    }
    // aca me tendria que guardar la clave con un malloc, fijarse en la clase de hashes:implementacion
    // del 2022-02-29
    campo->clave = (char *)clave;
    campo->dato = dato;
    if(!hash_pertenece(hash, clave)){
        lista_t* lista = lista_crear();
        hash->tabla[indice] = lista;
        lista_insertar_ultimo(lista, campo);
    }else{
		lista_t* lista = hash->tabla[indice];
		lista_iter_t* iter = lista_iter_crear(lista);
		while(!lista_iter_al_final(iter)){
			campo_t* campo = lista_iter_ver_actual(iter);
			if(campo->clave == clave){
				campo->dato = dato;
				lista_iter_destruir(iter);
				return true;
			}
			lista_iter_avanzar(iter);
		}
    }
	hash->cantidad++;
    return true;
}

campo_t* hash_iterar_indice(hash_t *hash, const char *clave, lista_t* lista, bool destruir){
    lista_iter_t* iter = lista_iter_crear(lista);
    while(!lista_iter_al_final(iter)){
        campo_t* campo = lista_iter_ver_actual(iter);
        if(campo->clave == clave && !destruir){
            lista_iter_destruir(iter);
            return campo;
        }else if(campo->clave == clave && destruir){
            campo_t* campo_borrado = lista_iter_borrar(iter);
            lista_iter_destruir(iter);
            return campo_borrado;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return NULL;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if(!hash_pertenece(hash, clave)){
        return NULL;
    }else{
        size_t indice = FNVHash(clave, hash->capacidad);
        lista_t* lista = hash->tabla[indice];
		campo_t* campo = hash_iterar_indice((hash_t *)hash, clave, lista, true);
		hash->cantidad--;
        return campo->dato;
    }
    return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if(!hash_pertenece(hash, clave)){
        return NULL;
    }else{
        size_t indice = FNVHash(clave, hash->capacidad);
        lista_t* lista = hash->tabla[indice];
        campo_t* campo = hash_iterar_indice((hash_t *)hash, clave, lista, false);
        return campo->dato;
    }
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t indice = FNVHash(clave, hash->capacidad);
	if(hash->tabla[indice] == NULL){
		return false;
	}
    lista_t* lista = hash->tabla[indice];
    if(lista_esta_vacia(lista)){
        return false;
    }else{
        if(hash_iterar_indice((hash_t *)hash, clave, lista, false) != NULL){
            return true;
        }
    }
    return false;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for(size_t i = 0; i < hash->capacidad; i++){
		if(hash->tabla[i] == NULL){
			continue;
		}
		lista_t* lista = hash->tabla[i];
		lista_destruir(lista, hash->funcion_destruccion);
	}
	free(hash->tabla);
	free(hash);
}
