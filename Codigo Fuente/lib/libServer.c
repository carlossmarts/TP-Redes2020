#include "libServer.h"

/* --------------------------------------------------------------------
                                FUNCIONES
 --------------------------------------------------------------------*/
//-------------------
// Manejo de errores
//-------------------
void error(const char *msg)
{
    printf("%s\n", msg);
    exit(1);
}

//--------------------------
//Inicialización del server
//--------------------------
void iniciarServerSocket(int *servidor, int puerto)
{
    struct sockaddr_in serv_addr;
    //Crear el socket del servidor
    (*servidor) = socket(AF_INET, SOCK_STREAM, 0);
    if ((*servidor) < 0)
        error("Error al crear el socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    //inicializar variables
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(puerto);
    //timeout para recepción de mensajes
    struct timeval time_out;
    time_out.tv_sec = 120; //TIENE QUE SER 120!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    time_out.tv_usec = 0;
    setsockopt(*servidor, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_out, sizeof(time_out));

    //Asignación de dirección al socket
    int n = bind((*servidor), (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0)
        error("Error al asignar puerto");
    registrarInicioServidor(puerto);
}

//--------------------------
//aceptar cliente
//--------------------------
void aceptarCliente(int *cliente, int *servidor)
{
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);
    *cliente = accept(*servidor, (struct sockaddr *)&cli_addr, &clilen);
    if (*cliente < 0)
        error("Error al recibir un cliente");
}

/*-------
Enviar
--------*/
void enviarTexto(int *cliente, char *texto)
{
    int n = send(*cliente, texto, 1024, 0);
    if (n < 0)
        error("error al escribir mensaje");
}

void enviarNumero(int *cliente, int num)
{
    int n = send(*cliente, &num, 4, 0);
    if (n < 0)
        error("error al escribir mensaje");
}

/*-------
recibir
--------*/
void recibirTexto(int *cliente, char *texto, int *err)
{
    char m[1024];
    int n = recv(*cliente, m, 1024, 0);
    if (n < 0)
    {
        printf("tiempo de espera superado");
        *err = TIMEOUTVAL;
    }
    strcpy(texto, m);
}

int recibirNumero(int *cliente, int *err)
{
    int valor;
    int n = recv(*cliente, &valor, 4, 0);
    if (n < 0)
    {
        printf("tiempo de espera superado");
        *err = TIMEOUTVAL;
    }
    return valor;
}

/*--------------------------------
varificar usuario
devuelve 1 si encontró el usuario
---------------------------------*/

int verificarNombre(const char *nombre, struct usuario *usuarios, int cant)
{
    int encontrado = 0;
    int j = 0;
    while (j < cant && encontrado == 0)
    {
        if (strcmp(usuarios[j].nombre, nombre) == 0)
        {
            encontrado = 1;
        }
        j++;
    }
    return encontrado;
}

int verificarUsuario(const char *user, const char *pass, struct usuario *usuarios, int cant)
{
    int encontrado = 0;
    int j = 0;
    while (j < cant && encontrado == 0)
    {
        if (strcmp(usuarios[j].nombre, user) == 0 && strcmp(usuarios[j].contr, pass) == 0)
        {
            encontrado = 1;
        }
        j++;
    }
    return encontrado;
}

/*--------------------------------
login
devuelve 1 si se logueo
---------------------------------*/
int login(int cliente, struct usuario *usuarios, int cant, char nombre[1024], int *err)
{
    int encontrado = 0;
    enviarTexto(&cliente, "USUARIO: ");
    //char nombre[1024];
    recibirTexto(&cliente, nombre, err); //nombre
    if (*err == TIMEOUTVAL)
        goto F;

    printf("\nNombre recibido: %s", nombre);
    int existeNombre = 0;
    existeNombre = verificarNombre(nombre, usuarios, cant);
    enviarNumero(&cliente, existeNombre);
    if (existeNombre == 0)
    { //si no encontró el nombre
        enviarTexto(&cliente, "Nombre de usuario incorrecto");
    }
    else
    {
        enviarTexto(&cliente, "CONTRASEÑA: ");
        char contr[1024];
        recibirTexto(&cliente, contr, err); //contraseña
        if (*err == TIMEOUTVAL)
            goto F;
        encontrado = verificarUsuario(nombre, contr, usuarios, cant);
    }

F:;
    return encontrado;
}

void cargarUsuarios(struct usuario *usuarios)
{
    FILE *f;
    f = fopen("../txt/usuarios.txt", "r");
    if (f == NULL)
    {
        printf("\nError al abrir el archivo\n");
        exit(1);
    }
    int i;
    char temp[24];
    int n = 0;
    while (!feof(f))
    {
        char nombre[12];
        char contr[12];
        fscanf(f, " %[^;];%[^\n]", nombre, contr);
        strcpy(usuarios[n].nombre, nombre);
        strcpy(usuarios[n].contr, contr);
        n++;
    }
}

int altaServicio(int cliente, char *usuario, int *err)
{

    int partida = 0, dia = 0, mes = 0, anio = 0, turno = 0;

    //origen
    partida = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    turno = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    dia = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    mes = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    anio = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;

    int fecha = anio * 10000 + mes * 100 + dia;
    int creado = guardarServicio(fecha, turno, partida);
    char mensaje[1024] = "";
    if (creado)
    {
        enviarTexto(&cliente, "Servicio creado existosamente");
        printf("Servicio creado existosamente\n");
        int idServicio = traerUltimoID();
        registrarAltaServicio(usuario, idServicio);
        serverLog("Servicio creado existosamente\n");
        userLog(usuario, "Servicio creado existosamente\n");
    }
    else
    {
        enviarTexto(&cliente, "Error, el servicio ya existe");
        printf("Error, el servicio ya existe\n");
    }
F:;
}

/*................................
Llamar a operaciones del servidor
................................*/
int recibirOperacion(int cliente, int *err)
{
    return recibirNumero(&cliente, err);
}

int cantidadUsuarios()
{
    int cont = 0;
    FILE *f;
    char temp[24];
    f = fopen("../txt/usuarios.txt", "r");
    if (f == NULL)
    {
        printf("Error al abrir el archivo");
        exit(1);
    }
    while (!feof(f))
    {
        fgets(temp, 24, f);
        cont++;
    }
    return cont;
}

void enviarLogCliente(int cliente, char *usuario, int *err)
{
    char nombre[12];
    recibirTexto(&cliente, nombre, err);
    if (*err == TIMEOUTVAL)
        goto F;
    char path[50] = "../txt/usuario";
    strcat(path, nombre);
    strcat(path, ".log");
    FILE *f;
    f = fopen(path, "r");
    if (f == NULL)
    {
        printf("Error al abrir el archivo");
        exit(1);
    }
    char temp[256];
    //contar cantidad de renglones a enviar
    int cont = 0;
    while (!feof(f))
    {
        fgets(temp, 256, f);
        cont++;
    }
    //informar cantidad al cliente
    enviarNumero(&cliente, cont-1);
    //volver al principio del archivo
    rewind(f);
    //enviar al cliente renglon por renglon
    int i;
    for (i = 0; i < cont; i++)
    {
        fgets(temp, 256, f);
        enviarTexto(&cliente, temp);
    }
    fclose(f);
    char svLog[200] = "Usuario ";
    strcat(svLog, usuario);
    strcat(svLog, " consultó su log de usuario \n");
    serverLog(svLog);
    userLog(usuario, "Consulta a log de usuario \n");
F:;
}

void enviarArrayServicio(int *cliente, int *num)
{
    //serv_0 -> idServicio
    //serv_1 -> origen
    //serv_2 -> Fecha
    //serv_3 -> turno

    int n = write(*cliente, num, 16);
    if (n < 0)
    {
        error("error al escribir mensaje");
    }
}

void mostrarListadoServicios(int cliente, char *usuario, int *err)
{
    //recibo los parametros
    printf("Buscando servicios Filtrados \n");

    int partida = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    int dia = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    int mes = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    int anio = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    int turno = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    int fecha = anio * 10000 + mes * 100 + dia;
    int total = 0;
    Servicio *arrayServicios = buscarServicios(fecha, turno, partida, &total);
    //envio el total de registros:
    enviarNumero(&cliente, total);
    //envio los servicios a mostrar
    for (int i = 0; i < total; i++)
    {
        int servicio[4];
        servicio[0] = arrayServicios[i].id;
        servicio[1] = arrayServicios[i].partida;
        servicio[2] = arrayServicios[i].fecha;
        servicio[3] = arrayServicios[i].turno;
        enviarArrayServicio(&cliente, servicio);
    }
    char svLog[200] = "Usuario ";
    strcat(svLog, usuario);
    strcat(svLog, " realizó una consulta al listado de servicios\n");
    serverLog(svLog);
    userLog(usuario, svLog);

F:;
    printf("\n");
}

void enviarAsientos(int cliente, int id)
{
    //recibo los parametros
    Servicio s;
    traerServicio(id, &s);
    int i, j;
    int col[20];
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 20; j++)
        {
            col[j] = s.asientos[i][j];
        }
        enviarArrayAsientos(&cliente, col);
    }
}

void mostrarAsientos(int cliente, char *usuario, int *err)
{
    printf("Recibiendo ID de servicio para filtro\n");
    int idServicio = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    printf("Servicio a buscar: %d - ", idServicio);
    enviarAsientos(cliente, idServicio);
    gestionarAsiento(cliente, usuario, err);
// escuchamos numero num
// if num == 1 o ==2
// modificar reserva
// if ==3 gestionarReserva
// if ==4 mostrarListadoServicios
F:;
}

void enviarArrayAsientos(int *cliente, int *num)

{
    int n = write(*cliente, num, 80);
    if (n < 0)
    {
        error("error al escribir mensaje");
    }
}

void gestionarAsiento(int cliente, char *usuario, int *err)
{
    //recibo parametros
    int idServicio = recibirNumero(&cliente, err);
    if (*err == TIMEOUTVAL)
        goto F;
    if (idServicio > 0)
    {
        int fila = recibirNumero(&cliente, err);
        if (*err == TIMEOUTVAL)
            goto F;
        int columna = recibirNumero(&cliente, err);
        if (*err == TIMEOUTVAL)
            goto F;
        int operacion = recibirNumero(&cliente, err);
        if (*err == TIMEOUTVAL)
            goto F;
        int resp = hacerReserva(idServicio, fila, columna, operacion);
        //registrar en log si se pudo gestionar el asiento
        if (resp == 0)
        {
             registrarGestionDeAsiento(usuario, idServicio, fila, columna, operacion);
        }
        //respuesta del servidor
        enviarNumero(&cliente, resp);
        mostrarAsientos(cliente, usuario, err);
    }
F:;
}
int modificarReserva(int cliente, int id, int fila, int numero, int modificacion)
{
    int guardado = hacerReserva(id, fila, numero, modificacion);
    return guardado;
}

//----------------------
//anotaciones en log
//----------------------
//esta falta agregarle la escritura en el archivo de texto
void nota(const char *path, const char *msg)
{
    FILE *archivo;
    char texto[1024] = "";
    strcat(texto, msg);
    archivo = fopen(path, "a");
    fprintf(archivo, "%s", texto);
    fclose(archivo);
    // printf("nota en log: %s", texto);
}

void obtenerFecha(char *fecha)
{
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    strftime(fecha, 100, "%d/%m/%Y", tm);
}

void obtenerHora(char *hora)
{
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    strftime(hora, 100, "%H:%M:%S", tm);
}

void serverLog(char *msg)
{
    char path[20] = "../txt/server.log";
    char texto[1024] = "";
    char fecha[20];
    obtenerFecha(fecha);
    char hora[20];
    obtenerHora(hora);
    strcat(texto, fecha);
    strcat(texto, " - ");
    strcat(texto, hora);
    strcat(texto, " : ");
    strcat(texto, msg);
    strcat(texto, "\n");
    nota(path, texto);
}

void userLog(char *nombre, char *msg)
{
    char path[20] = "../txt/usuario";
    strcat(path, nombre);
    strcat(path, ".log");

    char texto[1024] = "";
    char fecha[20];
    obtenerFecha(fecha);
    char hora[20];
    obtenerHora(hora);
    strcat(texto, fecha);
    strcat(texto, " - ");
    strcat(texto, hora);
    strcat(texto, " : ");
    strcat(texto, msg);
    strcat(texto, "\n");
    nota(path, texto);
}

void registrarInicioServidor(int puerto)
{
    char serverIniciado[1024] = "";
    strcat(serverIniciado, "Inicia Servidor\n");
    char msg[1024] = "";
    strcat(msg, "Socket creado. Puerto de escucha: ");
    char noPuerto[1024] = "";
    sprintf(noPuerto, "%d", puerto);
    strcat(msg, noPuerto);
    strcat(msg, "\n");
    char guiones[1024] = "";
    strcat(guiones, "==============================\n");

    serverLog(guiones);
    serverLog(serverIniciado);
    serverLog(guiones);
    serverLog(msg);
}

void registrarLoginUsuario(char *nombre)
{
    char msg[1024] = "";
    strcat(msg, "Usuario ");
    strcat(msg, nombre);
    strcat(msg, " Logueado\n");
    char guiones[1024] = "";
    strcat(guiones, "==============================\n");
    userLog(nombre, guiones);
    userLog(nombre, msg);
    userLog(nombre, guiones);

    serverLog(guiones);
    serverLog(msg);
    serverLog(guiones);
}

void registrarLogoutUsuario(char *nombre)
{
    char msg[1024] = "";
    strcat(msg, "Usuario ");
    strcat(msg, nombre);
    strcat(msg, " Deslogueado\n");
    char guiones[1024] = "";
    strcat(guiones, "==============================\n");
    userLog(nombre, guiones);
    userLog(nombre, msg);
    userLog(nombre, guiones);

    serverLog(guiones);
    serverLog(msg);
    serverLog(guiones);
}

void registrarAltaServicio(char *usuario, int idServicio)
{
    char msg[1024] = "";
    //2020-09-20_12:34: IdServicio: 7 - Alta de Servicio
    strcat(msg, "IdServicio: ");
    char texto[100];
    sprintf(texto, "%d", idServicio);
    strcat(msg, texto);
    strcat(msg, " - Alta de Servicio\n");
    serverLog(msg);
    userLog(usuario, msg);
}
void registrarGestionDeAsiento(char *usuario, int idServicio, int fila, int columna, int operacion)
{
    char msg[1024] = "";
    //2020-09-20_12:34: IdServicio - Reserva_A1
    //2020-09-20_12:36: IdServicio - Libera_C10
    strcat(msg, "IdServicio: ");
    char texto[100];
    sprintf(texto, "%d", idServicio);
    strcat(msg, texto);
    if (operacion == 1)
    {
        strcat(msg, " - Reserva_");
    }
    else
    {
        strcat(msg, " - Libera_");
    }
    switch (fila)
    {
    case 1:
        strcat(msg, "A");
        break;
    case 2:
        strcat(msg, "B");
        break;
    case 3:
        strcat(msg, "C");
        break;
    }
    char coltxt[100];
    sprintf(coltxt, "%d", columna);
    strcat(msg, coltxt);
    strcat(msg, "\n");
    serverLog(msg);
    userLog(usuario, msg);
}
