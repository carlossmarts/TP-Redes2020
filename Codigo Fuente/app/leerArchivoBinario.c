#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/servicios.h"

int main()
{
    FILE *fp;
    Servicio s;
    fp = fopen("servicios.bin", "r+b");

    if (fp)
    {
        while (fread(&s, sizeof(Servicio), 1, fp) == 1)
        {
            printf("ID: %d \n", s.id);
            printf("Fecha:");
            int dia = s.fecha % 100;
            int mes = (s.fecha % 10000) / 100;
            int anio = s.fecha / 10000;
            printf(" %d/%d/%d", dia, mes, anio);
            if (s.partida == MarDelPlata)
            {
                printf("\nOrigen: Mar del Plata");
            }
            else
            {
                printf("\nOrigen: Buenos Aires");
            }

            printf(" \n");

            if (s.partida == TM)
            {
                printf("Turno Mañana");
            }
            else if (s.partida == TT)
            {
                printf("Turno Tarde");
            }
            else
            {
                printf("Turno Noche");
            }
            printf(" \n");

            for (int i = 0; i < COLUMNAS; i++)
            {
                for (int j = 0; j < FILAS; j++)
                {
                    printf("%d", s.asientos[i][j]);
                }
                printf("\n");
            }
            printf("\n\n");
        }
    }
    else
    {
        printf("Vacío\n");
    }
    return 0;
}
