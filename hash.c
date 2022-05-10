#include "hash.h"
#include "lista.h"
#include <string.h>
#define TAMANIO_INICIAL 100
#define FACTOR_CARGA_MIN 2.0
#define FACTOR_CARGA_MAX 3.0
#define POTENCIA_AUMENTAR_MEMORIA 2

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

typedef struct hash_iter {
	lista_iter_t * lista_iter;
	size_t posc;
	hash_t * hash;
} hash_iter_t;

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
	iter->posc = posc;
	return iter;
}


bool hash_iter_avanzar(hash_iter_t *iter) {
	lista_iter_t * lista_iter_nuevo;
	size_t posc;
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
		iter->posc = posc;
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

unsigned int FNVHash(const char* str, size_t length) {
	const unsigned int fnv_prime = 0x811C9DC5;
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; str++, i++)
	{
		hash *= fnv_prime;
		hash ^= (*str);
	}

	return hash;
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
    hash->tabla = calloc(hash->capacidad,sizeof(lista_t*));
    if(hash->tabla == NULL){
        free(hash);
        return NULL;
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
	if ((float)hash->cantidad / hash->capacidad > FACTOR_CARGA_MIN && (float)hash->cantidad / hash->capacidad < FACTOR_CARGA_MAX) {
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
			if (lista_insertar_ultimo(tabla_nueva[indice],) == true)
		}
		// ME FALTA COMPLETAR ESTA PARTE; DEJAME LAS FUNCIONES HASTA HASH GUARDAR
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if (_redimensionar_hash(hash) == false)    return false;
    //falta aplicar algun criterio de redimension, de ser necesario, llamando una funcion
    size_t indice = FNVHash(clave, hash->capacidad); //mover esto a una funcion que solo me calcula el indice
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
        //creo un iterador y voy hasta la clave y la piso con el campo
    }
    return true;
}

void* hash_iterar_indice(hash_t *hash, const char *clave, lista_t* lista){
    lista_iter_t* iter = lista_iter_crear(lista);
    while(!lista_iter_al_final(iter)){
        campo_t* campo = lista_iter_ver_actual(iter);
        if(campo->clave == clave){
            lista_iter_destruir(iter);
            return campo->clave;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return NULL;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if(!hash_pertenece(hash, clave)){
        return NULL;
    }
    return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    if(!hash_pertenece(hash, clave)){
        return NULL;
    }else{
        size_t indice = FNVHash(clave, hash->capacidad);
        lista_t* lista = hash->tabla[indice];
        void* valor = hash_iterar_indice((hash_t *)hash, clave, lista);
        return valor;
    }
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t indice = FNVHash(clave, hash->capacidad);
    lista_t* lista = hash->tabla[indice];
    if(lista_esta_vacia(lista)){
        return false;
    }else{
        if(hash_iterar_indice((hash_t *)hash, clave, lista) != NULL){
            return true;
        }
    }
    return false;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

