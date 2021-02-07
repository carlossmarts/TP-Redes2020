#include "../lib/libClient.h"
//include

/* --------------------------------------------------------------------
                                MAIN
 --------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int cliente;
    inicarClientSocket(&cliente, argc, argv);

    int intentos = 3;       
    int logueado = 0;
    char nombre[1024];
    
    while (intentos > 0 && logueado == 0){
        printf("por favor ingrese sus datos\n-----------------------\n\n");
        login(cliente, nombre);
        logueado = recibirNumero(&cliente);
        if (logueado == 0){
            intentos --;
            system("clear");
            printf("fallo en el login, intentos restantes: %d\n\n\n", intentos);
        }
    }
    if (logueado == 0){
        system("clear");
        printf("intentos de login superados, desconectando del servidor\n");
        goto F;
    } //else
    printf("\n\nbienvenido %s\n\n", nombre);


    //Inicio de la aplicación
    menu(cliente, nombre);

	
    F:
    close(cliente);
    exit(0);
    return 0;
}
