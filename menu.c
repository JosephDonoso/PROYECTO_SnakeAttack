#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

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

bool cambiarOpcion(short * opcion){
    Sleep(250);
    fflush(stdin);
    getch();

    if( GetAsyncKeyState(VK_UP) ){
        *opcion -= 1;
    }
    else if( GetAsyncKeyState(VK_DOWN) ){
        *opcion += 1;
    }
    else if( GetAsyncKeyState(VK_RETURN) ){
        return false;
    }
    return true;
}

void ubicarFlecha(short opcion){

    gotoxy(55,31 + opcion);
    printf("-->");

}

void menuNivel(Propiedades* prop){
    short opcion = 0;
    while(true){

        while(true){
            system("cls");
            gotoxy(0, 0);
            dibujarLogo();
            gotoxy(0, 31);
            printf("                                                            Nivel 1\n");
            printf("                                                            Nivel 2\n");
            printf("                                                            Nivel 3\n");
            printf("                                                            Nivel 4\n");
            printf("                                                            Nivel 5\n");
            printf("                                                            VOLVER\n");
            
            opcion = opcion % 6;
            if(opcion < 0) opcion = 6 + opcion;

            ubicarFlecha(opcion);
            if(!cambiarOpcion(&opcion)) break;
        }

        switch (opcion)
        {
        case 0: //NIVEL 1 - SIGUIENTE
            prop->nivelJuego = 0;
            return;
        case 1: //NIVEL 2 - SIGUIENTE
            prop->nivelJuego = 1;
            return;
        case 2: //NIVEL 3 - SIGUIENTE
            prop->nivelJuego = 2;
            return;
        case 3: //NIVEL 4 - SIGUIENTE
            prop->nivelJuego = 3;
            return;
        case 4: //NIVEL 5 - SIGUIENTE
            prop->nivelJuego = 4;
            return;
        case 5: //VOLVER
            menuModo(prop);
            return;
        }
    }
}

void menuModo(Propiedades* prop){
    short opcion = 0;
    while(true){

        while(true){
            system("cls");
            gotoxy(0, 0);
            dibujarLogo();
            gotoxy(0, 31);
            printf("                                                            MODO CLASICO\n");
            printf("                                                            MODO INFINITO\n");
            printf("                                                            VOLVER\n");
            
            opcion = opcion % 3;
            if(opcion < 0) opcion = 3 + opcion;
            ubicarFlecha(opcion);
            if(!cambiarOpcion(&opcion)) break;
        }

        switch (opcion)
        {
        case 0: //MODO CLÁSICO - SIGUIENTE 
            prop->nivelJuego = -1;
            return;
        case 1: //MODO INFINITO - SIGUIENTE 
            menuNivel(prop);
            return;
        case 2: //VOLVER
            menuJugadores(prop);
            return;
        }  
    } 
}


void menuJugadores(Propiedades* prop){
    short opcion = 0;
    while(true){

        while(true){
            system("cls");
            gotoxy(0, 0);
            dibujarLogo();
            gotoxy(0, 31);
            printf("                                                            1 JUGADOR\n");
            printf("                                                            2 JUGADORES\n");
            printf("                                                            VOLVER\n");
            
            opcion = opcion % 3;
            if(opcion < 0) opcion = 3 + opcion;
            ubicarFlecha(opcion);
            if(!cambiarOpcion(&opcion)) break;
        }

        switch (opcion)
        {
        case 0: //1 JUGADOR - SIGUIENTE
            prop->numJugadores = 1;
            //recibirNombre(prop->nombreP1, 0);
            menuModo(prop);
            return;
        case 1: //2 JUGADORES - SIGUIENTE
            prop->numJugadores = 2;
            //recibirNombre(prop->nombreP1, 1);
            //recibirNombre(prop->nombreP1, 2);
            menuModo(prop);
            return;
        case 2: //VOLVER
            menuPrincipal(prop);
            return;
        }
    }
}

void menuPrincipal(Propiedades* prop){
    short opcion = 0;
    while(true){

        while(true){
            system("cls");
            gotoxy(0, 0);
            dibujarLogo();
            gotoxy(0, 31);
            printf("                                                            JUGAR\n");
            printf("                                                            HIGHSCORE\n");
            printf("                                                            COMO JUGAR\n");
            printf("                                                            SALIR\n");

            opcion = opcion % 4;
            if(opcion < 0) opcion = 4 + opcion;
            ubicarFlecha(opcion);
            if(!cambiarOpcion(&opcion)) break;
        }

        switch (opcion)
        {
        case 0: //SIGUIENTE
            menuJugadores(prop);
            return;
        case 1: //HIGHSCORE
            //mostrarHighscore();
            break;
        case 2: //COMO JUGAR
            //instrucciones();
            break;
        case 3: //SALIR
            prop->flag = true;
            return;
        } 
    }
}

bool menu(int* numJugadores , int* nivelJuego, char* nombreP1, char* nombreP2 ){
    Propiedades* prop = (Propiedades*) malloc(sizeof(Propiedades));
    //importarHighscore();
    prop->flag = false;
    system("cls");
    system("color 0E");
    menuPrincipal( prop );

    *numJugadores = prop->numJugadores;
    *nivelJuego = prop->nivelJuego;
    /*strcpy(nombreP1, prop->nombreP1); 
    strcpy(nombreP2, prop->nombreP2);*/

    GetAllKeys();

    return prop->flag;
}
