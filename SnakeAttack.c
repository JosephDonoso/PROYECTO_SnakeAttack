#include <stdbool.h>

#include "list.h"
#include "menu.h"
#include "juego.h"

int main(){

    int numeroJugadores;
    int nivelJuego;
    char nombreP1[15];
    char nombreP2[15];
    while(true){
        if(menu(&numeroJugadores, &nivelJuego, nombreP1, nombreP2)) break;
        numeroJugadores = 2;
        nivelJuego = 4;
        juego(numeroJugadores, nivelJuego, nombreP1, nombreP2);
    }
    

    return 0;
}




