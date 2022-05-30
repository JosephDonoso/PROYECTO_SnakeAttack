#ifndef Juego_h
#define Juego_h

typedef struct Pixel Pixel;
//Tipo pixel, son los que se dibujan en pantalla

typedef struct Muro Muro;
//Tipo muro, 4 listas de pixeles que serán los bordes del mapa

typedef struct Boost Boost;
//Tipo boost, tiene un pixel atribuido y un estado

typedef struct Misil Misil;
//Tipo misil, tiene un pixel atribuido y una velocidad que va de 1 a 3

typedef struct Player Player;

typedef struct Game Game;
//Tipo game, lista de jugador, de muros, de boost y caracteristicas del juego

void juego(int , int );
//Función que despliega el juego

#endif /* Juego_h */