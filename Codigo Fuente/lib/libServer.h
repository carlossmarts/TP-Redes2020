#ifndef LIBSERVER
#define LIBSERVER

#define TIMEOUTVAL 120

//importar librerías
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <time.h>
#include "servicios.h"

/* --------------------------------------------------------------------
                                ESTRUCTURAS
 --------------------------------------------------------------------*/
struct usuario
{
    char nombre[1024];
    char contr[1024];
};


/* --------------------------------------------------------------------
                                FUNCIONES
 --------------------------------------------------------------------*/
//-------------------
// Manejo de errores
//-------------------
void error(const char *msg);

//--------------------------
//Inicialización del server
//--------------------------
void iniciarServerSocket(int *servidor, int puerto);

//--------------------------
//aceptar cliente
//--------------------------
void aceptarCliente(int * cliente, int * servidor);

/*-------
Enviar
--------*/
void enviarTexto(int *cliente, char *texto);

void enviarNumero(int *cliente, int num);

/*-------
recibir
--------*/
void recibirTexto(int *cliente, char *texto, int * err);

int recibirNumero(int *cliente, int * err);

/*-----------------------------------------------------------------------------
Cargar usuarios
Aca habría que implementar la carga de los usuarios desde un archivo de texto
-----------------------------------------------------------------------------*/
void cargarUsuarios(struct usuario *usuarios);

/*--------------------------------
varificar usuario
devuelve 1 si encontró el usuario
---------------------------------*/
int verificarNombre(const char *nombre, struct usuario *usuarios, int cant);

int verificarUsuario(const char *user, const char *pass, struct usuario *usuarios, int cant);

/*--------------------------------
login
devuelve 1 si se logueo
---------------------------------*/
int login (int cliente, struct usuario *usuarios, int cant, char *nombre, int * err);

/*--------------------------------
Cantidad de usuarios
devuelve la cantidad de usuarios guardados
---------------------------------*/
int cantidadUsuarios();

/*--------------------------------
altaServicio
maneja los servicios
---------------------------------*/
int altaServicio(int cliente, char *usuario, int * err);

/*................................
Llamar a operaciones del servidor
................................*/
int recibirOperacion (int cliente, int * err);
/*VASO DE USO 3*/
void gestionarAsiento(int cliente, char * usuario, int * err);
/*CASO DE USO 2*/
void mostrarListadoServicios(int cliente, char * usuario, int * err);

void enviarLogCliente(int cliente, char * usuario, int * err);

void enviarAsientos(int cliente, int id);

void mostrarAsientos(int cliente, char * usuario, int * err);

void enviarArrayAsientos(int *cliente, int *num);


//----------------------
//anotaciones en log
//----------------------
//esta falta agregarle la escritura en el archivo de texto
void nota(const char *path, const char *msg);

void obtenerFecha(char *fecha);

void obtenerHora(char *hora);

void serverLog(char *msg);

void userLog(char *path, char *msg);

void registrarInicioServidor(int puerto);

void registrarLoginUsuario(char *nombre);

void registrarLogoutUsuario(char *nombre);

void registrarAltaServicio(char* usuario,int idServicio);

void registrarGestionDeAsiento(char* usuario,int idServicio, int fila, int columna, int operacion);
#endif