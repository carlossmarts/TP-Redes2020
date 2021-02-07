#include "../lib/libServer.h"
#include <time.h>

/* --------------------------------------------------------------------
                                MAIN
 --------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    //declaración de variables
    int servidor, cliente;
    if (argc < 2)
        error("Error, falta especificar puerto");
    int puerto = atoi(argv[1]);
    int err; //variable para errores de timeout

    //Inicialización
    iniciarServerSocket(&servidor, puerto);
    int cant = cantidadUsuarios();
    struct usuario usuarios[cant];
    cargarUsuarios(usuarios); //la carga devuelve el largo del array

S:
    //esperar cliente
    printf("servidor a la escucha en PUERTO %d\n", puerto);
    listen((servidor), 5);
    //Aceptar cliente
    aceptarCliente(&cliente, &servidor);

    //intentos de login
    int intentos = 3;
    int logueado = 0;
    char nombre[1024];
    err = 0;

    while (logueado == 0 && intentos > 0)
    {
        printf("intento de login numero: %d\n", (3 + 1 - intentos));
        logueado = login(cliente, usuarios, cant, nombre, &err);
        if (err == TIMEOUTVAL)
            goto P;
        intentos--;
        // registrarLoginUsuario(nombre);
        enviarNumero(&cliente, logueado); //le avisa al cliente si encontro el usuario o no
    }
    if (logueado == 0)
    {
        printf("intentos de login superados, cliente rechazado\n\n");
        goto P;
    }
    else
    {
        registrarLoginUsuario(nombre);
    }
    printf("\nUsuario Conectado: %s\n\n", nombre);
    //else

I: //empty
    //comienzo de la aplicación

    printf("Esperando operacion - ");
    int idOperacion = recibirOperacion(cliente, &err);
    if (err == TIMEOUTVAL)
    {
        serverLog("Usuario desconectado por tiempo de espera\n");
        userLog(nombre, "Desconectado por tiempo de espera\n");
        goto P;
    }

    printf("Operacion: %d \n", idOperacion);
    switch (idOperacion)
    {
    case 0:
        registrarLogoutUsuario(nombre);
        //enviarTexto(&cliente, "Sesion Finalizada");
        goto P;
        break;
    case 1:
        altaServicio(cliente, nombre, &err);
        break;
    case 2:
        mostrarListadoServicios(cliente, nombre, &err);
        break;
    case 3:
        enviarLogCliente(cliente, nombre, &err);
        break;
    case 4:
        mostrarAsientos(cliente, nombre, &err);
        break;
    default:
        break;
    }
    if (err == TIMEOUTVAL)
    {
        serverLog("usuario desconectado por tiempo de espera");
        userLog(nombre, "desconectado por tiempo de espera");
        goto P;
    }
    goto I;

P:
    printf("\n\n----------------------------------");
    printf("\nfin de conexion con un cliente\n");
    goto S;

    close(cliente);
    close(servidor);
    return 0;
}
