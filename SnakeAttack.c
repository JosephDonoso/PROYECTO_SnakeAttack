#include <stdbool.h>

#include "list.h"
#include "menu.h"
#include "juego.h"

int main(){

    int numeroJugadores;
    int modoJuego;
    char nombreP1[15];
    char nombreP2[15];
    while(true){
        if(menu(&numeroJugadores, &modoJuego, nombreP1, nombreP2)) break;
        numeroJugadores = 1;
        juego(numeroJugadores, modoJuego, nombreP1, nombreP2);
    }
    

    return 0;
}




