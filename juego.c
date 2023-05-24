#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <math.h>

#include "list.h"
#include "juego.h"
#include "functAux.h"
#define VACIO " " 
#define DELAY 100
//NÚMERO CARACTER EN EL CÓDIGO ASCII
#define MISIL 207
#define JUGADOR_1 79
#define JUGADOR_2 157
#define BOOST_COLOR 15
#define BOOST_REVERSE 21
#define BOOST_SCORE 4
#define BOOST_APPEND 1
#define BOOST_SLOW 17
#define BOOST_FAST 16
#define BOOST_LIFE 3
//TECLAS
#define KEY_I 73
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76
#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
//DIRECCIONES DE MOVIMIENTO
#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3
#define STOP -1

struct Pixel{ 
    COORD pos;
    char* value;
};

//Se dividen para facilitar la validación cundo el jugador colisiona con un muro
struct Muro{ 
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
    short lifes;
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
        nuevoJuego[i]->P1->lifes = 5;
        nuevoJuego[i]->P1->choco = false;
        nuevoJuego[i]->P2 = (Player*) malloc( sizeof( Player ) );
        nuevoJuego[i]->P2->snake = createList();
        nuevoJuego[i]->P2->largoSnake = 0;
        nuevoJuego[i]->P2->opcionMover = -1;
        nuevoJuego[i]->P2->lifes = 5;
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
    newMisil->pixel->value = (char *) MISIL;
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
    fp = fopen ( "archivosCSV\\PropiedadesJuego.csv", "r");
    if(!fp){
        system("cls");
        system("color 0d");
        gotoxy(0,0);
        printf("ERROR AL ABRIR EL ARCHIVO DE PROPIEDADES DEL JUEGO\n");
        system("pause");
        exit(1);
    }
    char linea[1024];
    char* aux;
    int nivel;
    int constante, rangoFinal, rangoInicial;
    
    while (fgets (linea, 1023, fp) != NULL) { 
        Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
        aux = (char *) get_csv_field(linea, 0);
        nivel = atoi(aux); //El primer elemento indica el nivel de juego en que se agregran las características

        aux = (char *) get_csv_field(linea, 1);
        if( strcmp( aux, "player") == 0 ){ //Si el elemento es player obtiene las caracteristicas
            Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
            nuevoPixel->value = (char *) JUGADOR_1; //caracter O
            pushFront( juego[nivel]->P1->snake, nuevoPixel ); //Agrega una sección a la serpiente del jugador 1
            juego[nivel]->P1->largoSnake += 1;
            if(numJugadores == 2){
                Pixel* nuevoPixel = (Pixel*) malloc( sizeof(Pixel) );
                nuevoPixel->value = (char *) JUGADOR_2 ; //caracter Ø
                pushFront( juego[nivel]->P2->snake, nuevoPixel ); //Agrega una sección a la serpiente del jugador 1
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
            nuevoBoost->pixel->value = (char *) BOOST_COLOR ; //caracter ☼
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "reverseBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char *) BOOST_REVERSE; //caracter §
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "appendBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char *) BOOST_APPEND; //caracter ☺
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "fastBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char *) BOOST_FAST; //caracter ►
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "slowBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char *) BOOST_SLOW; //caracter ◄
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "scoreBoost") == 0 && numJugadores == 1 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char *) BOOST_SCORE; //caracter ♦
            juego[nivel]->cantPowerUps += 1;
            pushFront( juego[nivel]->powerUps, nuevoBoost );
        }
        else if( strcmp( aux, "lifeBoost") == 0 ){
            Boost *nuevoBoost = createBoost();
            nuevoBoost->pixel->value = (char *) BOOST_LIFE; //caracter ♥
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
    int mitadX = (int) ((bordeSupDerecho->pos.X - bordeInfIzquierdo->pos.X)/2) + bordeInfIzquierdo->pos.X;
    mitadX += mitadX % 2; //Mitad del eje x
    int mitadY = (int) ((bordeInfIzquierdo->pos.Y - bordeSupDerecho->pos.Y)/2) + bordeSupDerecho->pos.Y;
    mitadY -= 2; //Mitad del eje y

    if(numJugadores == 2){ // Si son 2, el jugador uno esta desplazado mas a la derecha y el dos más a la izquierda
        jugador = (Pixel*) firstList( juego->P1->snake );
        i = 0;
        while( jugador ){ //Las secciones se dibujan una abajo de la otra
            jugador->pos.X = mitadX + 8;
            jugador->pos.Y = mitadY + i;
            gotoxy( jugador->pos.X, jugador->pos.Y );
            printf("%c", jugador->value);
            jugador = (Pixel*) nextList( juego->P1->snake );
            i += 1;
        }
        juego->P1->opcionMover = STOP;
        juego->P1->choco = false;

        jugador = (Pixel*) firstList( juego->P2->snake );
        i = 0;
        while( jugador ){ //Las secciones se dibujan una abajo de la otra
            jugador->pos.X = mitadX - 8;
            jugador->pos.Y = mitadY + i;
            gotoxy( jugador->pos.X, jugador->pos.Y );
            printf("%c", jugador->value);
            jugador = (Pixel*) nextList( juego->P2->snake );
            i += 1;
        }
        juego->P2->opcionMover = STOP;
        juego->P2->choco = false;
    }
    else{//Si es un jugador solo queda centrado
        jugador = (Pixel*) firstList( juego->P1->snake );
        i = 0;
        while( jugador ){ //Las secciones se dibujan una abajo de la otra
            jugador->pos.X = mitadX;
            jugador->pos.Y = mitadY + i;
            gotoxy( jugador->pos.X, jugador->pos.Y );
            printf("%c", jugador->value);
            jugador = (Pixel*) nextList( juego->P1->snake );
            i += 1;
        }
        juego->P1->opcionMover = STOP;
        juego->P1->choco = false;
    }
    //Se inicializa su movimiento como quieto y su bool choco en falso
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

//Asigna una nueva lista vacía de misiles
void inicializarMisiles(Game* juego){
    juego->misiles = createList();
}

//Inicializa un nuevo nivel en caso de pasar al siguiente o perder una vida
void inicializarNivel( Game* juego, int numJugadores ){
    sndPlaySound( "musica//juego.wav" , SND_ASYNC | SND_LOOP);
    inicializarJugador( juego, numJugadores );
    inicializarBordes( juego->wallStatic );
    inicializarPowerUps( juego->powerUps );
    inicializarMisiles( juego );
}

//Dibua la cabez del jugador en la pantalla (el movimiento como serpiente solo requiere dibujar la cabeza)
void printCabezaJugador(List* player){
    Pixel* jugador = (Pixel*) firstList( player );
    if(jugador){
        gotoxy( jugador->pos.X, jugador->pos.Y );
        printf("%c", jugador->value);
    }
}

//Dibuja a un misil que recibe la función
void printMisil(Misil* misil){
    gotoxy( misil->pixel->pos.X, misil->pixel->pos.Y );
    printf("%c", misil->pixel->value);
}

//Recorre todos los misiles y los mueve un espacio en la pantalla
void moverMisiles(Game* juego, Boost* boost, int time ){ 
    Misil* misil = (Misil*) firstList(juego->misiles);
    while(misil){
        int g = misil->grado;//Dependiendo del grado del misil, va a ir más rápido o más lento
        if((g == 0 && time % 1 == 0) || (g == 1 && time % 2 == 0) || (g == 2 && time % 3 == 0) ){ 
            gotoxy( misil->pixel->pos.X, misil->pixel->pos.Y );
            printf("%s", VACIO);
            misil->pixel->pos.X += misil->velX;
            misil->pixel->pos.Y += misil->velY;
            if(misil->pixel->pos.X == misil->FinalX && misil->pixel->pos.Y == misil->FinalY){//Si llega al muro se borra
                popCurrent(juego->misiles);
            }
            else{
                if(misil->pixel->pos.X == boost->pixel->pos.X && misil->pixel->pos.Y == boost->pixel->pos.Y){//Si colisiona un boost lo elimina
                    borrarPowerUp(boost);
                }
                
                printMisil(misil);
            }
        }

        misil = (Misil*) nextList(juego->misiles);
    }
    
    //Solo por si el jugador pasa detras del misil y se borra accidentalmente
    printCabezaJugador(juego->P1->snake);
    printCabezaJugador(juego->P2->snake);
}

//Por cada misil, evalua si el misil colisiona con la serpiente de algun jugador
void validarChoqueMisil(Player* player, List* misiles){
    Misil* misil = (Misil*) firstList( misiles );
    while(misil){
        Pixel* snake = (Pixel*) firstList( player->snake );
        while(snake){
            if( (misil->pixel->pos.X == snake->pos.X) && (misil->pixel->pos.Y == snake->pos.Y)){
                player->choco = true;
                return;
            }
            snake = (Pixel*) nextList( player->snake );
        }
        misil = (Misil*) nextList( misiles );
    }
}

//Valida cuando una serpiente choca con la del otro jugador o con su propia cola
void validarChoqueSnakes(Player* playerA, Player* playerB){
    Pixel* cabeza = (Pixel*) firstList(playerA->snake);
    Pixel* snakeRival = (Pixel*) firstList(playerB->snake);
    if( snakeRival && cabeza ){
        if(cabeza->value == snakeRival->value){ //Si son la misma serpiente se salta una sección de cuerpo para no evaluar con su propia cabeza
            snakeRival = (Pixel*) nextList(playerB->snake);
        }
        while(snakeRival){
            if(cabeza->pos.X == snakeRival->pos.X && cabeza->pos.Y == snakeRival->pos.Y){
                playerA->choco = true;
                return;
            }
            snakeRival = (Pixel*) nextList(playerB->snake);
        }
    }
}

//Se evalúa por todas las formas posibles de colision
void validarChoques(Game* juego){
    validarChoqueSnakes(juego->P1, juego->P2);
    validarChoqueSnakes(juego->P2, juego->P1);
    validarChoqueSnakes(juego->P1, juego->P1);
    validarChoqueSnakes(juego->P2, juego->P2);
    validarChoqueMisil( juego->P1, juego->misiles );
    validarChoqueMisil(juego->P2, juego->misiles );
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
    jugadorCola->pos.Y = jugadorCabeza->pos.Y - 1;//La actualiza y la agrega
    pushFront( player->snake, jugadorCola);
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
    jugadorCola->pos.Y = jugadorCabeza->pos.Y + 1;//La actualiza y la agrega
    pushFront( player->snake, jugadorCola);
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

    jugadorCola->pos.X = jugadorCabeza->pos.X + 2;//La actualiza y la agrega
    jugadorCola->pos.Y = jugadorCabeza->pos.Y;
    pushFront( player->snake, jugadorCola);
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

    jugadorCola->pos.X = jugadorCabeza->pos.X - 2;//La actualiza y la agrega
    jugadorCola->pos.Y = jugadorCabeza->pos.Y;
    pushFront( player->snake, jugadorCola);
    return false;
}
//***********************************************************//

void moverPlayers(Game* juego, Boost* boostActual, int numJugadores){
    if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState(KEY_I)){

        //En caso de que el boost de reversa este activo se invertirán las opciones
        if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){ 
            if ( juego->P1->opcionMover != UP){//Valida que el jugador no vuelva por su propia cola
                juego->P1->opcionMover = DOWN;
            }
        }
        else{
            if ( juego->P1->opcionMover != DOWN ){//Valida que el jugador no vuelva por su propia cola
                juego->P1->opcionMover = UP;
            } 
        } 
    }
    else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState(KEY_K)){
        if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
            if ( juego->P1->opcionMover != DOWN){
                juego->P1->opcionMover = UP;
            }
        }
        else{
            //Condición extra, si el jugador esta quieto, no podrá ir hacia abajo
            if ( juego->P1->opcionMover != UP && juego->P1->opcionMover != STOP){ //Para no volver por su cola cuando se inicializa una partida
                juego->P1->opcionMover = DOWN;
            }
        }
    }
    else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(KEY_L)){
        if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
            if ( juego->P1->opcionMover != RIGHT){
                juego->P1->opcionMover = LEFT;
            }
        }
        else{
            if ( juego->P1->opcionMover != LEFT){
                juego->P1->opcionMover = RIGHT;
            } 
        }
    }
    else if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(KEY_J)){
        if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
            if ( juego->P1->opcionMover != LEFT){
                juego->P1->opcionMover = RIGHT;
            }
        }
        else{
            if ( juego->P1->opcionMover != RIGHT){
                juego->P1->opcionMover = LEFT;
            } 
        }
    }
    switch (juego->P1->opcionMover)
    {
    case 0://UP
        juego->P1->choco = moverUpPlayer( juego->P1, juego->wallStatic->muroUp );
        break;
    case 1://DOWN
        juego->P1->choco = moverDownPlayer( juego->P1, juego->wallStatic->muroDown );
        break;
    case 2://RIGHT
        juego->P1->choco = moverRightPlayer( juego->P1, juego->wallStatic->muroRight );
        break;
    case 3://LEFT
        juego->P1->choco = moverLeftPlayer( juego->P1, juego->wallStatic->muroLeft );
        break;
    default:
    //Si el jugador esta quieto, pero el adversario avanzo, este también se moverá automáticamente hacia arriba
        if( numJugadores == 2){
            if(juego->P2->opcionMover != STOP){
                juego->P1->opcionMover = UP;
            }
        }
        break;
    }

    //Solo si existe un jugador 2
    if(numJugadores == 2){
        if (GetAsyncKeyState(KEY_W)){
            if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
                if ( juego->P2->opcionMover != UP){
                    juego->P2->opcionMover = DOWN;
                }
            }
            else{
                if ( juego->P2->opcionMover != DOWN  ){
                    juego->P2->opcionMover = UP;
                } 
            } 
        }
        else if (GetAsyncKeyState(KEY_S)){
            if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
                if ( juego->P2->opcionMover != DOWN){
                    juego->P2->opcionMover = UP;
                }
            }
            else{
                if ( juego->P2->opcionMover != UP && juego->P2->opcionMover != STOP ){
                    juego->P2->opcionMover = DOWN;
                }
            }
        }
        else if (GetAsyncKeyState(KEY_D)){
            if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
                if ( juego->P2->opcionMover != RIGHT){
                    juego->P2->opcionMover = LEFT;
                }
            }
            else{
                if ( juego->P2->opcionMover != LEFT){
                    juego->P2->opcionMover = RIGHT;
                } 
            }
        }
        else if (GetAsyncKeyState(KEY_A)){
            if( boostActual->activo && (char *)boostActual->pixel->value == (char *) BOOST_REVERSE ){
                if ( juego->P2->opcionMover != LEFT){
                    juego->P2->opcionMover = RIGHT;
                }
            }
            else{
                if ( juego->P2->opcionMover != RIGHT){
                    juego->P2->opcionMover = LEFT;
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
        //Si el jugador esta quieto, pero el adversario avanzo, este también se moverá automáticamente hacia arriba
            if(juego->P1->opcionMover != STOP){
                juego->P2->opcionMover = UP;
            }
            break;
        }
        
        //Se dibujan las serpientes en este orden para que no haya mucha diferencia entre la que se movió antes y la que lo hizo después
        printCabezaJugador(juego->P2->snake);
    }
    printCabezaJugador(juego->P1->snake);
    
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
    if( (char *)boost->pixel->value == (char *) BOOST_FAST ){ 
        if(*delayBoost == 1){
            *delayBoost = 0.5;//Delay más cortos
        }
    }
    else if( (char *)boost->pixel->value == (char *) BOOST_SLOW ){ 
        if(*delayBoost == 1){
            *delayBoost = 2;//Delay más largos
        }
    }
    else if( (char *)boost->pixel->value == (char *) BOOST_SCORE ){
        if(*scoreBoost == 1){
            *scoreBoost = 8;//Indice de aumento mayor en el puntaje
        }
    }
}

//Función del appendBoost que agrega un nuevo segmento a la serpiente del jugador que lo recoja
void agregarCola( Player* player ){
    char* valor = ((Pixel*)firstList(player->snake))->value; 
    if(player->largoSnake < 5){ //Máximo de 5 segmentos
        Pixel* colaNueva = (Pixel*) malloc( sizeof(Pixel) );
        colaNueva->pos.X = -1;
        colaNueva->pos.Y = -1;
        if(valor == (char *) JUGADOR_1 ){ //Agrega el caracter apropiado dependiendo del jugador
            colaNueva->value = (char *) JUGADOR_1;
        }
        else{
            colaNueva->value = (char *) JUGADOR_2;
        }
        pushBack( player->snake , colaNueva);
        player->largoSnake += 1;
    }
}

//Imprime el nivel actual en el que se encuentra el jugador
void printNivel( List* muroDown, int nivel){
    Pixel* bordeInf = (Pixel*) firstList(muroDown);
    gotoxy(5,bordeInf->pos.Y + 1);
    printf("NIVEL: %i", nivel);
}

//Imprime el boost activo actual
void printBoost(Boost* boost){
    if(boost->activo){
        if((char *)boost->pixel->value == (char *) BOOST_SCORE) printf("SCOREBOOST");
        if((char *)boost->pixel->value == (char *) BOOST_REVERSE) printf("REVERSEBOOST");
        if((char *)boost->pixel->value == (char *) BOOST_SLOW) printf("SLOWBOOST");
        if((char *)boost->pixel->value == (char *) BOOST_FAST) printf("FASTBOOST");
    }
    else printf("             ");
}

//Entrega el mensaje para cuando ocurre una colisión
void imprimirPierdeVida(int numJugadores, bool chocoP1, bool chocoP2){
    GetAllKeys();
    gotoxy(20, 5);
    if(numJugadores == 1){
        printf("OH NOO... PIERDES UNA VIDA");
    }
    else{
        if(chocoP1 && chocoP2){
            printf("OH NOO... AMBOS PIERDEN UNA VIDA");
        }
        else if(chocoP1){
            printf("OH NOO... JUGADOR 1 PIERDE UNA VIDA");
        }
        else{
            printf("OH NOO... JUGADOR 2 PIERDE UNA VIDA");
        }
    }
    gotoxy(20, 7);
    system("pause");
    GetAllKeys();
}

//Imprime informaciones generales del juego actual
void printInfo(Game* juego, Boost* boost , int score, int time, int numJugadores ){
    Pixel* bordeInf = (Pixel*) firstList(juego->wallStatic->muroDown);
    gotoxy(5,bordeInf->pos.Y + 2);
    time = (int) time/100;
    if(numJugadores == 1){
        printf("SCORE: %5i         VIDAS: %2i       TIME: %4i         ", score, juego->P1->lifes, time);    
    }
    else{
        printf("VIDAS PLAYER 1: %2i   VIDAS PLAYER 2: %2i   TIME: %4i   ", juego->P1->lifes, juego->P2->lifes, time);   
    }
    printBoost( boost );
}

//Entrega el mensaje para cuando termina la partida
void infoFinalJuego(Game* juego, int numJugadores, int score ){
    GetAllKeys();
    gotoxy(20, 5);
    if(numJugadores == 1){
        printf("TE HAS QUEDADO SIN VIDAS Y TU PUNTUACION ES DE %i PUNTOS", score);
    }
    else{
        if(juego->P1->lifes == 0 && juego->P2->lifes == 0){
            printf("ES UN EMPATE");
        }
        else if(juego->P1->lifes != 0){
            printf("GANO EL JUGADOR 1");
        }
        else{
            printf("GANO EL JUGADOR 2");
        }
    }
    gotoxy(20, 7);
    system("pause");
    GetAllKeys();
}

//Agrega el puntaje que consiguió el jugador en el csv de HighScores
void agregarScore(int modoJuego, int score, char* nombre){
    FILE *fp;
 	fp = fopen ( "archivosCSV\\HighScores.csv", "r+" );
 	
    if(!fp){
        system("cls");
        system("color 0d");
        gotoxy(0,0);
        printf("ERROR AL ABRIR EL ARCHIVO PARA AGREGAR UN NUEVO HIGHSCORE\n");
        system("pause");
        exit(1);
    }

    while (feof(fp) == 0){
        fgetc(fp);
    }
    fprintf(fp, "%i,%i,%s\n", modoJuego, score, nombre);

 	fclose ( fp );
}

void juego(int numJugadores , int modoJuego, char* nombreJugador){
    ocultarCursor();
    Game** juego = createGame();
    Boost* boostActual = createBoost();
    importarGame( juego, numJugadores );

    short nivel;
    if(modoJuego != -1) nivel = modoJuego;
    else nivel = 0;
    float delayBoost = 1;
    int scoreBoost = 1;
    int time = 0;
    long long score = -2;
    int limiteBoost;

    while (juego[nivel]->P1->lifes > 0 && juego[nivel]->P2->lifes > 0 && nivel < 5){ //Mientras ningun jugador haya perdido todas las vidas
        inicializarNivel( juego[nivel], numJugadores );
        printNivel(juego[nivel]->wallStatic->muroDown, nivel+1);
        limiteBoost = -100;

        while(true){ //Mientras ninguno haya colisionado o no hayan pasado de nivel

            Pixel* P1 = (Pixel*) firstList(juego[nivel]->P1->snake);
            Pixel* P2 = (Pixel*) firstList(juego[nivel]->P2->snake);
            time += (int) (10 * delayBoost);
            score += 2 * scoreBoost;
            Sleep( (int) (DELAY * delayBoost) );
            scoreBoost = 1;
            delayBoost = 1;
            printInfo(juego[nivel], boostActual, score, time, numJugadores);
            moverPlayers(juego[nivel], boostActual, numJugadores);

            //Todo lo que se ejecuta cuando el jugador no esta en pausa
            if(juego[nivel]->P1->opcionMover != STOP ){

                //Agrega un boost random cada 30 segundos
                if((int)(time/10) % 300 == 0){
                    if(time == 3000){
                        srand(P1->pos.X * P1->pos.Y);
                    }
                    boostActual = ubicarBoostRand(juego[nivel]);
                    limiteBoost = time + 1000;
                }
                
                //Activa el boost actual y guarda por referencia al jugador que lo activó. Dura 19 segundos activos, solo si no es de un uso
                if(P1->pos.X == boostActual->pixel->pos.X && P1->pos.Y == boostActual->pixel->pos.Y){
                    boostActual->activo = true;
                    boostActual->pixel->pos.X = -1;
                    boostActual->pixel->pos.Y = -1;
                    boostActual->refer = juego[nivel]->P1;
                    limiteBoost = time + 1900;
                }
                if(numJugadores == 2){
                    if(P2->pos.X == boostActual->pixel->pos.X && P2->pos.Y == boostActual->pixel->pos.Y){
                        boostActual->activo = true;
                        boostActual->pixel->pos.X = -1;
                        boostActual->pixel->pos.Y = -1;
                        boostActual->refer = juego[nivel]->P2;
                        limiteBoost = time + 1900;
                    }
                }

                if( boostActual->activo ){
                    //Activa boost de un uso
                    if( (char *)boostActual->pixel->value == (char *) BOOST_LIFE ){// Otra vida
                        boostActual->refer->lifes += 1; //Depende del jugador
                        limiteBoost = time;
                    }
                    else if((char *)boostActual->pixel->value == (char *) BOOST_APPEND){// Otra cola
                        agregarCola( boostActual->refer ); //Depende del jugador
                        limiteBoost = time; 
                    }
                    else if((char *)boostActual->pixel->value == (char *) BOOST_COLOR){// Otro color
                        colorRandom();
                        limiteBoost = time;
                    }
                    else{
                        //Activa la mayoria de boost que se prolongan en el tiempo
                        activarBoost(juego[nivel], boostActual, &delayBoost, &scoreBoost);
                    }
                }

                if( ( ((int) (time/100)) * 100) == ( ((int) (limiteBoost/100)) * 100) ){ //Elimina un boost tras 10 segundos de activarse
                    limiteBoost = -100;
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

                //El boost fast hace que el tiempo suba de 2 en 2 y la mayoria de funciones ocupan números pares
                if(boostActual->activo && boostActual->pixel->value == (char *) BOOST_FAST ){ 
                    time += ( (int) time/10 % 2)*10;          //Elimina el desface de tiempo para que solo recorra pares de 2 en 2
                }
                
                if( (int)(time/10) % 20 == 0){ //Genera misil cada 2 segundos
                    Misil* misil = createMisil( juego[nivel] );
                    pushBack(juego[nivel]->misiles, misil );
                    printMisil(misil);
                }

                moverMisiles(juego[nivel], boostActual, (int)(time/10));
                validarChoques(juego[nivel]); 
            }
            else{ //Evita que el tiempo y score avance
                time -= (int) (10 * delayBoost);
                score -= 2 * scoreBoost;
            }  

            //Si alguno choca se resta una vida y se rompe el ciclo
            if(juego[nivel]->P1->choco || juego[nivel]->P2->choco){
                if(juego[nivel]->P1->choco){
                    juego[nivel]->P1->lifes -= 1;  
                }
                if(juego[nivel]->P2->choco){
                    juego[nivel]->P2->lifes -= 1;
                }
                sndPlaySound( "musica//colision.wav" , SND_ASYNC);
                system("cls");
                system("color 0d");
                imprimirPierdeVida(numJugadores, juego[nivel]->P1->choco, juego[nivel]->P2->choco);
                break;
            }
            
            //Condiciones para pasar de nivel, solo en el caso de que se encuentre en el modo clásico y en un nivel inferior al 5
            if(modoJuego == -1 && nivel < 4){
                if(numJugadores == 1){
                    if(score >= 2000 * pow(2, nivel)){
                        sndPlaySound( "musica//siguienteNivel.wav" , SND_SYNC);
                        borrarPowerUp(boostActual);
                        nivel += 1;
                        break;
                    } 
                }
                else{
                    if(time >= 10000 * (nivel + 1) ){
                        sndPlaySound( "musica//siguienteNivel.wav" , SND_SYNC);
                        borrarPowerUp(boostActual);
                        nivel += 1;
                        break;
                    } 
                }
            }
        }
        system("cls");
        system("color 0d");
    }
    
    infoFinalJuego( juego[nivel], numJugadores, score);

    if(numJugadores == 1){
        agregarScore(modoJuego + 1, score, nombreJugador);
    }
}