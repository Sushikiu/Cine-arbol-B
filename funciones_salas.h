#ifndef FUNCIONES_SALAS_H
#define FUNCIONES_SALAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estructura para los espectadores
struct cliente
{
    char nombre[30];
    int numasiento;
    int edad;
    struct cliente *ant;
    struct cliente *sig;
};

// Estructura para las salas
struct salas
{
    char clave[4];              // Clave de la sala
    int numsala;                // Número de sala
    char pelicula[40];          // Nombre de la película
    char clasificacion[4];      // Clasificación de la película
    char tipo[12];              // Tipo de sala (Tradicional o VIP)
    struct cliente *espectadores; // Lista de espectadores
    int numespectadores;        // Número de espectadores
    long posicionArchivo;
};

// Funciones para manejar salas y espectadores
struct salas *agregarSala();
void imprimirSala(struct salas *sala);
void liberarSala(struct salas *sala);
struct cliente *agregarEspectador(struct salas *sala);
void eliminarEspectador(struct salas *sala, char nombre[30]);
void convertirMayusculas(char *cadena);

// Función para convertir una cadena a mayúsculas
void convertirMayusculas(char *cadena)
{
    for (int i = 0; cadena[i]; i++)
    {
        cadena[i] = toupper(cadena[i]);
    }
}

// Función para agregar una nueva sala
struct salas *agregarSala()
{
    struct salas *nuevasala = (struct salas *)malloc(sizeof(struct salas));
    if (nuevasala == NULL)
    {
        printf("Error de asignacion de memoria.\n");
        return NULL;
    }

    printf("\nDame el numero de sala (1-10): ");
    fflush(stdin);
    scanf("%d", &nuevasala->numsala);
    if (nuevasala->numsala < 1 || nuevasala->numsala > 10)
    {
        printf("Numero de sala fuera de rango.\n");
        free(nuevasala);
        return NULL;
    }

    printf("Dime el nombre de la pelicula a agregar: ");
    fflush(stdin);
    gets(nuevasala->pelicula);
    convertirMayusculas(nuevasala->pelicula);

    // Generamos la clave de la sala
    sprintf(nuevasala->clave, "%d", nuevasala->numsala);

    // Validar clasificación
    int clasificacionValida = 0;
    do
    {
        printf("Ingresa la clasificacion de la pelicula (A, B, B15, C): ");
        fflush(stdin);
        scanf("%s", nuevasala->clasificacion);
        convertirMayusculas(nuevasala->clasificacion);
        clasificacionValida = validarClasificacion(nuevasala->clasificacion);
    } while (!clasificacionValida);

    // Tipo de sala
    int tipodesala = 0;
    printf("Dime el tipo de sala\n  1. Tradicional\n  2. VIP\n");
    fflush(stdin);
    scanf("%d", &tipodesala);
    if (tipodesala == 1)
        strcpy(nuevasala->tipo, "TRADICIONAL");
    else if (tipodesala == 2)
        strcpy(nuevasala->tipo, "VIP");
    else
    {
        printf("Opcion no valida, no se pudo completar el registro.\n");
        free(nuevasala);
        return NULL;
    }

    nuevasala->numespectadores = 0;
    nuevasala->espectadores = NULL;

    return nuevasala;
}

// Función para imprimir una sala y sus espectadores
void imprimirSala(struct salas *sala)
{
    if (sala == NULL)
    {
        printf("Sala no encontrada.\n");
        return;
    }

    printf("\nPelicula: %s \tNum. de sala: %d \tClasificacion: %s \tTipo de sala: %s\n",
           sala->pelicula, sala->numsala, sala->clasificacion, sala->tipo);

    if (sala->espectadores == NULL)
    {
        printf("\n\tNo hay espectadores para mostrar\n");
    }
    else
    {
        printf("\n\tEspectadores:");
        struct cliente *aux = sala->espectadores;
        while (aux != NULL)
        {
            printf("\n\tNombre: %s\tNum. de asiento: %d\tEdad: %d",
                   aux->nombre, aux->numasiento, aux->edad);
            aux = aux->sig;
        }
        printf("\n");
    }
}

// Función para liberar la memoria de una sala y sus espectadores
void liberarSala(struct salas *sala)
{
    if (sala == NULL)
        return;

    struct cliente *aux = sala->espectadores;
    while (aux != NULL)
    {
        struct cliente *temp = aux;
        aux = aux->sig;
        free(temp);
    }
    free(sala);
}

// Función para agregar un espectador a una sala
struct cliente *agregarEspectador(struct salas *sala)
{
    if (sala->numespectadores >= 30)
    {
        printf("Lo siento, la sala esta llena.\n");
        return NULL;
    }

    struct cliente *nuevoespectador = (struct cliente *)malloc(sizeof(struct cliente));
    if (nuevoespectador == NULL)
    {
        printf("Error de asignacion de memoria.\n");
        return NULL;
    }

    printf("Dime tu nombre: ");
    fflush(stdin);
    gets(nuevoespectador->nombre);
    convertirMayusculas(nuevoespectador->nombre);

    do
    {
        printf("Dime el numero de asiento que quieres (1-30): ");
        fflush(stdin);
        scanf("%d", &nuevoespectador->numasiento);
        if (nuevoespectador->numasiento < 1 || nuevoespectador->numasiento > 30)
        {
            printf("Numero de asiento fuera de rango.\n");
        }
    } while (nuevoespectador->numasiento < 1 || nuevoespectador->numasiento > 30);

    do
    {
        printf("Dime tu edad: ");
        fflush(stdin);
        if (scanf("%d", &nuevoespectador->edad) != 1)
        {
            printf("Entrada no valida, debe ser un numero.\n");
            fflush(stdin);
            nuevoespectador->edad = 0;
        }
        else if (nuevoespectador->edad < 1 || nuevoespectador->edad > 100)
        {
            printf("Edad fuera de rango real.\n");
            nuevoespectador->edad = 0;
        }
    } while (nuevoespectador->edad == 0);

    // Validar edad según clasificación
    int edadPermitida = 0;
    if (strcmp(sala->clasificacion, "A") == 0)
    {
        edadPermitida = 1;
    }
    else if (strcmp(sala->clasificacion, "B") == 0 && nuevoespectador->edad >= 12)
    {
        edadPermitida = 1;
    }
    else if (strcmp(sala->clasificacion, "B15") == 0 && nuevoespectador->edad >= 15)
    {
        edadPermitida = 1;
    }
    else if (strcmp(sala->clasificacion, "C") == 0 && nuevoespectador->edad >= 18)
    {
        edadPermitida = 1;
    }

    if (!edadPermitida)
    {
        printf("Lo siento, no puedes entrar a esa funcion por tu edad.\n");
        free(nuevoespectador);
        return NULL;
    }

    // Agregar espectador a la lista de espectadores de la sala
    nuevoespectador->sig = sala->espectadores;
    if (sala->espectadores != NULL)
        sala->espectadores->ant = nuevoespectador;
    nuevoespectador->ant = NULL;
    sala->espectadores = nuevoespectador;
    sala->numespectadores++;

    return nuevoespectador;
}

// Función para eliminar un espectador de una sala
void eliminarEspectador(struct salas *sala, char nombre[30])
{
    if (sala->espectadores == NULL)
    {
        printf("No hay espectadores en la sala.\n");
        return;
    }

    struct cliente *aux = sala->espectadores;

    while (aux != NULL)
    {
        if (strcmp(aux->nombre, nombre) == 0)
        {
            if (aux->ant == NULL && aux->sig == NULL)
            {
                sala->espectadores = NULL;
            }
            else if (aux->ant == NULL)
            {
                sala->espectadores = aux->sig;
                sala->espectadores->ant = NULL;
            }
            else if (aux->sig == NULL)
            {
                aux->ant->sig = NULL;
            }
            else
            {
                aux->ant->sig = aux->sig;
                aux->sig->ant = aux->ant;
            }
            free(aux);
            sala->numespectadores--;
            printf("Espectador eliminado correctamente.\n");
            return;
        }
        aux = aux->sig;
    }
    printf("Espectador no encontrado.\n");
}

// Función para validar la clasificación
int validarClasificacion(char clasificacion[4])
{
    if (strcmp(clasificacion, "A") == 0 ||
        strcmp(clasificacion, "B") == 0 ||
        strcmp(clasificacion, "B15") == 0 ||
        strcmp(clasificacion, "C") == 0)
    {
        return 1; // Clasificación válida
    }
    printf("Clasificacion no valida. Las opciones permitidas son: A, B, B15, C.\n");
    return 0; // Clasificación no válida
}


#endif // FUNCIONES_SALAS_H
