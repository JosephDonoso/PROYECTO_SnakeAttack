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
    alertaMaxPantalla();
    while(true){
        Propiedades* prop = menu();
        if(prop->flag) break; //Del archivo menu.h
        juego(prop->numJugadores, prop->nivelJuego, prop->nombreJugador);        //Del archivo juego.h
    }

    return 0;
}




