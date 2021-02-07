#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "servicios.h"

int guardarServicio(int fecha, int turno, int partida)
{
    /*
    si buscarServicio devuelve 1 -> cucha // pendiente
    si buscarServicio devuelve 0 -> s = {} -> guardar en servicios.bin
    */

    FILE *fp;
    int existe = existeServicio(fecha, turno, partida);
    if (existe == FALSO)
    {
        fp = fopen("servicios.bin", "ab+");
        Servicio s = completarServicio(fecha, turno, partida);
        fwrite(&s, sizeof(Servicio), 1, fp);
        fclose(fp);
        return VERDADERO;
    }
    else
    {
        return FALSO;
    }
}

int existeServicio(int fecha, int turno, int partida)
{
    int existe = FALSO;
    Servicio s;
    FILE *fp;
    fp = fopen(archivoServicios, "r+b");
    if (fp) //si el archivo ya existe
    {

        while (fread(&s, sizeof(Servicio), 1, fp) == 1 && existe == FALSO)
        {
            if (s.fecha == fecha && s.partida == partida && s.turno == turno)
            {
                existe = VERDADERO;
            }
        }
        fclose(fp);
    }
    return existe;
}

Servicio completarServicio(int fecha, int turno, int partida)
{
    int asientosVacios[COLUMNAS][FILAS];
    FILE *fp;
    int id = traerUltimoID(fp);
    Servicio servicio = {id + 1, fecha, turno, partida, asientosVacios};
    for (int i = 0; i < COLUMNAS; i++)
    {
        for (int j = 0; j < FILAS; j++)
        {
            servicio.asientos[i][j] = 0;
        }
    }

    return servicio;
}

int traerUltimoID()
{
    int id = 0;
    Servicio s;
    FILE *fp;
    fp = fopen(archivoServicios, "r+b");

    if (fp)
    {
        {
            while (fread(&s, sizeof(Servicio), 1, fp) == 1)
            {
                id = s.id;
            }
        }
    }
    return id;
}

int traerServicio(int id, Servicio *servicio)
{
    /*
    abre el archivo
    recorre todos los structs
    devuelve el servicio
    */
    FILE *fp;
    Servicio s;
    fp = fopen("servicios.bin", "r+b");
    if (!fp)
        return 0;

    while (fread(&s, sizeof(Servicio), 1, fp) == 1)
    {
        if (s.id == id)
        {
            *servicio = s;
        }
    }
    fclose(fp);
    return (0);
}

Servicio *buscarServicios(int fecha, int turno, int partida, int *total)
{
    /*
    abre el archivo
    recorre todos los structs
    si no hay match, devuelve 0
    si hay match, devuelve 1
    */

    /*
    if fecha==0
        if turno==0 -> partida
        else turno + partida
    else if turno==0
        if partida == 0 -> fecha
        else fecha + partida
    else if partida==0
        if fecha == 0 -> turno
        else fecha + turno
    else partida + turno + fecha
        */

    FILE *fp;
    int totalDeServicios = traerUltimoID();
    int totalDeGuardados = 0;

    Servicio s;
    Servicio lista[totalDeServicios]; //-1
    fp = fopen(archivoServicios, "r+b");
    if (fp)
    {
        while (fread(&s, sizeof(Servicio), 1, fp) == 1)
        {

            if (fecha == 0)
            {
                if (turno == 0)
                {
                    if (partida == s.partida)
                    {
                        lista[totalDeGuardados] = s;
                        totalDeGuardados++;
                    }
                }
                else
                {
                    if (turno == s.turno && partida == s.partida)
                    {
                        lista[totalDeGuardados] = s;
                        totalDeGuardados++;
                    }
                }
            }
            else if (turno == 0)
            {
                if (partida == 0)
                {
                    if (fecha == s.fecha)
                    {
                        lista[totalDeGuardados] = s;
                        totalDeGuardados++;
                    }
                }
                else
                {
                    if (fecha == s.fecha && partida == s.partida)
                    {
                        lista[totalDeGuardados] = s;
                        totalDeGuardados++;
                    }
                }
            }
            else if (partida == 0)
            {
                if (fecha == 0)
                {
                    if (turno == s.turno)
                    {
                        lista[totalDeGuardados] = s;
                        totalDeGuardados++;
                    }
                }
                else
                {
                    if (fecha == s.fecha && turno == s.turno)
                    {
                        lista[totalDeGuardados] = s;
                        totalDeGuardados++;
                    }
                }
            }
            else
            {
                if (fecha == s.fecha && turno == s.turno && partida == s.partida)
                {
                    lista[totalDeGuardados] = s;
                    totalDeGuardados++;
                }
            }
        }
        fclose(fp);
    }
    else
    {
        totalDeGuardados = 0;
    }
    Servicio *arrayServicios = (Servicio *)calloc(totalDeGuardados + 1, sizeof(Servicio));
    for (int i = 0; i < totalDeGuardados; i++)
    {
        arrayServicios[i] = lista[i];
    }
    *total = totalDeGuardados;
    return arrayServicios;
}

// Servicio *traerTodosServicios(int *total)
// {
//     FILE *fp;
//     Servicio arrayServicios[100];
//     Servicio s;
//     fp = fopen(archivoServicios, "r+b");
//     int i = 0;
//     while (fread(&s, sizeof(Servicio), 1, fp) == 1)
//     {
//         arrayServicios[i] = s;
//         i++;
//     }
//     fclose(fp);

//     Servicio *lista = (Servicio *)calloc(i + 1, sizeof(Servicio));
//     lista = arrayServicios;
//     *total = i;
//     return lista;
// }

int hacerReserva(int id, int fila, int numero, int reserva)
{
    //DEVUELVE 0 SI SE PUDO HACER EL CAMBIO
    /*
    ya sabemos que el servicio existe
    abrimos el archivo
    traemos el servicio por ID
    modificamos el asiento
    */

    FILE *fp;
    Servicio s;
    fp = fopen(archivoServicios, "r+b");

    while (fread(&s, sizeof(Servicio), 1, fp) == 1)
    {
        if (s.id == id)
        {
            //reserva tiene 1 si quiere reservar, 0 si quiere liberar
            if (s.asientos[fila - 1][numero - 1] == reserva)
            {
                fclose(fp);
                return 1;
            }
            else
            {
                s.asientos[fila - 1][numero - 1] = reserva;
                fseek(fp, ftell(fp) - sizeof(Servicio), SEEK_SET);
                fwrite(&s, sizeof(Servicio), 1, fp);
                fclose(fp);
                return 0;
            }
        }
    }
}

// void mostrarTodosLosServicios()
// {
//     FILE *fp;
//     Servicio s;
//     fp = fopen(archivoServicios, "r+b");

//     while (fread(&s, sizeof(Servicio), 1, fp) == 1)
//     {
//         printf("ID: %d \n", s.id);
//         printf("fecha: %d \n", s.fecha);
//         printf("partida: %d \n", s.partida);
//         printf("turno: %d \n", s.turno);
//         for (int i = 0; i < COLUMNAS; i++)
//         {
//             for (int j = 0; j < FILAS; j++)
//             {
//                 printf("%d", s.asientos[i][j]);
//             }
//             printf("\n");
//         }
//         printf("\n\n");
//     }
// }


/*          TO STRING        */
void servicioToString(char * str, int dia, int mes, int anio, int turno, int partida){
    
}
