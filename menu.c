#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

#include "list.h"
#include "menu.h"
#include "treemap.h"
#include "functAux.h"

//Estructura que agrupa las características de una partida
struct Propiedades{
    int numJugadores;
    int nivelJuego;
    char nombreJugador[15];
    bool flag;
};

//Reserva memoria para 6 mapas ordenados, uno por cada ranking distinto
TreeMap ** createArrTreeMaps(){
    TreeMap ** Mapas = (TreeMap**) malloc(6 * sizeof(TreeMap*));
    for(int i = 0; i < 6; i++){
        Mapas[i] = createTreeMap(higher_than);
    }
    return Mapas;
}

void instrucciones(){
    system("cls");
    gotoxy(0,5);
    printf("       ====================================== INSTRUCCIONES ======================================        \n");
    printf("       Luego de escoger el numero de jugadores y el modo de juego, la partida iniciara en un mapa\n");
    printf("       rectangular, donde el objetivo sera sobrevivir sin chocar con los bordes, los proyectiles\n");
    printf("       que recorren la pantalla o la cola de la serpiente del otro jugador, en caso que hubiese.\n");
    printf("       Las vidas con las que el jugador iniciara en cada nivel son 5 y las secciones de la\n");
    printf("       serpiente del jugador podra crecer hasta 5 secciones. El movimiento del jugador sera\n");
    printf("       constante hasta que cambie de direccion.\n\n");
    printf("       ======================================= POWER-UPS =========================================        \n");
    printf("       Apareceran por el mapa distintos tipos de Boost cada 30 segundos, si ningun jugador lo\n");
    printf("       recoge este desaparecera en 10 segundos, tambien desaparecera si algun proyectil lo\n");
    printf("       colisiona. Existen Boost con efectos de un solo uso y otros activos donde su efecto dura\n");
    printf("       19 segundos, estos seran los siguientes:\n");
    printf("       %c: De un uso. Otorga una vida extra.\n",(char*) 3);
    printf("       %c: De un uso. Cambia el color de los elementos\n",(char*) 15);
    printf("       %c: De un uso. Agrega una nueva seccion a la serpiente\n",(char*) 1);
    printf("       %c: Activo. El juego avanza mas lento mientras dure el efecto\n",(char*) 17);
    printf("       %c: Activo. El juego avanza mas rapido mientras dure el efecto\n",(char*) 16);
    printf("       %c: Activo. Los controles se invertiran mientras dure el efecto\n",(char*) 21);
    printf("       %c: Activo. El puntaje del jugador aumenta mas rapido (solo para 1 jugador)\n\n",(char*) 4);
    printf("       ===================================== MODOS DE JUEGO ======================================        \n");
    printf("       Habran 2 modos de juego:\n");
    printf("       Modo clasico: Donde el o los juagadores empezaran del nivel 1 y avanzaran hasta el 5\n");
    printf("       Modo infinito: Donde escogeran el nivel en el que jugaran y no podran pasar a otro\n");
    printf("       A la misma vez se podra escoger entre 1 o 2 jugadores. Con el modo 1 jugador, el puntaje\n");
    printf("       sera lo mas importante, deberan sobrevivir para obtener los mayores puntajes en su\n");
    printf("       categoria y su nombre quedara registrado en el highscore. Con el modo 2 jugadores, ambos\n");
    printf("       se enfrentaran por ganar la partida, quien se quede sin vidas perdera.\n\n");
    printf("       ======================================= CONTROLES =========================================        \n");
    printf("       Las teclas de movimiento para el jugador 1:\n");
    printf("       Arriba: I o cursor de navegacion UP\n");
    printf("       Abajo: K o cursor de navegacion DOWN\n");
    printf("       Izquierda: J o cursor de navegacion LEFT\n");
    printf("       Derecha: L o cursor de naveacion RIGHT\n");
    printf("       Las teclas de movimiento para el jugador 2:\n");
    printf("       Arriba: W\n");
    printf("       Abajo: S\n");
    printf("       Izquierda: A\n");
    printf("       Derecha: D\n\n");
    printf("       "), system("pause");
    GetAllKeys();
}

//Importa del csv de Highscores todos los rankings para 1 jugador y los clasifica por nivel entre 6 árboles
void importarHighscore(TreeMap** Mapas){
    FILE *fp;
 	fp = fopen ( "archivosCSV\\HighScores.csv", "r" );
 	
    if(!fp){
        system("cls");
        gotoxy(0,0);
        printf("ERROR AL ABRIR EL ARCHIVO PARA LEER EL HIGHSCORE\n");
        system("pause");
        exit(1);
    }

    char linea[1024];
    fgets (linea, 1023, fp);
    char* aux;
    int nivel;
    while (fgets (linea, 1023, fp) != NULL) {
        int* score = (int*) malloc(sizeof(int));
        char* nombre = (char*) malloc(15*sizeof(char));

        aux = (char *) get_csv_field(linea, 0);
        nivel = atoi(aux);
        aux = (char *) get_csv_field(linea, 1);
        *score = atoi(aux);
        aux = (char *) get_csv_field(linea, 2);
        strcpy(nombre, aux);
        insertTreeMap(Mapas[nivel], score, nombre); 
    }

 	fclose ( fp );
}

//Muestra los 6 ranking por orden de modo y nivel
void mostrarHighscore(){
    TreeMap ** Mapas = createArrTreeMaps();
    importarHighscore(Mapas);
    system("cls");
    gotoxy(0, 0);
    printf("\n\n\n\n                                            =========== HIGHSCORES ===========\n\n");
    int puesto, score;
    for(int i = 0; i < 6; i++){

        if(i == 0){
            printf("                                            ========== MODO CLASICO ==========\n");
        }
        else{
            printf("                                            ====== MODO INFINITO NIVEL %i =====\n", i);
        }

        Pair* jugador = firstTreeMap(Mapas[i]);
        if(!jugador){
            printf("                                               NO SE REGISTRAN PUNTUACIONES   \n");    
        } 
        else {
            printf("                                            PUESTO == SCORE == NOMBRE         \n");
        }

        puesto = 1;
        while ( jugador && puesto <= 10 ){
            score = *(int *)(jugador->key);
            printf("                                               %2i.-   %5i    %s\n" , puesto, score, jugador->value);
            jugador = nextTreeMap(Mapas[i]);
            puesto += 1;
        }
        printf("\n");
    }

    printf("\n                                            "), system("pause");
    free(Mapas);
    GetAllKeys();
}

//Se rcibe solo en caso de escoger el modo 1 jugador, ya que se registra en el ranking
void recibirNombre(char* nombreJugador){
    system("cls");
    gotoxy(0, 0);
    dibujarLogo();
    gotoxy(50, 31);
    printf("INGRESE SU NOMBRE: ");
    scanf("%s", nombreJugador);
    GetAllKeys();
}

//Recibe la tecla para mover la flecha que apunta a las opciones, retorna un buleano
//Si recibe el enter, la función retorna falso y la opción queda decidia por el usuario
bool cambiarOpcion(short * opcion){
    Sleep(250);
    fflush(stdin);
    getch();

    if( GetAsyncKeyState(VK_UP) ){
        *opcion -= 1;
    }
    else if( GetAsyncKeyState(VK_DOWN) ){
        *opcion += 1;
    }
    else if( GetAsyncKeyState(VK_RETURN) ){
        return false;
    }
    return true;
}

//Dibuja la flecha en la nueva posición
void ubicarFlecha(short opcion){
    gotoxy(55,31 + opcion);
    printf("-->");
}

//Se limpia el espacio donde se dibuja la flecha
void limpiarFlecha(){
    for(int i = 0; i < 6; i++){
        gotoxy(55, 31 + i); 
        printf("   ");
    }
}

//Último menú para escoger la dificultad del modo infinito
void menuNivel(Propiedades* prop){
    short opcion = 0;
    system("cls");
    gotoxy(0, 0);
    dibujarLogo();
    gotoxy(0, 31);
    printf("                                                            Nivel 1\n");
    printf("                                                            Nivel 2\n");
    printf("                                                            Nivel 3\n");
    printf("                                                            Nivel 4\n");
    printf("                                                            Nivel 5\n");
    printf("                                                            VOLVER\n");

    while(true){
        limpiarFlecha();
        opcion = opcion % 6;
        if(opcion < 0) opcion = 6 + opcion;
        ubicarFlecha(opcion);
        if(!cambiarOpcion(&opcion)) break;
    }

    switch (opcion)
    {
    case 0: //NIVEL 1 - SIGUIENTE
        prop->nivelJuego = 0;
        return;
    case 1: //NIVEL 2 - SIGUIENTE
        prop->nivelJuego = 1;
        return;
    case 2: //NIVEL 3 - SIGUIENTE
        prop->nivelJuego = 2;
        return;
    case 3: //NIVEL 4 - SIGUIENTE
        prop->nivelJuego = 3;
        return;
    case 4: //NIVEL 5 - SIGUIENTE
        prop->nivelJuego = 4;
        return;
    case 5: //VOLVER
        menuModo(prop);
        return;
    }
}

//Menú MODO CLÁSICO - MODO INFINITO
void menuModo(Propiedades* prop){
    short opcion = 0;
    system("cls");
    gotoxy(0, 0);
    dibujarLogo();
    gotoxy(0, 31);
    printf("                                                            MODO CLASICO\n");
    printf("                                                            MODO INFINITO\n");
    printf("                                                            VOLVER\n");

    while(true){
        limpiarFlecha();
        opcion = opcion % 3;
        if(opcion < 0) opcion = 3 + opcion;
        ubicarFlecha(opcion);
        if(!cambiarOpcion(&opcion)) break;
    }

    switch (opcion)
    {
    case 0: //MODO CLÁSICO - SIGUIENTE 
        prop->nivelJuego = -1;
        return;
    case 1: //MODO INFINITO - SIGUIENTE 
        menuNivel(prop);
        return;
    case 2: //VOLVER
        menuJugadores(prop);
        return;
    }  
}

//Menú para decidir el número de jugadores 1 o 2, si se escoge 1 se pedirá un nombre
void menuJugadores(Propiedades* prop){
    short opcion = 0;
    system("cls");
    gotoxy(0, 0);
    dibujarLogo();
    gotoxy(0, 31);
    printf("                                                            1 JUGADOR\n");
    printf("                                                            2 JUGADORES\n");
    printf("                                                            VOLVER\n");

    while(true){
        limpiarFlecha();
        opcion = opcion % 3;
        if(opcion < 0) opcion = 3 + opcion;
        ubicarFlecha(opcion);
        if(!cambiarOpcion(&opcion)) break;
    }

    switch (opcion)
    {
    case 0: //1 JUGADOR - SIGUIENTE
        prop->numJugadores = 1;
        recibirNombre(prop->nombreJugador);
        menuModo(prop);
        return;
    case 1: //2 JUGADORES - SIGUIENTE
        prop->numJugadores = 2;
        menuModo(prop);
        return;
    case 2: //VOLVER
        menuPrincipal(prop);
        return;
    }
}

//El primer menú que verá el usuario
void menuPrincipal(Propiedades* prop){
    short opcion = 0;
    while(true){
        system("cls");
        gotoxy(0, 0);
        dibujarLogo();
        gotoxy(0, 31);
        printf("                                                            JUGAR\n");
        printf("                                                            HIGHSCORE\n");
        printf("                                                            COMO JUGAR\n");
        printf("                                                            SALIR\n");

        while(true){
            limpiarFlecha();
            opcion = opcion % 4;
            if(opcion < 0) opcion = 4 + opcion;
            ubicarFlecha(opcion);
            if(!cambiarOpcion(&opcion)) break;
        }

        switch (opcion)
        {
        case 0: //SIGUIENTE
            menuJugadores(prop);
            return;
        case 1: //HIGHSCORE
            mostrarHighscore();
            break;
        case 2: //COMO JUGAR
            instrucciones();
            break;
        case 3: //SALIR
            prop->flag = true;
            return;
        } 
    }
}

//Función que será llamada por el main del archivo principal, retorna un buleano
//True para cerrar el programa y falso para continuar con el juego
bool menu(int* numJugadores , int* nivelJuego, char* nombreJugador){
    Propiedades* prop = (Propiedades*) malloc(sizeof(Propiedades));
    GetAllKeys();
    prop->flag = false;
    system("cls");
    system("color 0E");
    sndPlaySound( "musica//menu.wav" , SND_ASYNC | SND_LOOP);

    menuPrincipal( prop );
    *numJugadores = prop->numJugadores;
    *nivelJuego = prop->nivelJuego;
    strcpy(nombreJugador, prop->nombreJugador); 

    GetAllKeys();
    system("cls");
    system("color 0d");
    return prop->flag;
}
