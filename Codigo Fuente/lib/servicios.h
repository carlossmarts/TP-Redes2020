#ifndef SERVICIOS_H_INCLUDED
#define SERVICIOS_H_INCLUDED
#define TM 1
#define TT 2
#define TN 3
#define COLUMNAS 3
#define FILAS 20
#define MarDelPlata 1
#define BsAs 2
#define VERDADERO 1
#define FALSO 0
#define archivoServicios "servicios.bin"
#include <stdio.h>
#include <string.h>

typedef struct
{
    int id;
    int fecha;
    int turno;
    int partida;
    int asientos[COLUMNAS][FILAS];
} Servicio;

// Si no existe, guarda el servicio en el archivo binario
int guardarServicio(int fecha, int turno, int partida);

//busca en el binario si existe un servicio con los parametros brindados
int existeServicio(int fecha, int turno, int partida);

//Genera el struct servicio con su id, los parámetros brindados y los asientos vacíos
Servicio completarServicio(int fecha, int turno, int partida);

//trae el ultimo id registrado en el archivo binario
int traerUltimoID();

//trae el servicio por ID
int traerServicio(int id, Servicio *servicio);

// Trae todos los servicios que cumplan con los parámetros enviados.
//Si no se quiere filtrar por un parametro, se envía 0 - total es el total de numeros del array
Servicio *buscarServicios(int fecha, int turno, int partida, int *total);

//trae todos los servicios - usado para testear - total es el total de numeros del array
// Servicio *traerTodosServicios( int *total);

//hace la reserva
int hacerReserva(int id, int fila, int numero, int reserva);

// void mostrarTodosLosServicios();

//void registrarServicioCreado(char *nombre);

#endif // FUNCIONES_H_INCLUDED
