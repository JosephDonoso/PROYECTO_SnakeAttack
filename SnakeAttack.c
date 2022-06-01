#include <stdbool.h>

#include "list.h"
#include "menu.h"
#include "juego.h"

int main(){

    int numeroJugadores;
    int modoJuego;
    while(true){
        if(menu(&numeroJugadores, &modoJuego)) break;
        numeroJugadores = 2;
        juego(numeroJugadores, modoJuego);
    }
    

    return 0;
}




