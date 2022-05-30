#include "hash.h"
#include "lista.h"
#include <string.h>
#include <stdio.h>
#define TAMANIO_INICIAL 100
#define FACTOR_DE_CARGA 2
#define FACTOR_AUMENTAR_MEMORIA 2


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
	lista_iter_t* lista_iter;
	size_t posc;
	hash_t* hash;
};

int  _siguiente_elemento_valido(const hash_t * hash, int inicio) {
	int i;
	for (i = inicio; i < hash->capacidad; i++) {
		if (lista_esta_vacia(hash->tabla[i]) == false){
			return i;
		}
	}
	return -1;
}

void _vaciar_vector_con_listas_vacias(lista_t **tabla, size_t largo) {
        for (size_t i = 0; i < largo; i++) {
                lista_destruir(tabla[i],NULL);
        }
}

bool _inicializar_tabla_con_listas_vacias(lista_t **tabla,size_t largo) {
	for (size_t i = 0; i < largo; i++){
		lista_t *lista = lista_crear();
		if (lista == NULL) {
            _vaciar_vector_con_listas_vacias(tabla,i);
			return false;	
        }
		tabla[i] = lista;
	}
	return true;
}

hash_iter_t *hash_iter_crear(const hash_t *hash) {
	int posc;
	lista_iter_t* lista_iter;
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (iter == NULL){
		return  NULL;
	}
	if (hash->cantidad == 0) {
		lista_iter = NULL;
		posc = 0;
	}
	else {
		posc = _siguiente_elemento_valido(hash,0);
		lista_iter = lista_iter_crear(hash->tabla[posc]);
		if (lista_iter == NULL) {
			free(iter);
			return NULL;
		}
	} 
	iter->hash = (hash_t*)hash;
	iter->lista_iter = lista_iter;
	iter->posc = (size_t)posc;
	return iter;
}


bool hash_iter_avanzar(hash_iter_t *iter) {
	lista_iter_t* lista_iter_nuevo;
	int posc;
	if (hash_iter_al_final(iter) == true){
		return false;
	}
	if (lista_iter_avanzar(iter->lista_iter) == false){
		return false;
	}
	if (lista_iter_al_final(iter->lista_iter) == true) {
		posc = _siguiente_elemento_valido(iter->hash,(int)iter->posc + 1);
		if (posc == -1){
			return true;
		}
		lista_iter_destruir(iter->lista_iter);
		lista_iter_nuevo = lista_iter_crear(iter->hash->tabla[posc]);
		if (lista_iter_nuevo == NULL){
			return false;
		}
		iter->lista_iter = lista_iter_nuevo;
		iter->posc = (size_t)posc;
	}
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter) {
	if (hash_iter_al_final(iter) == true){
		return NULL;
	}
	campo_t * campo = lista_iter_ver_actual(iter->lista_iter);
	if (campo == NULL){
		return NULL;
	}
	return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter) {
	return (iter->lista_iter == NULL || lista_iter_al_final(iter->lista_iter));
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

size_t FNVHash_normalizada(const char* clave, size_t capacidad) {
	const unsigned int fnv_prime = 0x811C9DC5;
	size_t hash = 0;
	size_t i = 0;
	size_t largo_cadena = strlen(clave);
	for (i = 0; i < largo_cadena; clave++, i++){
		hash *= fnv_prime;
		hash = hash^(size_t)(*clave);
	}
	
	return hash%capacidad;
}

hash_t *_hash_crear(size_t capacidad, hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if(hash == NULL){
		return NULL;
	}

    hash->tabla = malloc(capacidad*sizeof(lista_t*));
    if(hash->tabla == NULL){
        free(hash);
        return NULL;
    }
    if (!_inicializar_tabla_con_listas_vacias(hash->tabla,capacidad)) {
	    free(hash->tabla);
	    free(hash);
        return NULL;
	}
    hash->funcion_destruccion = destruir_dato;
    hash->cantidad = 0; 
    hash->capacidad = capacidad;
    return hash;
}


hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {
    return _hash_crear(TAMANIO_INICIAL,destruir_dato);
}
campo_t* crear_campo(const char * clave, void *dato) {
	campo_t* campo = malloc(sizeof(campo_t));
	if (campo == NULL){
		return NULL;
	}
	campo->clave = malloc(strlen(clave)+1);
	if (campo->clave == NULL) {
		free(campo);
		return NULL;
	}
	strcpy(campo->clave,clave);
	campo->dato = dato;
	return campo;
}

void campo_destruir(campo_t * campo) {
	free(campo->clave);
	free(campo);
}

bool _hash_rehashear_nueva_tabla(hash_t* hash, hash_t* hash_aux) {
	for (size_t i = 0; i < hash->capacidad; i++) {
		if (lista_esta_vacia(hash->tabla[i])){
			continue;
		}
		lista_iter_t* lista_iter = lista_iter_crear(hash->tabla[i]);
		if (lista_iter == NULL){
			return false;
		}
		while (lista_iter_al_final(lista_iter) == false) {
			campo_t* campo_actual = lista_iter_ver_actual(lista_iter);
			campo_t* campo_aux = crear_campo(campo_actual->clave,campo_actual->dato);
			if (campo_aux == NULL) {
				lista_iter_destruir(lista_iter);
				return false;
			}
			size_t indice = FNVHash_normalizada(campo_aux->clave,hash_aux->capacidad);
			if (lista_insertar_ultimo(hash_aux->tabla[indice],campo_aux) == false) {
				campo_destruir(campo_aux);
				lista_iter_destruir(lista_iter);
				return false;
			}
			hash_aux->cantidad++;
			if (lista_iter_avanzar(lista_iter) == false) {
				lista_iter_destruir(lista_iter);
				return false;
			}
		}
		lista_iter_destruir(lista_iter);
	}
	return true;
}

void _hash_swap_tabla(hash_t *hash_destino, hash_t *hash_origen) {
	lista_t **tabla_aux = hash_origen->tabla;
	size_t capacidad_aux = hash_origen->capacidad;
	size_t cantidad_aux = hash_origen->cantidad;

	hash_origen->tabla = hash_destino->tabla;
	hash_origen->capacidad = hash_destino->capacidad;
	hash_origen->cantidad = hash_destino->cantidad;

	hash_destino->tabla = tabla_aux;
	hash_destino->capacidad = capacidad_aux;
	hash_destino->cantidad = cantidad_aux;
}

bool _redimensionar_hash(hash_t* hash, size_t capacidad_nueva) {
	hash_t* hash_aux = _hash_crear(capacidad_nueva,NULL);
	if (hash_aux == NULL){
		return false;
	}
	if (_hash_rehashear_nueva_tabla(hash,hash_aux) == false) {
		hash_destruir(hash_aux);
		return false;
	}
	_hash_swap_tabla(hash,hash_aux);
	hash_destruir(hash_aux);
	return true;
}

lista_iter_t* _hash_lista_iter_buscar_clave(const hash_t *hash, const char *clave) {
	size_t indice = FNVHash_normalizada(clave, hash->capacidad);
	lista_iter_t* iter = lista_iter_crear(hash->tabla[indice]);
	if (iter == NULL) {
		return NULL;
	}
	while(!lista_iter_al_final(iter)) {
		campo_t* campo = lista_iter_ver_actual(iter);
		if (strcmp(campo->clave,clave) == 0) {
			return iter;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return NULL;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	lista_iter_t* lista_iter = _hash_lista_iter_buscar_clave(hash,clave);
	if (lista_iter == NULL) {
		if (hash->cantidad/hash->capacidad >= FACTOR_DE_CARGA) {
			if (_redimensionar_hash(hash,FACTOR_AUMENTAR_MEMORIA*hash->capacidad) == false) {
				return false;
			}
		}
		size_t indice = FNVHash_normalizada(clave,hash->capacidad);
		campo_t* campo = crear_campo(clave,dato);
		if (campo == NULL) {
			return false;
		}
		if (lista_insertar_ultimo(hash->tabla[indice],campo) == false) {
			campo_destruir(campo);
			return false;
		}
		hash->cantidad++;
	}
	else {
		campo_t* campo_actual = lista_iter_ver_actual(lista_iter);
		if (campo_actual == NULL) {
			lista_iter_destruir(lista_iter);
			return false;
		}
		if (hash->funcion_destruccion != NULL) {
			hash->funcion_destruccion(campo_actual->dato);
		}
		campo_actual->dato = dato;
		lista_iter_destruir(lista_iter);
	}
	return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
	if(!hash_pertenece(hash, clave)){
		return NULL;
	}
	if (hash->capacidad/hash->cantidad >= FACTOR_DE_CARGA && hash->capacidad >= TAMANIO_INICIAL) {
		if (_redimensionar_hash(hash,hash->capacidad/FACTOR_DE_CARGA) == false) {
			return NULL;
		}
	}
	lista_iter_t* lista_iter = _hash_lista_iter_buscar_clave(hash, clave);
	if (lista_iter == NULL) {
		return NULL;
	}
	campo_t* campo = lista_iter_borrar(lista_iter);
	hash->cantidad--;
	void* dato = campo->dato;
	campo_destruir(campo);
	lista_iter_destruir(lista_iter);
	return dato;
}

void *hash_obtener(const hash_t *hash, const char *clave){
	if(!hash_pertenece(hash, clave)){
		return NULL;
	}
	lista_iter_t* lista_iter = _hash_lista_iter_buscar_clave(hash, clave);
	if (lista_iter == NULL) {
		return NULL;
	}
	campo_t* campo = lista_iter_ver_actual(lista_iter);
	lista_iter_destruir(lista_iter);
	return campo->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	lista_iter_t* lista_iter = _hash_lista_iter_buscar_clave(hash,clave);
	if (lista_iter == NULL) {
		return false;
	}
	lista_iter_destruir(lista_iter);
	return true;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void hash_destruir(hash_t *hash){
	for(size_t i = 0; i < hash->capacidad; i++){
		lista_t* lista = hash->tabla[i];
		lista_iter_t* lista_iter = lista_iter_crear(lista);
		while (lista_iter_al_final(lista_iter) == false) {
			campo_t* campo = lista_iter_ver_actual(lista_iter);
			if (hash->funcion_destruccion != NULL){
				hash->funcion_destruccion(campo->dato);
			}
			campo_destruir((campo_t *)lista_iter_borrar(lista_iter));
		}
		lista_destruir(lista,NULL);
		lista_iter_destruir(lista_iter);
	}
	free(hash->tabla);
	free(hash);
}
