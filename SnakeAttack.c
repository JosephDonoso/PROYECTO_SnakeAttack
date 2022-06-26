#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>


#include "menu.h"
#include "juego.h"
#include "functAux.h"

//Función principal donde serán llamadas a las demás funciones de los archios de juego y menú
int main(){
    //Se declararán variables que se utilizan en ambas funciones de distintos archivos
    int* numeroJugadores = (int*) malloc(sizeof(int));
    int* nivelJuego = (int*) malloc(sizeof(int));
    char nombreJugador[15];
    alertaMaxPantalla();
    while(true){
        if(menu(numeroJugadores, nivelJuego, nombreJugador)) break; //Del archivo menu.h
        juego(*numeroJugadores, *nivelJuego, nombreJugador);        //Del archivo juego.h
    }
    

    return 0;
}




