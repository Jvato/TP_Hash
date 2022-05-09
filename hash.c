#include "hash.h"
#include "lista.h"
#include <string.h>

typedef struct hash {
	lista_t **tabla;
	size_t capacidad;
	size_t cantidad;
} hash_t;

typedef struct campo {
	char *clave;
	void *dato;
} campo_t;

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

