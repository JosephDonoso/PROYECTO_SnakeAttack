#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>


#include "menu.h"
#include "juego.h"
#include "functAux.h"

int main(){

    int* numeroJugadores = (int*) malloc(sizeof(int));
    int* nivelJuego = (int*) malloc(sizeof(int));
    char nombreJugador[15];
    alertaMaxPantalla();
    while(true){
        if(menu(numeroJugadores, nivelJuego, nombreJugador)) break;
        juego(*numeroJugadores, *nivelJuego, nombreJugador);
    }
    

    return 0;
}




