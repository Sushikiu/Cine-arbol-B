#ifndef FUNCIONES_ARCHIVOS_H
#define FUNCIONES_ARCHIVOS_H

#include "funciones_salas.h"

// Estructura para los registros en el árbol B
struct registro
{
    char clave[4];          // Clave de la sala
    long posicionArchivo;   // Posición en el archivo donde se encuentra la sala
};

// Estructura para los nodos del árbol B
struct hoja
{
    int numRegistrosEnHoja;         // Número de registros en la hoja
    struct registro registros[4];   // Registros almacenados en la hoja
    struct hoja *llaves[5];         // Punteros a hijos
    int esHoja;                     // Indicador si es hoja (1) o no (0)
};

// Funciones para manejo del árbol B y archivos
long guardarSalaEnArchivo(FILE *archivo, struct salas *sala);
struct salas *cargarSalaDesdeArchivo(FILE *archivo, long posicion);
void actualizarSalaEnArchivo(FILE *archivo, struct salas *sala);
struct hoja *insertarEnArbolB(struct hoja *raiz, struct registro nuevoRegistro);
struct salas *buscarEnArbolB(struct hoja *raiz, char clave[4], FILE *archivo);
void guardarArbolBEnArchivo(FILE *archivoArbol, struct hoja *nodo);
struct hoja *cargarArbolBDesdeArchivo(FILE *archivoArbol);
void liberarArbolB(struct hoja *nodo);
struct hoja *eliminarDeArbolB(struct hoja *raiz, char clave[4], FILE *archivo);
void buscarEspectadorEnTodasLasSalas(struct hoja *raiz, char nombre[30], FILE *archivo);

// Función para guardar una sala en el archivo y retornar su posición
long guardarSalaEnArchivo(FILE *archivo, struct salas *sala)
{
    fseek(archivo, 0, SEEK_END); // Nos movemos al final del archivo
    long posicion = ftell(archivo); // Obtenemos la posición actual

    fwrite(sala, sizeof(struct salas), 1, archivo);

    // Guardar los espectadores asociados a la sala
    struct cliente *clienteActual = sala->espectadores;
    while (clienteActual != NULL)
    {
        fwrite(clienteActual, sizeof(struct cliente), 1, archivo);
        clienteActual = clienteActual->sig;
    }
    return posicion;
}

// Función para cargar una sala desde el archivo en una posición dada
struct salas *cargarSalaDesdeArchivo(FILE *archivo, long posicion)
{
    fseek(archivo, posicion, SEEK_SET); // Nos movemos a la posición de la sala
    struct salas *sala = (struct salas *)malloc(sizeof(struct salas));
    if (sala == NULL)
    {
        printf("Error de asignacion de memoria.\n");
        return NULL;
    }
    fread(sala, sizeof(struct salas), 1, archivo);

    // Cargar los espectadores asociados a la sala
    sala->espectadores = NULL;
    struct cliente *ultimoCliente = NULL;
    for (int i = 0; i < sala->numespectadores; i++)
    {
        struct cliente *nuevoCliente = (struct cliente *)malloc(sizeof(struct cliente));
        if (nuevoCliente == NULL)
        {
            printf("Error de asignacion de memoria.\n");
            liberarSala(sala);
            return NULL;
        }
        fread(nuevoCliente, sizeof(struct cliente), 1, archivo);
        nuevoCliente->sig = NULL;

        if (sala->espectadores == NULL)
        {
            sala->espectadores = nuevoCliente;
            nuevoCliente->ant = NULL;
        }
        else
        {
            ultimoCliente->sig = nuevoCliente;
            nuevoCliente->ant = ultimoCliente;
        }
        ultimoCliente = nuevoCliente;
    }
    return sala;
}

// Función para actualizar una sala en el archivo
void actualizarSalaEnArchivo(FILE *archivo, struct salas *sala)
{
    // Buscar la posición de la sala en el archivo
    // Asumimos que la posición está almacenada en sala->posicionArchivo (necesitas agregar este campo)
    long posicion = sala->posicionArchivo;
    fseek(archivo, posicion, SEEK_SET);

    fwrite(sala, sizeof(struct salas), 1, archivo);

    // Actualizar los espectadores asociados a la sala
    struct cliente *clienteActual = sala->espectadores;
    while (clienteActual != NULL)
    {
        fwrite(clienteActual, sizeof(struct cliente), 1, archivo);
        clienteActual = clienteActual->sig;
    }
}

// Función para crear una nueva hoja
struct hoja *crearHoja()
{
    struct hoja *nuevaHoja = (struct hoja *)malloc(sizeof(struct hoja));
    nuevaHoja->numRegistrosEnHoja = 0;
    nuevaHoja->esHoja = 1;
    memset(nuevaHoja->llaves, 0, sizeof(nuevaHoja->llaves));
    return nuevaHoja;
}

// Función simplificada para insertar en el árbol B
struct hoja *insertarEnArbolB(struct hoja *raiz, struct registro nuevoRegistro)
{
    if (raiz == NULL)
    {
        raiz = crearHoja();
        raiz->registros[0] = nuevoRegistro;
        raiz->numRegistrosEnHoja = 1;
        return raiz;
    }

    // Por simplicidad, agregamos el registro al final si hay espacio
    if (raiz->numRegistrosEnHoja < 4)
    {
        raiz->registros[raiz->numRegistrosEnHoja] = nuevoRegistro;
        raiz->numRegistrosEnHoja++;
    }
    else
    {
        printf("Necesita implementar división de nodos.\n");
        // Aquí deberías implementar la división de nodos en un árbol B real
    }
    return raiz;
}

// Función para buscar una sala en el árbol B
struct salas *buscarEnArbolB(struct hoja *raiz, char clave[4], FILE *archivo)
{
    int i;
    if (raiz == NULL)
    {
        return NULL;
    }

    // Recorremos los registros en la hoja
    for (i = 0; i < raiz->numRegistrosEnHoja; i++)
    {
        if (strcmp(raiz->registros[i].clave, clave) == 0)
        {
            // Encontramos la clave, cargamos la sala desde el archivo
            return cargarSalaDesdeArchivo(archivo, raiz->registros[i].posicionArchivo);
        }
    }

    // Si no es hoja, buscamos en los hijos correspondientes (no implementado aquí)
    return NULL;
}

// Función para guardar el árbol B en un archivo
void guardarArbolBEnArchivo(FILE *archivoArbol, struct hoja *nodo)
{
    if (nodo == NULL)
        return;

    fwrite(&(nodo->numRegistrosEnHoja), sizeof(int), 1, archivoArbol);
    fwrite(&(nodo->esHoja), sizeof(int), 1, archivoArbol);
    fwrite(nodo->registros, sizeof(struct registro), nodo->numRegistrosEnHoja, archivoArbol);

    if (!nodo->esHoja)
    {
        for (int i = 0; i <= nodo->numRegistrosEnHoja; i++)
        {
            guardarArbolBEnArchivo(archivoArbol, nodo->llaves[i]);
        }
    }
}

// Función para cargar el árbol B desde un archivo
struct hoja *cargarArbolBDesdeArchivo(FILE *archivoArbol)
{
    struct hoja *nodo = (struct hoja *)malloc(sizeof(struct hoja));
    if (fread(&(nodo->numRegistrosEnHoja), sizeof(int), 1, archivoArbol) != 1)
    {
        free(nodo);
        return NULL;
    }
    fread(&(nodo->esHoja), sizeof(int), 1, archivoArbol);
    fread(nodo->registros, sizeof(struct registro), nodo->numRegistrosEnHoja, archivoArbol);

    if (!nodo->esHoja)
    {
        for (int i = 0; i <= nodo->numRegistrosEnHoja; i++)
        {
            nodo->llaves[i] = cargarArbolBDesdeArchivo(archivoArbol);
        }
    }
    else
    {
        for (int i = 0; i <= nodo->numRegistrosEnHoja; i++)
        {
            nodo->llaves[i] = NULL;
        }
    }
    return nodo;
}

// Función para liberar la memoria del árbol B
void liberarArbolB(struct hoja *nodo)
{
    if (nodo == NULL)
        return;

    if (!nodo->esHoja)
    {
        for (int i = 0; i <= nodo->numRegistrosEnHoja; i++)
        {
            liberarArbolB(nodo->llaves[i]);
        }
    }
    free(nodo);
}

// Función para eliminar una sala del árbol B y del archivo
struct hoja *eliminarDeArbolB(struct hoja *raiz, char clave[4], FILE *archivo)
{
    if (raiz == NULL)
    {
        printf("La pelicula no existe en el sistema.\n");
        return NULL;
    }

    int i;
    for (i = 0; i < raiz->numRegistrosEnHoja; i++)
    {
        if (strcmp(raiz->registros[i].clave, clave) == 0)
        {
            // Cargamos la sala para verificar si tiene espectadores
            struct salas *sala = cargarSalaDesdeArchivo(archivo, raiz->registros[i].posicionArchivo);
            if (sala->numespectadores > 0)
            {
                printf("No se puede eliminar la pelicula porque tiene espectadores.\n");
                liberarSala(sala);
                return raiz;
            }

            // Eliminamos la sala del archivo (opcional, requiere manejo adicional)
            // Aquí podrías marcar la sala como eliminada o reusar el espacio

            // Eliminamos el registro del árbol B
            for (int j = i; j < raiz->numRegistrosEnHoja - 1; j++)
            {
                raiz->registros[j] = raiz->registros[j + 1];
            }
            raiz->numRegistrosEnHoja--;

            printf("Pelicula eliminada exitosamente.\n");
            liberarSala(sala);
            return raiz;
        }
    }

    // Si no es hoja, buscar en los hijos correspondientes (no implementado aquí)
    printf("La pelicula no existe en el sistema.\n");
    return raiz;
}

// Función para buscar un espectador en todas las salas
void buscarEspectadorEnTodasLasSalas(struct hoja *raiz, char nombre[30], FILE *archivo)
{
    if (raiz == NULL)
    {
        printf("No hay peliculas registradas.\n");
        return;
    }

    int encontrado = 0;

    // Recorremos los registros en la hoja
    for (int i = 0; i < raiz->numRegistrosEnHoja; i++)
    {
        struct salas *sala = cargarSalaDesdeArchivo(archivo, raiz->registros[i].posicionArchivo);
        struct cliente *aux = sala->espectadores;

        while (aux != NULL)
        {
            if (strcmp(aux->nombre, nombre) == 0)
            {
                printf("\nPersona encontrada:\nNombre: %s\tAsiento: %d\tEdad: %d\tPelicula: %s",
                       aux->nombre, aux->numasiento, aux->edad, sala->pelicula);
                encontrado = 1;
                liberarSala(sala);
                break;
            }
            aux = aux->sig;
        }
        if (encontrado)
            break;

        liberarSala(sala);
    }

    if (!encontrado)
    {
        printf("\nPersona no encontrada.\n");
    }
}


#endif // FUNCIONES_ARCHIVOS_H
