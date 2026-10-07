# Simulador de base de datos de cine (Árbol B en C)

> Proyecto final de tercer semestre: Sistema de gestión para un cine que implementa un **Árbol B** en memoria y persistencia en archivos binarios utilizando C.

## Tecnologías y conceptos
* **Lenguaje:** C estándar
* **Estructuras de datos:** Árbol B (B-Tree), Punteros, Estructuras (`struct`)
* **Manejo de archivos:** Archivos binarios (`fopen`, `fread`, `fwrite`, `fseek`) y persistencia en disco.

## Características
* Indexación de salas de cine mediante un Árbol B almacenado en `arbolB.dat`.
* Almacenamiento de salas y espectadores en el archivo binario `cine.dat` usando posiciones físicas en disco (`long`).
* Operaciones CRUD completas para películas, salas y espectadores.

## Compilación y Ejecución
Si estás usando GCC desde la terminal:
```bash
gcc main.c funciones_salas.h funciones_archivos.h -o cine
./cine
