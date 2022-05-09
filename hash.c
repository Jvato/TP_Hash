#include "hash.h"
#include "lista.h"

#define TAMANIO_INICIAL 100

typedef struct campo{
    char* clave;
    void* dato;
}campo_t;

struct hash{
    size_t cantidad;
    size_t capacidad;
    lista_t** tabla;
    hash_destruir_dato_t* funcion_destruccion;
};

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

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* hash = malloc(sizeof(hash_t));
    if(hash == NULL){
        return NULL;
    }

    hash->funcion_destruccion = destruir_dato;
    hash->cantidad = 0;
    hash->cantidad = TAMANIO_INICIAL;
    hash->tabla = malloc(hash->cantidad * sizeof(void*));
    if(hash->tabla == NULL){
        free(hash);
        return NULL;
    }
    //aca faltaria rellenar la tabla con listas vacias

    return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    //falta aplicar algun criterio de redimension, de ser necesario, llamando una funcion
    size_t indice = FNVHash(clave, hash->capacidad); //mover esto a una funcion que solo me calcula el indice
    campo_t* campo = malloc(sizeof(campo_t));
    if(campo == NULL){
        return false;
    }
    // aca me tendria que guardar la clave con un malloc, fijarse en la clase de hashes:implementacion
    // del 2022-02-29
    campo->clave = clave;
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
        void* valor = hash_iterar_indice(hash, clave, lista);
        return valor;
    }
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    size_t indice = FNVHash(clave, hash->capacidad);
    lista_t* lista = hash->tabla[indice];
    if(lista_esta_vacia(lista)){
        return false;
    }else{
        if(hash_iterar_indice(hash, clave, lista) != NULL){
            return true;
        }
    }
    return false;
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

void destruir_listas(){

}

void hash_destruir(hash_t *hash){

}

hash_iter_t *hash_iter_crear(const hash_t *hash){
    return NULL;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    return "aq";
}

bool hash_iter_al_final(const hash_iter_t *iter){
    return false;
}

void hash_iter_destruir(hash_iter_t *iter){

}