#include <stdbool.h>

#include "list.h"
#include "menu.h"
#include "juego.h"

int main(){

    int numeroJugadores;
    int modoJuego;
    while(true){
        if(menu(&numeroJugadores, &modoJuego)) break;
        juego(numeroJugadores, modoJuego);
    }
    

    return 0;
}




