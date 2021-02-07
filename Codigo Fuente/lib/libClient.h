#ifndef LIBCLIENT
#define LIBCLIENT

//Importar librerías
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include "servicios.h"

/* --------------------------------------------------------------------
                                FUNCIONES
 --------------------------------------------------------------------*/

//-------------------
// Manejo de errores
//-------------------
void error(const char *msg);
//----------------------
//anotaciones en log
//----------------------
//esta falta agregarle la escritura en el archivo de texto
void nota(const char *msg);

//--------------------------
//Inicialización del cliente
//--------------------------

void inicarClientSocket (int * socketClient, int argc, char *argv[]);

/*-------
Enviar
--------*/
void enviarTexto(int * socket, char texto[1024]);
void enviarNumero(int * socket, int num);

/*-------
recibir
--------*/
void recibirEImprimirTexto(int * socket);

void recibirEImprimirNumero(int * socket);

int recibirNumero(int * socket);

void login(int cliente, char nombre[1024]);

/*................................
Lllamar a operaciones del servidor
................................*/
void llamarOperacion (int cliente,int idOperacion);


/* --------------------------------------------------------------------
                                MENUES
 --------------------------------------------------------------------*/
void altaServicioCliente(int cliente);
void gestionarPasajes(int cliente, char nombre[1024]);
void mostrarListadoServicios(int cliente, int partida, int dia, int mes, int anio, int turno, char nombre[1024]);
void mostrarServicio(int cliente, int idServicio, char nombre[1024]);
void gestionarPasajes(int cliente, char nombre[1024]);
void gestionarAsiento(int cliente, int idServicio, char fila, int columna, int operacion);
void mostrarRegistroDeactividades(int cliente, char* usuario);
void menu(int cliente, char nombre[1024]);
void solicitarLogCliente (int cliente, char *nombre);
void recibirEImprimirServicio(int *socket);
void recibirTexto(int *cliente, char *texto);
void recibirAsientos(int *socket, int *array);
void imprimirBus(int socket);
int getFila(char fila);
#endif