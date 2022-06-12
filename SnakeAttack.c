#include <stdbool.h>
#include <windows.h>

#include "menu.h"
#include "juego.h"
#include "functAux.h"

int main(){

    int numeroJugadores;
    int nivelJuego;
    char nombreP1[15];
    char nombreP2[15];
    alertaMaxPantalla();
    while(true){
        if(menu(&numeroJugadores, &nivelJuego, nombreP1, nombreP2)) break;
        juego(numeroJugadores, nivelJuego, nombreP1, nombreP2);
    }
    

    return 0;
}




