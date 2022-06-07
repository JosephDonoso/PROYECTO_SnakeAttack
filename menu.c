#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#include "list.h"
#include "menu.h"
#include "functAux.h"

struct Propiedades
{
    int numJugadores;
    int nivelJuego;
    char nombreP1[15];
    char nombreP2[15];
    bool flag;
};

void menuNivel(Propiedades* prop){
    short opcion = -1;
    switch (opcion)
    {
    case 1: //SIGUIENTE
        prop->nivelJuego = 1;
        return;
    case 2: //SIGUIENTE
        prop->nivelJuego = 2;
        return;
    case 3: //SIGUIENTE
        prop->nivelJuego = 3;
        return;
    case 4: //SIGUIENTE
        prop->nivelJuego = 4;
        return;
    case 5: //SIGUIENTE
        prop->nivelJuego = 5;
        return;
    case 6: //VOLVER
        menuModo(prop);
        return;
    default: //REINTENTAR
        printf("VUELVA A INTENTARLO");
        break;
    }
}

void menuModo(Propiedades* prop){
    short opcion = -1;
    switch (opcion)
    {
    case 1: //SIGUIENTE, MODO CLÁSICO
        prop->nivelJuego = -1;
        return;
    case 2: //SIGUIENTE, MODO INFINITO
        menuModoinfinito(prop);
        return;
    case 3: //VOLVER
        menuJugadores(prop);
        return;
    default: //REINTENTAR
        printf("VUELVA A INTENTARLO");
        break;
    }
}


void menuJugadores(Propiedades* prop){
    short opcion = -1;
    switch (opcion)
    {
    case 1: //SIGUIENTE
        prop->numJugadores = 1;
        menuModo(prop);
        return;
    case 2: //SIGUIENTE
        prop->numJugadores = 2;
        menuModo(prop);
        return;
    case 3: //VOLVER
        menuPrincipal(prop);
        return;
    default: //REINTENTAR
        printf("VUELVA A INTENTARLO");
        break;
    }
}

void menuPrincipal(Propiedades* prop){
    short opcion = -1;
    switch (opcion)
    {
    case 1: //SIGUIENTE
        menuJugadores(prop);
        return;
    case 2:
        mostrarHighscore();
        break;
    case 3:
        instrucciones();
        break;
    case 4: //SALIR
        prop->flag = false;
        return;
    default: //REINTENTAR
        printf("VUELVA A INTENTARLO");
        break;
    }
}

bool menu(int* numJugadores , int* nivelJuego, char* nombreP1, char* nombreP2 ){

    Propiedades* prop = (Propiedades*) malloc(sizeof(Propiedades));
    //importarHighscore();
    prop->flag = true;

    menuPrincipal( prop );

    * numJugadores = prop->numJugadores;
    * nivelJuego = prop->nivelJuego;
    strcpy(nombreP1, prop->nombreP1); 
    strcpy(nombreP2, prop->nombreP2);

    return prop->flag;
}




