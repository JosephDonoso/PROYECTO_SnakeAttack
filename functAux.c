#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "functAux.h"

//Obtiene todas las teclas que se utilizan en el juego
void GetAllKeys(){
    GetAsyncKeyState(VK_UP);
    GetAsyncKeyState(73);
    GetAsyncKeyState(VK_DOWN);
    GetAsyncKeyState(75);
    GetAsyncKeyState(VK_RIGHT);
    GetAsyncKeyState(76);
    GetAsyncKeyState(VK_LEFT);
    GetAsyncKeyState(74);

    GetAsyncKeyState(87);
    GetAsyncKeyState(83);
    GetAsyncKeyState(68);
    GetAsyncKeyState(65);

    GetAsyncKeyState(VK_RETURN);
}

void alertaMaxPantalla(){
    printf("\n\n\n        PARA CONSEGUIR UNA MEJOR EXPERIENCIA DE JUEGO SE RECOMIENDA AL USUARIO MAXIMIZAR LA PANTALLA\n");
    printf("        ");
    system("pause");
    GetAllKeys();
}

void dibujarLogo(){
    printf("\n\n\n");
    printf("                           # # #                     # # #      # #         # # #               # # #                         \n");
    printf("                        # # # # #      # # # #      # # #      # # #      # # # #         # #  # # # #                        \n");
    printf("                     # # # # # # #     # # # #     # # #      # # # #     # # # #        # # #  # # #                         \n");
    printf("                   # # # # #           # # # #     # # #     # # # # #    # # # #       # # #  # # #                          \n");
    printf("                  # # # #               # # # #    # # #    # #   # # #    # # # #    # # #   # #                             \n");
    printf("                  # # #       # # #     # # # #    # # #   # #      # # #   # # # # # # # #  # #    # #                       \n");
    printf("                  # # # # # # # # # #   # # # # #  # # #  # #       # # # # #  # # # # # #   # # # # # #                      \n");
    printf("                   # # # # # # # # # #  # #   # # # # #  # # # # # # # # # #  # # # # # #     # # # # #                       \n");
    printf("                              # # # #   # #     # # # #  # # # # # # # # #    # # # # # #      # # # #                        \n");
    printf("                             # # # #  # # #      # # #  # # #       # # # #   # # # #  # # #    # # #   # #                   \n");
    printf("                     # # # # # # #   # # #             # # #         # # # #  # # # #    # # #   # # # # #                    \n");
    printf("                      # # # # #                       # # #                   # # # #      # # #  # # # #                     \n");
    printf("                       # # #                                                    # # #              # #                        \n");
    printf("                                                                                                                              \n");
    printf("              # #      # # # # # # # # #  # # # # # # # # #        # #               # # # # #       # # #                    \n");
    printf("             # # #     # # # # # # # # #  # # # # # # # # #       # # #            # # # # # # #   # # # #         # #        \n");
    printf("            # # # #    # # # # # # # # #  # # # # # # # # #      # # # #          # # # # # # # #  # # # #        # # #       \n");
    printf("           # # # # #   # #   # # #        # #   # # #           # # # # #        # # # #    # # #  # # # #       # # #        \n");
    printf("          # #   # # #        # # #              # # #          # #   # # #       # # #              # # # #    # # #          \n");
    printf("         # #      # # #      # # #              # # #         # #      # # #     # # #               # # # # # # # #          \n");
    printf("        # #       # # # # #  # # #              # # #        # #       # # # # # # # #                # # # # # # #           \n");
    printf("       # # # # # # # # # #   # # #              # # #       # # # # # # # # # #  # # #                # # # # # #             \n");
    printf("      # # # # # # # # #      # # #              # # #      # # # # # # # # #     # # # #    # # #      # # # # # # #          \n");
    printf("     # # #       # # # #     # # #              # # #     # # #       # # # #     # # # # # # # #      # # # #   # # #        \n");
    printf("    # # #         # # # #    # # #              # # #    # # #         # # # #     # # # # # # #       # # # #     # # #      \n");
    printf("   # # #                    # # # #            # # # #  # # #                        # # # # #         # # # #      # # #     \n");
    printf("                                                                                                         # # #                \n");
}

void ocultarCursor(){
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursInfo;
    cursInfo.dwSize = 2;
    cursInfo.bVisible = FALSE;

    SetConsoleCursorInfo( consola, &cursInfo);
}

//Dirige el cursor a una posición de la pantalla
void gotoxy(int x, int y){
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(consola, pos);
}

//Obtiene los elementos de una fila de un csv
const char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (100*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {
       ret[i-ini_i] = 0;
       return ret;
    }


    return NULL;
}

//Entrega un número random entr un rango inicial a uno final
int randRange(int inicial, int final){
    int rango = final - inicial;
    return (rand() % rango) + inicial;
}

//Cambia el color de los caracteres de la pantalla
void colorRandom(){
    int opcion = rand() % 10;

    switch (opcion)
    {
    case 0:
        system("color 01");
        break;
    case 1:
        system("color 02");
        break;
    case 2:
        system("color 03");
        break;
    case 3:
        system("color 04");
        break;
    case 4:
        system("color 05");
        break;
    case 5:
        system("color 06");
        break;
    case 6:
        system("color 07");
        break;
    case 7:
        system("color 08");
        break;
    case 8:
        system("color 09");
        break;
    case 9:
        system("color 0A");
        break;
    case 10:
        system("color 0B");
        break;
    case 11:
        system("color 0C");
        break;
    case 12:
        system("color 0E");
        break;
    case 13:
        system("color 0F");
        break;
    }
    /*
Colores 
0 = Negro
1= Azul
2= Verde
3= Aguamarina
4= Rojo
5= Púrpura
6= Amarillo
7= Blanco
8= Gris
9= Azul Claro
A= Verde Claro
B= Aguamarina Claro
C= Rojo Claro
D= Púrpura Claro
E= Amarillo Claro
F= Blanco Brillante
*/
}

//Función comparativa para ordenar el árbol binario de mayor a menor
int higher_than(void* key1, void* key2){
    double* k1=(double*) key1;
    double* k2=(double*) key2;
    if(*k1 >= *k2) return 1;
    return 0;
}