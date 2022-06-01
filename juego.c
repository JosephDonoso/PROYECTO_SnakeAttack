#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <math.h>

#include "list.h"
#include "juego.h"
#include "functAux.h"
//Este es el string que reemplaza un pixel en caso de desaparecer
#define VACIO " " 
#define DELAY 100

struct Pixel{ 
    COORD pos;
    char* value;
};
 
struct Muro{ //Se dividen para facilitar la validación cundo el jugador colisiona con un muro
    List* muroUp;
    List* muroDown;
    List* muroRight;
    List* muroLeft;
};

struct Boost{ 
    bool activo;
    Pixel* pixel;
    Player* refer;
};

struct Misil{
    Pixel* pixel;
    short grado;
    short velX, velY;
    short FinalX, FinalY;
};

struct Player{
    List* snake;
    short largoSnake;
    short opcionMover;
    short lives;
    bool choco;
};

struct Game{
    Player* P1;
    Player* P2;
    Muro* wallStatic;
    List* powerUps;
    short cantPowerUps;
    List* misiles;
};

Muro * createWallStatic(){ //Crea un tipo muro y las listas para los 4 muros que contiene
    Muro * bordes = (Muro*) malloc( sizeof(Muro) ); 
    bordes->muroUp = createList();
    bordes->muroDown = createList();
    bordes->muroRight = createList();
    bordes->muroLeft = createList();
    return bordes;
}

Game ** createGame(){ //Crea un array de 5 games, uno por nivel, y reserva memoria a sus componentes
    Game **nuevoJuego = (Game**) malloc( 5*sizeof(Game*) );
    for(short i = 0; i < 5; i++){
        nuevoJuego[i] = (Game*) malloc( sizeof( Game ) );
        nuevoJuego[i]->P1 = (Player*) malloc( sizeof( Player ) );
        nuevoJuego[i]->P1->snake = createList();
        nuevoJuego[i]->P1->largoSnake = 0;
        nuevoJuego[i]->P1->opcionMover = -1;
        nuevoJuego[i]->P1->lives = 5;
        nuevoJuego[i]->P1->choco = false;
        nuevoJuego[i]->P2 = (Player*) malloc( sizeof( Player ) );
        nuevoJuego[i]->P2->snake = createList();
        nuevoJuego[i]->P2->largoSnake = 0;
        nuevoJuego[i]->P2->opcionMover = -1;
        nuevoJuego[i]->P2->lives = 5;
        nuevoJuego[i]->P2->choco = false;
        nuevoJuego[i]->wallStatic = createWallStatic();
        nuevoJuego[i]->powerUps = createList();
        nuevoJuego[i]->cantPowerUps = 0;
    }
    return nuevoJuego;
}

Boost* createBoost(){ //Crea y reserva memoria para un nuevo boost
    Boost* nuevoBoost = (Boost*) malloc( sizeof(Boost) );
    nuevoBoost->activo = false;
    nuevoBoost->pixel = (Pixel*) malloc( sizeof(Pixel) );
    nuevoBoost->pixel->pos.X = -1;
    nuevoBoost->pixel->pos.Y = -1;
    nuevoBoost->refer = NULL;
    return nuevoBoost;
}

Misil* createMisil( Game* juego ){
    Misil* newMisil = (Misil*) malloc(sizeof(Misil));
    newMisil->pixel = (Pixel*) malloc(sizeof(Pixel));
    newMisil->pixel->value = (char*) 207;
    newMisil->grado = rand() % 3;

    int orientacion = rand() % 4;
    Pixel* bordeSupDerecho = (Pixel*) lastList(juego->wallStatic->muroUp);
    Pixel* bordeInfIzquierdo = (Pixel*) firstList(juego->wallStatic->muroDown);
    if(orientacion == 0){ //Arriba
        newMisil->pixel->pos.Y = bordeSupDerecho->pos.Y + 1;
        newMisil->pixel->pos.X = randRange(bordeInfIzquierdo->pos.X+2, bordeSupDerecho->pos.X-2);
        newMisil->pixel->pos.X += newMisil->pixel->pos.X % 2;
        newMisil->velX = 0;
        newMisil->velY = 1;
        newMisil->FinalX = newMisil->pixel->pos.X;
        newMisil->FinalY = bordeInfIzquierdo->pos.Y;
    }
    else if(orientacion == 1){ //Abajo
        newMisil->pixel->pos.Y = bordeInfIzquierdo->pos.Y - 1;
        newMisil->pixel->pos.X = randRange(bordeInfIzquierdo->pos.X+2, bordeSupDerecho->pos.X-2);
        newMisil->pixel->pos.X += newMisil->pixel->pos.X % 2;
        newMisil->velX = 0;
        newMisil->velY = - 1;
        newMisil->FinalX = newMisil->pixel->pos.X;
        newMisil->FinalY = bordeSupDerecho->pos.Y;
    }
    else if(orientacion == 2){ //Izquierda
        newMisil->pixel->pos.Y = randRange(bordeSupDerecho->pos.Y+1,bordeInfIzquierdo->pos.Y-1);
        newMisil->pixel->pos.X = bordeInfIzquierdo->pos.X + 2;
        newMisil->pixel->pos.X += newMisil->pixel->pos.X % 2;
        newMisil->velX = 2;
        newMisil->velY = 0;
        newMisil->FinalY = newMisil->pixel->pos.Y;
        newMisil->FinalX = bordeSupDerecho->pos.X;
        newMisil->FinalX += newMisil->FinalX % 2;
    }
    else if(orientacion == 3){ //Derecha
        newMisil->pixel->pos.Y = randRange(bordeSupDerecho->pos.Y+1,bordeInfIzquierdo->pos.Y-1);
        newMisil->pixel->pos.X = bordeSupDerecho->pos.X - 2;
        newMisil->pixel->pos.X += newMisil->pixel->pos.X % 2;
        newMisil->velX = -2;
        newMisil->velY = 0;
        newMisil->FinalY = newMisil->pixel->pos.Y;
        newMisil->FinalX = bordeInfIzquierdo->pos.X;
        newMisil->FinalX -= newMisil->FinalX % 2;
    }
    return newMisil;
}

//Importa desde un csv de propiedades las características del juego
void importarGame( Game** juego, int numJugadores ){
    FILE* fp = NULL;
    fp = fopen ( "PropiedadesJuego.csv", "r");
    if(!fp){
        printf("ERROR AL ABRIR EL ARCHIVO DE PROPIEDADES DEL JUEGO\n");
        system("pause");
        exit(1);
    }
    char linea[1024];
    char* aux;
    int nivel;
    int constante, rangoFinal, rangoInicial;
    
    while (fgets (linea, 1023, fp) != NULL) { //Mientras no se haya leído todo el archivo
        Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
        aux = (char *) get_csv_field(linea, 0);
        nivel = atoi(aux); //El primer elemento indica el nivel de juego en que se agregran las características

        aux = (char *) get_csv_field(linea, 1);
        if( strcmp( aux, "player") == 0 ){ //Si el elemento es player obtiene las caracteristicas
            Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
            nuevoPixel->value = (char *) 79;
            pushFront( juego[nivel]->P1->snake, nuevoPixel ); //Lo agrega en la lista de "player del juego actual"
            juego[nivel]->P1->largoSnake += 1;
            if(numJugadores == 2){
                Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
                nuevoPixel->value = (char*) 157;
                pushFront( juego[nivel]->P2->snake, nuevoPixel ); //Lo agrega en la lista de "player del juego actual"
                juego[nivel]->P2->largoSnake += 1;
            }
        }
        else if( strcmp( aux, "muroUp") == 0 ){//Si el elemento es muroUp obtiene los rangos para crearse
            aux = (char *) get_csv_field(linea, 2);
            rangoInicial = atoi(aux);//En la columna que empieza
            aux = (char *) get_csv_field(linea, 3);
            rangoFinal = atoi(aux);//En la columna que termina
            aux = (char *) get_csv_field(linea, 4);
            constante = atoi(aux);//En la fila que se situa

            aux = (char *) get_csv_field(linea, 5);//Se crea y se agrega a la lista
            for(int i = rangoInicial; i < rangoFinal; i++ ){
                Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
                nuevoPixel->pos.X = i;
                nuevoPixel->pos.Y = constante;
                nuevoPixel->value = aux;
                pushBack( juego[nivel]->wallStatic->muroUp, nuevoPixel );
            }
        }
        else if( strcmp( aux, "muroDown") == 0 ){//Si el elemento es muroDown obtiene los rangos para crearse
            aux = (char *) get_csv_field(linea, 2);
            rangoInicial = atoi(aux);//En la columna que empieza
            aux = (char *) get_csv_field(linea, 3);
            rangoFinal = atoi(aux);//En la columna que termina
            aux = (char *) get_csv_field(linea, 4);
            constante = atoi(aux);//En la fila que se situa

            aux = (char *) get_csv_field(linea, 5);//Se crea y se agrega a la lista
            for(int i = rangoInicial; i < rangoFinal; i++ ){
                Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
                nuevoPixel->pos.X = i;
                nuevoPixel->pos.Y = constante;
                nuevoPixel->value = aux;
                pushBack( juego[nivel]->wallStatic->muroDown, nuevoPixel );
            }
        }
        else if( strcmp( aux, "muroLeft") == 0 ){//Si el elemento es muroLeft obtiene los rangos para crearse
            aux = (char *) get_csv_field(linea, 2);
            rangoInicial = atoi(aux);//En la fila que empieza
            aux = (char *) get_csv_field(linea, 3);
            rangoFinal = atoi(aux);//En la fila que termina
            aux = (char *) get_csv_field(linea, 4);
            constante = atoi(aux);//En la columna que se situa

            aux = (char *) get_csv_field(linea, 5);//Se crea y se agrega a la lista
            for(int i = rangoInicial; i < rangoFinal; i++ ){
                Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
                nuevoPixel->pos.X = constante;
                nuevoPixel->pos.Y = i;
                nuevoPixel->value = aux;
                pushBack( juego[nivel]->wallStatic->muroLeft, nuevoPixel );
            }
        }
        else if( strcmp( aux, "muroRight") == 0 ){//Si el elemento es muroRight obtiene los rangos para crearse
            aux = (char *) get_csv_field(linea, 2);
            rangoInicial = atoi(aux);//En la fila que empieza 
            aux = (char *) get_csv_field(linea, 3);
            rangoFinal = atoi(aux);// En la fila que terina
            aux = (char *) get_csv_field(linea, 4);
            constante = atoi(aux);//En la columna que se situa

            aux = (char *) get_csv_field(linea, 5);//Se crea y se agrega a la lista
            for(int i = rangoInicial; i < rangoFinal; i++ ){
                Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
                nuevoPixel->pos.X = constante;
                nuevoPixel->pos.Y = i;
                nuevoPixel->value = aux;
                pushBack( juego[nivel]->wallStatic->muroRight, nuevoPixel );
            }
        }

        //Agrega cada boost correspondiente a la lista de powerups
        else if( strcmp( aux, "colorBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 15; //☼
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "reverseBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 21; //§
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "appendBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 1; //☺
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "fastBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 16; //►
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "slowBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 17; //◄
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "scoreBoost") == 0 && numJugadores == 1 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 4; //♦
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "liveBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char*) 3; //♥
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }

    }

    fclose(fp);
}

//Ubica al jugador en medio del nivel
void inicializarJugador( Game* juego, int numJugadores){
    Pixel* bordeSupDerecho = (Pixel*) lastList(juego->wallStatic->muroUp);
    Pixel* bordeInfIzquierdo = (Pixel*) firstList(juego->wallStatic->muroDown);
    Pixel* jugador;
    int i;
    int mitadX = (int) ((bordeSupDerecho->pos.X - bordeInfIzquierdo->pos.X)/2) + bordeInfIzquierdo->pos.X ;
    mitadX += mitadX % 2;
    int mitadY = (int) ((bordeInfIzquierdo->pos.Y - bordeSupDerecho->pos.Y)/2) + bordeSupDerecho->pos.Y;
    mitadY -= 2;

    if(numJugadores == 2){
        jugador = (Pixel*) firstList( juego->P1->snake );
        i = 0;
        while( jugador ){
            jugador->pos.X = mitadX + 6;
            jugador->pos.Y = mitadY + i;
            gotoxy( jugador->pos.X, jugador->pos.Y );
            printf("%c", jugador->value);
            jugador = (Pixel*) nextList( juego->P1->snake );
            i += 1;
        }
        juego->P1->opcionMover = -1;
        juego->P1->choco = false;

        jugador = (Pixel*) firstList( juego->P2->snake );
        i = 0;
        while( jugador ){
            jugador->pos.X = mitadX - 6;
            jugador->pos.Y = mitadY + i;
            gotoxy( jugador->pos.X, jugador->pos.Y );
            printf("%c", jugador->value);
            jugador = (Pixel*) nextList( juego->P2->snake );
            i += 1;
        }
        juego->P2->opcionMover = -1;
        juego->P2->choco = false;
    }
    else{
        jugador = (Pixel*) firstList( juego->P1->snake );
        i = 0;
        while( jugador ){
            jugador->pos.X = mitadX;
            jugador->pos.Y = mitadY + i;
            gotoxy( jugador->pos.X, jugador->pos.Y );
            printf("%c", jugador->value);
            jugador = (Pixel*) nextList( juego->P1->snake );
            i += 1;
        }
        juego->P1->opcionMover = -1;
        juego->P1->choco = false;
    }

}

//Recorre todo el muro y lo imprime
void printMuro(List* muro){
    Pixel* pix = (Pixel*) firstList(muro);
    while( pix ){

        gotoxy( pix->pos.X, pix->pos.Y );
        printf("%s", pix->value);
        pix = (Pixel*) nextList(muro);
    }
}

//Crea el escenario del nivel actual
void inicializarBordes( Muro* muros ){
    printMuro( muros->muroUp );
    printMuro( muros->muroDown );
    printMuro( muros->muroRight);
    printMuro( muros->muroLeft);
}

//Recibe el boost actual, lo quita de la pantalla y lo inicializa en falso
void borrarPowerUp(Boost* powerUp){
    powerUp->activo = false;
    if(powerUp->pixel->pos.X != -1){
        gotoxy(powerUp->pixel->pos.X, powerUp->pixel->pos.Y);
        printf("%s", VACIO);
    }
    powerUp->pixel->pos.X = -1;
    powerUp->pixel->pos.Y = -1;
    powerUp->refer = NULL;
}

//Borra e inicializa en falso todos los powerups
void inicializarPowerUps( List* powerUps ){
    Boost* powerUp = (Boost*) firstList(powerUps);
    while( powerUp ){
        borrarPowerUp(powerUp);
        powerUp = (Boost*) nextList(powerUps);
    }
}

void inicializarMisiles(Game* juego){
    juego->misiles = createList();
}

//Inicializa un nuevo nivel en caso de pasar al siguiente o perder una vida
void inicializarNivel( Game* juego, int numJugadores ){
    inicializarJugador( juego, numJugadores );
    inicializarBordes( juego->wallStatic );
    inicializarPowerUps( juego->powerUps );
    inicializarMisiles( juego );
}

//Dibua al jugador en la pantalla
void printJugador(List* player){
    Pixel* jugador = (Pixel*) firstList( player );
    while( jugador ){
        gotoxy( jugador->pos.X, jugador->pos.Y );
        printf("%c", jugador->value);
        jugador =  (Pixel*) nextList( player );
    }
}

void printMisil(Misil* misil){
    gotoxy( misil->pixel->pos.X, misil->pixel->pos.Y );
    printf("%c", misil->pixel->value);
}

void moverMisiles(Game* juego, Boost* boost, int time ){
    Misil* misil = (Misil*) firstList(juego->misiles);
    while(misil){
        int g = misil->grado;
        if((g == 0 && time % 1 == 0) || (g == 1 && time % 2 == 0) || (g == 2 && time % 3 == 0) ){
            gotoxy( misil->pixel->pos.X, misil->pixel->pos.Y );
            printf("%s", VACIO);
            misil->pixel->pos.X += misil->velX;
            misil->pixel->pos.Y += misil->velY;
            if(misil->pixel->pos.X == misil->FinalX && misil->pixel->pos.Y == misil->FinalY){
                popCurrent(juego->misiles);
            }
            else{
                if(misil->pixel->pos.X == boost->pixel->pos.X && misil->pixel->pos.Y == boost->pixel->pos.Y){
                    borrarPowerUp(boost);
                }
                printMisil(misil);
            }
        }

        misil = (Misil*) nextList(juego->misiles);
    }
}

//************ FUNCIONES PARA MOVER AL PERSONAJE ************//
//funciones booleanas que entregan true en caso de chocar o false si no ocurren colisiones
bool moverUpPlayer( Player* player, List* muroUp ){
    Pixel* jugadorCabeza = (Pixel*) firstList( player->snake );
    int posConstante = ((Pixel*) lastList( muroUp ))->pos.Y;

    if(jugadorCabeza->pos.Y - 1 == posConstante){//Si colisiona con el muro
        return true;
    }
    
    Pixel* jugadorCola = (Pixel*) popBack( player->snake );
    gotoxy( jugadorCola->pos.X, jugadorCola->pos.Y );
    printf("%s", VACIO);//Solo borra la cola

    jugadorCola->pos.X = jugadorCabeza->pos.X;
    jugadorCola->pos.Y = jugadorCabeza->pos.Y - 1;//La actualiza

    pushFront( player->snake, jugadorCola);
    printJugador(player->snake);//La vuelve a dibujar
    return false;
}

bool moverDownPlayer( Player* player, List* muroDown ){
    Pixel* jugadorCabeza = (Pixel*) firstList( player->snake );
    int posConstante = ((Pixel*) lastList( muroDown ))->pos.Y;

    if(jugadorCabeza->pos.Y + 1 == posConstante){//Si colisiona con el muro
        return true;
    }

    Pixel* jugadorCola = (Pixel*) popBack( player->snake );
    gotoxy( jugadorCola->pos.X, jugadorCola->pos.Y );
    printf("%s", VACIO);//Solo borra la cola

    jugadorCola->pos.X = jugadorCabeza->pos.X;
    jugadorCola->pos.Y = jugadorCabeza->pos.Y + 1;//La actualiza

    pushFront( player->snake, jugadorCola);
    printJugador(player->snake);//La vuelve a dibujar
    return false;
}

bool moverRightPlayer( Player* player, List* muroRight ){
    Pixel* jugadorCabeza = (Pixel*) firstList( player->snake );
    
    int posConstante = ((Pixel*) lastList( muroRight ))->pos.X;

    if(jugadorCabeza->pos.X + 2 >= posConstante){//Si colisiona con el muro
        return true;
    }

    Pixel* jugadorCola = (Pixel*) popBack( player->snake );
    gotoxy( jugadorCola->pos.X, jugadorCola->pos.Y );
    printf("%s", VACIO);//Solo borra la cola

    jugadorCola->pos.X = jugadorCabeza->pos.X + 2;//La actualiza
    jugadorCola->pos.Y = jugadorCabeza->pos.Y;

    pushFront( player->snake, jugadorCola);
    printJugador(player->snake);//La vuelve a dibujar
    return false;
}

bool moverLeftPlayer( Player* player, List* muroLeft ){
    Pixel* jugadorCabeza = (Pixel*) firstList( player->snake );
   
    int posConstante = ((Pixel*) lastList( muroLeft ))->pos.X;


    if(jugadorCabeza->pos.X - 2 <= posConstante){//Si colisiona con el muro
        return true;
    }

    Pixel* jugadorCola = (Pixel*) popBack( player->snake );
    gotoxy( jugadorCola->pos.X, jugadorCola->pos.Y );
    printf("%s", VACIO);//Solo borra la cola

    jugadorCola->pos.X = jugadorCabeza->pos.X - 2;//La actualiza
    jugadorCola->pos.Y = jugadorCabeza->pos.Y;

    pushFront( player->snake, jugadorCola);
    printJugador(player->snake);//La vuelve a dibujar
    return false;
}
//***********************************************************//

void moverPlayers(Game* juego, Boost* boostActual, int numJugadores){
    if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(73)){
        if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
            if ( juego->P1->opcionMover != 0){
                juego->P1->opcionMover = 1;
            }
        }
        else{
            if ( juego->P1->opcionMover != 1){
            juego->P1->opcionMover = 0;
            } 
        } 
    }
    else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(75)){
        if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
            if ( juego->P1->opcionMover != 1){
                juego->P1->opcionMover = 0;
            }
        }
        else{
            if ( juego->P1->opcionMover != 0){
            juego->P1->opcionMover = 1;
            }
        }
    }
    else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(76)){
        if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
            if ( juego->P1->opcionMover != 2){
                juego->P1->opcionMover = 3;
            }
        }
        else{
            if ( juego->P1->opcionMover != 3){
            juego->P1->opcionMover = 2;
            } 
        }
    }
    else if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(74)){
        if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
            if ( juego->P1->opcionMover != 3){
                juego->P1->opcionMover = 2;
            }
        }
        else{
            if ( juego->P1->opcionMover != 2){
            juego->P1->opcionMover = 3;
            } 
        }
    }

    switch (juego->P1->opcionMover)
    {
    case 0:
        juego->P1->choco = moverUpPlayer( juego->P1, juego->wallStatic->muroUp );
        break;
    case 1:
        juego->P1->choco = moverDownPlayer( juego->P1, juego->wallStatic->muroDown );
        break;
    case 2:
        juego->P1->choco = moverRightPlayer( juego->P1, juego->wallStatic->muroRight );
        break;
    case 3:
        juego->P1->choco = moverLeftPlayer( juego->P1, juego->wallStatic->muroLeft );
        break;
    default:
        if( numJugadores == 2){
            if(juego->P2->opcionMover != -1){
                juego->P1->opcionMover = 0;
            }
        }
        
        break;
    }

    if(numJugadores == 2){
        if (GetAsyncKeyState(87)){
            if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
                if ( juego->P2->opcionMover != 0){
                    juego->P2->opcionMover = 1;
                }
            }
            else{
                if ( juego->P2->opcionMover != 1){
                juego->P2->opcionMover = 0;
                } 
            } 
        }
        else if (GetAsyncKeyState(83)){
            if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
                if ( juego->P2->opcionMover != 1){
                    juego->P2->opcionMover = 0;
                }
            }
            else{
                if ( juego->P2->opcionMover != 0){
                juego->P2->opcionMover = 1;
                }
            }
        }
        else if (GetAsyncKeyState(68)){
            if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
                if ( juego->P2->opcionMover != 2){
                    juego->P2->opcionMover = 3;
                }
            }
            else{
                if ( juego->P2->opcionMover != 3){
                juego->P2->opcionMover = 2;
                } 
            }
        }
        else if (GetAsyncKeyState(65)){
            if( boostActual->activo && (char*)boostActual->pixel->value == (char*) 21 ){
                if ( juego->P2->opcionMover != 3){
                    juego->P2->opcionMover = 2;
                }
            }
            else{
                if ( juego->P2->opcionMover != 2){
                juego->P2->opcionMover = 3;
                } 
            }
        }

        switch (juego->P2->opcionMover)
        {
        case 0:
            juego->P2->choco = moverUpPlayer( juego->P2, juego->wallStatic->muroUp );
            break;
        case 1:
            juego->P2->choco = moverDownPlayer( juego->P2, juego->wallStatic->muroDown ); 
            break;
        case 2:
            juego->P2->choco = moverRightPlayer( juego->P2, juego->wallStatic->muroRight ); 
            break;
        case 3:
            juego->P2->choco = moverLeftPlayer( juego->P2, juego->wallStatic->muroLeft );
            break;
        default:
            if(juego->P1->opcionMover != -1){
                juego->P2->opcionMover = 0;
            }
            break;
        }
    }
}

//Retorna un boost random en una posición random restringida por los muros
Boost* ubicarBoostRand ( Game* juego ){
    //Se escoge uno random
    int boostRand = ((int) (rand()) % juego->cantPowerUps);
    Boost* boost = (Boost*) firstList( juego->powerUps );
    for( int i = 0; i < boostRand; i++){
        boost = (Boost*) nextList( juego->powerUps );
    }
    //Se ubica random
    Pixel* bordeSupDerecho = (Pixel*) lastList(juego->wallStatic->muroUp);
    Pixel* bordeInfIzquierdo = (Pixel*) firstList(juego->wallStatic->muroDown);
    boost->pixel->pos.X = randRange(bordeInfIzquierdo->pos.X+2, bordeSupDerecho->pos.X-2);
    boost->pixel->pos.X += boost->pixel->pos.X % 2;
    boost->pixel->pos.Y = randRange(bordeSupDerecho->pos.Y+1,bordeInfIzquierdo->pos.Y-1);
    //Se dibuja en pantalla
    gotoxy(boost->pixel->pos.X, boost->pixel->pos.Y);
    printf("%c", boost->pixel->value);
    return boost;
}

//Se activan ciertos boost que no cambian demasiado las caracteristicas del juego
void activarBoost(Game* juego, Boost* boost, float* delayBoost, int* scoreBoost){
    if( (char*)boost->pixel->value == (char*) 16 ){ //fastBoost
        if(*delayBoost == 1){
            *delayBoost = 0.5;
        }
    }
    else if( (char*)boost->pixel->value == (char*) 17 ){ //slowBoost
        if(*delayBoost == 1){
            *delayBoost = 2;
        }
    }
    else if( (char*)boost->pixel->value == (char*) 4 ){ //scoreBoost
        if(*scoreBoost == 1){
            *scoreBoost = 8;
        }
    }
}

//Función del appendBoost que agrega un nuevo segmento a la serpiente
//A MEJORAR PARA QUE EL APPEND SEA DEPENDIENTE AL JUGADOR QUE LO COLISIONA
void agregarCola( Player* player ){
    char* valor = ((Pixel*)firstList(player->snake))->value; 
    if(player->largoSnake < 5){ //Máximo de 5 segmentos
        Pixel* colaNueva = (Pixel*) malloc( sizeof(Pixel) );
        colaNueva->pos.X = -1;
        colaNueva->pos.Y = -1;
        if(valor == (char*) 79 ){
            colaNueva->value = (char*) 79;
        }
        else{
            colaNueva->value = (char*) 157;
        }
        pushBack( player->snake , colaNueva);
        player->largoSnake += 1;
    }
}

//Imprime el nivel actual en el que sencuentra el jugador
void printNivel( List* muroDown, int nivel){
    Pixel* bordeInf = (Pixel*) firstList(muroDown);
    gotoxy(5,bordeInf->pos.Y + 1);
    printf("NIVEL: %i", nivel);
}

//Imprime informaciones generales del juego actual
void printBoost(Boost* boost){
    if(boost->activo){
        if((char*)boost->pixel->value == (char*) 4) printf("SCOREBOOST");
        if((char*)boost->pixel->value == (char*) 21) printf("REVERSEBOOST");
        if((char*)boost->pixel->value == (char*) 17) printf("SLOWBOOST");
        if((char*)boost->pixel->value == (char*) 16) printf("FASTBOOST");
    }
    else printf("             ");
}

void printInfo(Game* juego, Boost* boost , int score, int time, int numJugadores ){
    Pixel* bordeInf = (Pixel*) firstList(juego->wallStatic->muroDown);
    gotoxy(5,bordeInf->pos.Y + 2);
    time = (int) time/100;
    if(numJugadores == 1){
        printf("SCORE: %5i         VIDAS: %2i       TIME: %4i         ", score, juego->P1->lives, time);    
    }
    else{
        printf("VIDAS PLAYER 1: %2i   VIDAS PLAYER 2: %2i   TIME: %4i   ", juego->P1->lives, juego->P2->lives, time);        
    }
    printBoost( boost );
}

void juego(int numJugadores , int nivelInf, char* nombreP1, char* nombreP2){
    ocultarCursor();
    Game** juego = createGame();
    Boost* boostActual = createBoost();
    importarGame( juego, numJugadores );

    short nivel;
    if(nivelInf != -1) nivel = nivelInf;
    else nivel = 0;
    float delayBoost = 1;
    int scoreBoost = 1;
    int time = 0;
    long long score = 0;
    int limiteBoost;
    while (juego[nivel]->P1->lives > 0 && juego[nivel]->P2->lives > 0 && nivel < 5){
        system("cls");
        system("color 0d");
        inicializarNivel( juego[nivel], numJugadores );
        printNivel(juego[nivel]->wallStatic->muroDown, nivel+1);
        limiteBoost = -1;

        while(true){
            Pixel* P1 = (Pixel*) firstList(juego[nivel]->P1->snake);
            Pixel* P2 = (Pixel*) firstList(juego[nivel]->P2->snake);
            time += (int) (10 * delayBoost);
            score += 2 * scoreBoost;
            Sleep( (int) (DELAY * delayBoost) );
            scoreBoost = 1;
            delayBoost = 1;

            if(juego[nivel]->P1->opcionMover != -1 ){
                if((int)(time/10) % 300 == 0){
                    if(time == 3000){
                        srand(P1->pos.X * P1->pos.Y);
                    }
                    boostActual = ubicarBoostRand(juego[nivel]);
                    limiteBoost = time+1000;
                }
                if(P1->pos.X == boostActual->pixel->pos.X && P1->pos.Y == boostActual->pixel->pos.Y){
                    boostActual->activo = true;
                    boostActual->refer = juego[nivel]->P1;
                    limiteBoost = time+1900;
                }
                if(numJugadores == 2){
                    if(P2->pos.X == boostActual->pixel->pos.X && P2->pos.Y == boostActual->pixel->pos.Y){
                        boostActual->activo = true;
                        boostActual->refer = juego[nivel]->P2;
                        limiteBoost = time+1900;
                    }
                }
                if( boostActual->activo ){
                    if( (char*)boostActual->pixel->value == (char*) 3 ){// Otra vida
                        boostActual->refer->lives += 1; //Depende del jugador
                        limiteBoost = time;
                    }
                    else if((char*)boostActual->pixel->value == (char*) 1){// Otra cola
                        agregarCola( boostActual->refer ); //Depende del jugador
                        limiteBoost = time; 
                    }
                    else if((char*)boostActual->pixel->value == (char*) 15){// Otro color
                        colorRandom();
                        limiteBoost = time;
                    }
                    else{
                        activarBoost(juego[nivel], boostActual, &delayBoost, &scoreBoost);
                    }
                }

                if(time == limiteBoost){
                    if(!boostActual->activo){
                        borrarPowerUp( boostActual );
                    }
                    else{
                        boostActual->activo = false;
                        boostActual->pixel->pos.X = -1;
                        boostActual->pixel->pos.Y = -1;
                        boostActual->refer = NULL;
                    }
                }

               if( (int)(time/10) % 20 == 0){
                    Misil* misil = createMisil( juego[nivel] );
                    pushBack(juego[nivel]->misiles, misil );
                    printMisil(misil);
                }
                moverMisiles(juego[nivel], boostActual, (int)(time/10)); 
            }
            else{
                time -= (int) (10 * delayBoost);
                score -= 2 * scoreBoost;
            }
            
            moverPlayers(juego[nivel], boostActual, numJugadores);

            if(juego[nivel]->P1->choco || juego[nivel]->P2->choco){
                if(juego[nivel]->P1->choco){
                    juego[nivel]->P1->lives -= 1;  
                }
                if(juego[nivel]->P2->choco){
                    juego[nivel]->P2->lives -= 1;
                }
                printInfo(juego[nivel], boostActual, score, time, numJugadores);
                break;
            }
            if(nivelInf == -1){
               if(score >= 2000 * pow(2, nivel)){
                    borrarPowerUp(boostActual);
                    nivel += 1;
                    break;
                } 
            }
            
            
            printInfo(juego[nivel], boostActual, score, time, numJugadores);
        }

    }

    if(juego[nivel]->P1->lives == 0){
        printf("HA PERDIDO");
    }
    else{
        printf("WOW LO HA CONSEGUIDO!!");
    }
    system("pause");

}

//CAMBIAR DIAGRAMA DE FLUJO PARA INGRESAR EL NOMBRE DEL USUARIO DEPENDIENDO DE LA SERPIENTE