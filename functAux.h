#ifndef FunctAux_h
#define FunctAux_h

typedef struct Propiedades{
    int numJugadores;
    int nivelJuego;
    char nombreJugador[15];
    bool flag;
}Propiedades;

void ocultarCursor(void);

void gotoxy(int , int );

const char *get_csv_field (char * , int );

int randRange(int , int );

void colorRandom(void);

void dibujarLogo(void);

void alertaMaxPantalla(void);

void GetAllKeys(void);

int higher_than(void* , void* );

#endif /* FunctAux_h */