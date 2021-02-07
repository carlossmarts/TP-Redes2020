#include "libClient.h"

/* --------------------------------------------------------------------
                           FUNCIONES GENERALES
 --------------------------------------------------------------------*/

//-------------------
// Manejo de errores
//-------------------
void error(const char *msg)
{
    printf("%s\n", msg);
    exit(1);
}

/* --------------------------------------------------------------------
                  FUNCIONES DE CONEXIÓN Y MENSAJES
 --------------------------------------------------------------------*/

//--------------------------
//Inicialización del cliente
//--------------------------

void inicarClientSocket(int *cliente, int argc, char *argv[])
{
    int puerto, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    puerto = atoi(argv[2]);
    *cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (*cliente < 0)
        error("error al crear el socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "error, servidor inactivo\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(puerto);

    if (connect(*cliente, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Error al conectar con servidor");
}

/*-------
Enviar
--------*/
void enviarTexto(int *socket, char texto[1024])
{
    int n = send(*socket, texto, 1024, 0);
    if (n < 0)
        error("Error al escribir mensaje");
}

void enviarNumero(int *socket, int num)
{
    int n = send(*socket, &num, 4, 0);
    if (n < 0)
        error("Error al escribir mensaje");
}

void llamarOperacion(int cliente, int idOperacion)
{
    enviarNumero(&cliente, idOperacion);
}

/*-------
recibir
--------*/

void recibirEImprimirServicio(int *socket)
{
    //serv_0 -> idServicio
    //serv_1 -> origen
    //serv_2 -> Fecha
    //serv_3 -> turno

    int serv[4];
    int n = recv(*socket, serv, 16, 0);
    if (n < 0)
        error("error al recibir mensaje");
    int fecha = serv[2];

    printf("Servicio %d ", serv[0]);
    printf("- Fecha: ");
    int dia = fecha % 100;
    int mes = (fecha % 10000) / 100;
    int anio = fecha / 10000;
    printf(" %d/%d/%d", dia, mes, anio);
    if (serv[1] == MarDelPlata)
    {
        printf(" - Origen: Mar del Plata");
    }
    else
    {
        printf(" - Origen: Buenos Aires");
    }
    if (serv[3] == TM)
    {
        printf(" - turno mañana");
    }
    else if (serv[3] == TT)
    {
        printf(" - turno tarde");
    }
    else
    {
        printf(" - turno noche");
    }
    printf("\n");
}

void recibirAsientos(int *socket, int *array)
{
    int n = recv(*socket, array, 80, 0);
    if (n < 0)
        error("error al recibir mensaje");
}

void recibirEImprimirTexto(int *socket)
{
    char m[1024];
    int n = recv(*socket, m, 1024, 0);
    if (n < 0)
        error("error al recibir mensaje");
    printf("%s\n", m);
}

void recibirEImprimirNumero(int *socket)
{
    int valor;
    int n = recv(*socket, &valor, 4, 0);
    if (n < 0)
        error("error al recibir mensaje");
    printf("%d\n", valor);
}

int recibirNumero(int *socket)
{
    int valor;
    int n = recv(*socket, &valor, 4, 0);
    if (n < 0)
        error("error al recibir mensaje");
    //printf("valor decibido: %d\n", valor);
    return valor;
}

void recibirTexto(int *cliente, char *texto)
{
    char m[1024];
    int n = recv(*cliente, m, 1024, 0);
    if (n < 0)
        error("error al recibir mensaje");
    strcpy(texto, m);
}

void login(int cliente, char nombre[1024])
{

    recibirEImprimirTexto(&cliente); //usuario:

    scanf("%s", nombre);
    enviarTexto(&cliente, nombre);

    int existeNombre = -1;
    existeNombre = recibirNumero(&cliente);
    recibirEImprimirTexto(&cliente); //contraseña o no existe nombre
    if (existeNombre == 1){
        char contr[1024];
        scanf("%s", contr);
        enviarTexto(&cliente, contr);
    }    
}
/* --------------------------------------------------------------------
                                MENUES
 --------------------------------------------------------------------*/
void altaServicioCliente(int cliente)
{
    char seguir;
    int partida = 0;
    int dia = 0;
    int mes = 0;
    int anio = 0;
    int turno = 0;
    int operacion = 1;
    //la id seria autoincremental
    do
    {
        system("clear");
        llamarOperacion(cliente, operacion);
        printf("     Alta servicio\n");
        printf("------------------------\n");
        printf("-Ingrese origen:\n[1]Mar del Plata\n[2]Buenos Aires\n");
        scanf("%d", &partida);
        enviarNumero(&cliente, partida);
        printf("-Ingrese Turno:\n[1]Turno Mañana\n[2]Turno Tarde\n[3]Turno Noche\n");
        scanf("%d", &turno);
        enviarNumero(&cliente, turno);
        printf("-Ingrese Fecha:\n");
        printf("Dia: ");
        scanf("%d", &dia);
        enviarNumero(&cliente, dia);
        printf("Mes: ");
        scanf("%d", &mes);
        enviarNumero(&cliente, mes);
        printf("Año: ");
        scanf("%d", &anio);
        enviarNumero(&cliente, anio);
        printf("\n------------------------\n");
        printf("Fecha: %d/%d/%d", dia, mes, anio);
        if (partida == MarDelPlata)
        {
            printf(" - Origen: Mar del Plata");
        }
        else
        {
            printf(" - Origen: Buenos Aires");
        }
        if (turno == TM)
        {
            printf(" - turno mañana");
        }
        else if (turno == TT)
        {
            printf(" - turno tarde");
        }
        else
        {
            printf(" - turno noche");
        }
        printf("\n------------------------\n");
        recibirEImprimirTexto(&cliente);
        printf(" ¿Desea generar otro? S/N\n");
        scanf(" %c", &seguir); //un maldito espacio faltaba adelante QUE DESGRACIA
        getchar();             //con getchar() rescatas el enter (30 hora como un salame y era esto
    } while (seguir == 'S' || seguir == 's');
}

void gestionarPasajes(int cliente, char nombre[1024])
{
    char seguir;
    int opcion = 0;
    int partida = 0;
    int dia = 0;
    int mes = 0;
    int anio = 0;
    int turno = 0;
    do
    {
        system("clear");
        printf("     Gestionar Pasajes\n");
        printf("------------------------\n");
        printf("Ingrese la opcion del criterio a consultar\n");
        printf("[1]Consultar por origen\n");
        printf("[2]Consultar por fecha\n");
        printf("[3]Consultar por turno\n");
        printf("[4]Consultar por fecha y turno\n");
        printf("[5]Consultar por fecha y origen\n");
        printf("[6]Consultar por turno y origen\n");
        printf("[7]Consultar por fecha, turno y origen\n");
        printf("[0]Volver al menu principal\n");
        scanf("%d", &opcion);
        system("clear");
        switch (opcion)
        {
        case 1:
            printf("Consultar por origen\n");
            printf("------------------------\n");
            printf("-Ingrese origen:\n[1]Mar del Plata\n[2]Buenos Aires\n");
            scanf("%d", &partida);
            mostrarListadoServicios(cliente, partida, 0, 0, 0, 0, nombre);
            break;
        case 2:
            printf("Consultar por fecha\n");
            printf("------------------------\n");
            printf("-Ingrese Fecha:\n");
            printf("Dia: ");
            scanf("%d", &dia);
            printf("Mes: ");
            scanf("%d", &mes);
            printf("Año: ");
            scanf("%d", &anio);
            mostrarListadoServicios(cliente, 0, dia, mes, anio, 0, nombre);
            break;
        case 3:
            printf("Consultar por turno\n");
            printf("------------------------\n");
            printf("-Ingrese Turno:\n[1]Turno Mañana\n[2]Turno Tarde\n");
            scanf("%d", &turno);
            mostrarListadoServicios(cliente, 0, 0, 0, 0, turno, nombre);
            break;
        case 4:
            printf("Consultar por fecha y turno\n");
            printf("------------------------\n");
            printf("-Ingrese Fecha:\n");
            printf("Dia: ");
            scanf("%d", &dia);
            printf("Mes: ");
            scanf("%d", &mes);
            printf("Año: ");
            scanf("%d", &anio);
            printf("-Ingrese Turno:\n[1]Turno Mañana\n[2]Turno Tarde\n");
            scanf("%d", &turno);
            mostrarListadoServicios(cliente, 0, dia, mes, anio, turno, nombre);
            break;
        case 5:
            printf("Consultar por fecha y origen\n");
            printf("------------------------\n");
            printf("-Ingrese Fecha:\n");
            printf("Dia: ");
            scanf("%d", &dia);
            printf("Mes: ");
            scanf("%d", &mes);
            printf("Año: ");
            scanf("%d", &anio);
            printf("-Ingrese origen:\n[1]Mar del Plata\n[2]Buenos Aires\n");
            scanf("%d", &partida);
            mostrarListadoServicios(cliente, partida, dia, mes, anio, 0, nombre);
            break;
        case 6:
            printf("Consultar por turno y origen\n");
            printf("------------------------\n");
            printf("-Ingrese Turno:\n[1]Turno Mañana\n[2]Turno Tarde\n");
            scanf("%d", &turno);
            printf("-Ingrese origen:\n[1]Mar del Plata\n[2]Buenos Aires\n");
            scanf("%d", &partida);
            mostrarListadoServicios(cliente, partida, 0, 0, 0, turno, nombre);
            break;
        case 7:
            printf("Consultar por fecha, turno y origen\n");
            printf("------------------------\n");
            printf("-Ingrese Fecha:\n");
            printf("Dia: ");
            scanf("%d", &dia);
            printf("Mes: ");
            scanf("%d", &mes);
            printf("Año: ");
            scanf("%d", &anio);
            printf("-Ingrese Turno:\n");
            scanf("%d", &turno);
            printf("-Ingrese origen:\n[1]Mar del Plata\n[2]Buenos Aires\n");
            scanf("%d", &partida);
            mostrarListadoServicios(cliente, partida, dia, mes, anio, turno, nombre);
            break;
        case 0:
            menu(cliente, nombre); //aca
            break;
        default:
            printf("Elige una opcion correcta\n");
        }
    } while (opcion >= 7); //aca
}

void mostrarListadoServicios(int cliente, int partida, int dia, int mes, int anio, int turno, char nombre[1024])
{
    int opcion = 0;
    int idServicio = 0;
    do
    {
        system("clear");
        printf("\nListado de Servicios: \n");
        printf("-----------------------\n");
        llamarOperacion(cliente, 2);
        enviarNumero(&cliente, partida);
        enviarNumero(&cliente, dia);
        enviarNumero(&cliente, mes);
        enviarNumero(&cliente, anio);
        enviarNumero(&cliente, turno);
        //Listado de servicios
        //recibo el total de registros
        int totalServicios = recibirNumero(&cliente);
        if (totalServicios == 0)
        {
            printf("\nNingún archivo cumple con ese criterio \n");
            printf("\nElige una opción:\n");
        }
        else
        {
            for (int i = 0; i < totalServicios; i++)
            {
                recibirEImprimirServicio(&cliente);
            }
            printf("\nElige una opción:\n");
            printf("[1]Elegir servicio\n");
        }
        printf("[2]Alta nuevo servicio\n");
        printf("[0]Volver al menu anterior\n");
        scanf("%d", &opcion);
        switch (opcion)
        {
        case 1:
            llamarOperacion(cliente, 4); //aca
            printf("Ingrese el ID del servicio elegido:");
            scanf("%d", &idServicio);
            system("clear");
            mostrarServicio(cliente, idServicio, nombre);
            break;
        case 2:
            altaServicioCliente(cliente);
            break;
        case 0:
            gestionarPasajes(cliente, nombre);
            break;
        default:
            system("clear");
            printf("Elige una opcion correcta\n");
        }
    } while (opcion != 0 || opcion != 1 || opcion != 2);
}

void mostrarServicio(int cliente, int idServicio, char nombre[1024])
{
    int opcion = 0;
    char fila = ' ';
    int columna = 0;
    int operacion = 1;
    printf("Servicio Nº: %d\n", idServicio);
    printf("\n------------------------\n");
    printf("Omnibus:\n");
    printf("O = Asiento libre\n");
    printf("X = Asiento ocupado\n");
    /*------------------------------
                CASO DE USO 3
        ------------------------------*/
    enviarNumero(&cliente, idServicio);
    do
    {
        imprimirBus(cliente);
        /*------------------------------
                IMPRIMIR MICRO
        ------------------------------*/
        printf("\n------------------------\n");
        printf("Elige una opción:\n");
        printf("[1]Reservar un asiento\n");
        printf("[2]Liberar un asiento\n");
        printf("[3]Elegir otro servicio\n"); //siento que es lo mismo que el menu anterior
        printf("[4]Volver al menú anterior\n");
        scanf(" %d", &opcion);
        switch (opcion)
        {
        case 1:
            printf("Ingrese una fila (A|B|C): ");
            scanf(" %c", &fila);
            getchar();
            printf("Ingrese el numero de asiento: ");
            scanf("%d", &columna);
            gestionarAsiento(cliente, idServicio, fila, columna, 1);
            enviarNumero(&cliente, idServicio);
            break;
        case 2:
            printf("Ingrese una fila (A|B|C): ");
            scanf(" %c", &fila);
            getchar();
            printf("Ingrese el numero de asiento: ");
            scanf("%d", &columna);
            gestionarAsiento(cliente, idServicio, fila, columna, 0);
            enviarNumero(&cliente, idServicio);
            break;
        case 3:
        case 4:
            getchar();
            enviarNumero(&cliente, 0);
            gestionarPasajes(cliente, nombre);
            break;
        default:
            system("clear");
            printf("Elige una opcion correcta\n");
        }
    } while (opcion == 1 || opcion == 2); //aca
}

int getFila(char fila)
{
    switch (fila)
    {
    case 'A':
    case 'a':
        return 1;
        break;
    case 'B':
    case 'b':
        return 2;
        break;
    case 'C':
    case 'c':
        return 3;
        break;
    }
}

void gestionarAsiento(int cliente, int idServicio, char fila, int columna, int operacion)
{
    system("clear");
    //llamarOperacion(&cliente, 1);
    //envio de parametros
    int fila_int=getFila(fila);
    enviarNumero(&cliente, idServicio);
    enviarNumero(&cliente, fila_int);
    enviarNumero(&cliente, columna);
    enviarNumero(&cliente, operacion);
    //respuesta del servidor
    int resp = recibirNumero(&cliente);
    char fila_char = ' ';
    if (resp == 0)
    {
        if (operacion == 0)
        {
            printf("Sevicio Nº %d, Asiento %c|%d liberado\n", idServicio, fila, columna);
        }
        else
        {
            printf("Sevicio Nº %d, Asiento %c|%d Reservado\n", idServicio, fila, columna);
        }
    }
    else
    {
        printf("* No se puede realizar la operacion solicitada en el Sevicio Nº %d, Asiento %c|%d , intente con otro asiento *\n", idServicio, fila, columna);
    }
    //mostrarServicio(&cliente,idServicio);
    printf("\n------------------------\n");
}

void mostrarRegistroDeactividades(int cliente, char *usuario)
{
    int opcion = 0;
    do
    {
        printf("\nRegistro de actividades de %s\n", usuario);
        llamarOperacion(cliente, 3);
        solicitarLogCliente(cliente, usuario);
        printf("------------------------\n");
        printf("[0]Volver al menu principal\n");
        scanf("%d", &opcion);
        if (opcion != 0)
        {
            system("clear");
            printf("Elige una opcion correcta\n");
        }

    } while (opcion != 0);
}
//MENU PRINCIPAL
void menu(int cliente, char nombre[1024])
{
    int opcion = 0;
    char *usuario;
    do
    {
        system("clear");
        printf("     Menu Principal\n");
        printf("------------------------\n");
        printf("Selecciona una opción:\n");
        printf("[1]Alta servicio\n");
        printf("[2]Gestionar pasajes\n");
        printf("[3]Ver registro de actividades\n");
        printf("[0]Cerrar sesión\n");
        scanf("%d", &opcion);
        switch (opcion)
        {
        case 0:
            system("clear");
            llamarOperacion(cliente, 0);
            //recibirEImprimirTexto(&cliente); //ACA!!
            printf("Sesion cerrada\n");
            exit(0);
            break;
        case 1:
            altaServicioCliente(cliente);
            system("clear");
            break;
        case 2:
            gestionarPasajes(cliente, nombre);
            break;
        case 3:
            system("clear");
            mostrarRegistroDeactividades(cliente, nombre);
            system("clear");
            break;
        default:
            system("clear");
            printf("Elige una opcion correcta\n");
        }

    } while (opcion != 0); //aca
}

void solicitarLogCliente(int cliente, char *nombre)
{
    //enviar solicitud
    enviarTexto(&cliente, nombre);
    //recibir del servidor cantidad de renglones
    int renglones = recibirNumero(&cliente);
    //abrir archivo donde guardar las respuestas
    FILE *f;
    char path[50] = "usuario";
    strcat(path, nombre);
    strcat(path, ".log");
    f = fopen(path, "a");

    //recibir todos los renglones y agregarlos al archivo
    int i;
    char buffer[256];
    for (i = 0; i < renglones; i++)
    {
        bzero(buffer, 256);
        recibirTexto(&cliente, buffer);
        fprintf(f, "%s", buffer);
        printf("%s", buffer);
    }
    fclose(f);
}

void imprimirBus(int socket)
{
    int array[3][20];
    int col1[20];
    recibirAsientos(&socket, col1);

    int col2[20];
    recibirAsientos(&socket, col2);

    int col3[20];
    recibirAsientos(&socket, col3);

    printf("%*c", 21, ' ');
    printf("1 1 1 1 1 1 1 1 1 1 2\n");
    printf("%*c", 3, ' ');
    printf("1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 ");
    printf("\n------------------------------------------\n");
    printf("A| ");
    for (int i = 0; i < FILAS; i++)
    {
        if (col1[i] == 0)
        {
            printf("O ");
        }
        else
        {
            printf("X ");
        }
    }
    printf("\nB| ");
    for (int i = 0; i < FILAS; i++)
    {
        if (col2[i] == 0)
        {
            printf("O ");
        }
        else
        {
            printf("X ");
        }
    }
    printf("\n==========================================");
    printf("\nC| ");
    for (int i = 0; i < FILAS; i++)
    {
        if (col3[i] == 0)
        {
            printf("O ");
        }
        else
        {
            printf("X ");
        }
    }
}