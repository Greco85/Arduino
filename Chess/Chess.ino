
//DEMASIADO EMOCIONANTE TERMINAR ESTE MUGRERO, CREO Q FUE DEMASIADO RARO Y ME ROMPÍ LA CABEZA COMO MUCHAS VECES
// Igual falta pulirlo mucho para que quede mejor y más optimizado.

//IGUAL FALTA PONER SI ES TABLAS, O SIMPLEMENTE NO HAY MATERIAL PARA CONTINUAR PERO ESO LUEGO LO POONGO

#include <Arduino.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;  
#include <TouchScreen.h>

char movimiento[4]; // Variable para almacenar el movimiento
bool esperandoSegundaSeleccion = false; // Variable de estado para determinar si estamos esperando la segunda selección
int primerFila;
int primerColumna;
int filamovimiento;
int columnamovimiento; 
TSPoint tp;  

bool solopaelbugcuriosodelenroque = false;
bool reiniciar = false;

// Definir las dimensiones del tablero de ajedrez
#define CHESSBOARD_SIZE 8
#define SQUARE_SIZE 40

// Definir los pines para la pantalla táctil
const int XP=6,XM=A2,YP=A1,YM=7; //ID=0x9341
const int TS_LEFT=905,TS_RT=136,TS_TOP=972,TS_BOT=145;

uint16_t ID;

// Crear un objeto para la pantalla táctil
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Definir la presión mínima para considerar un toque como válido
#define MINPRESSURE 100
#define MAXPRESSURE 1000

// Definir colores
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F 
#define GREEN 0x07E0
#define RED 0xF800 


char tablero[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };


bool juegoGanado = false;
bool turnoBlanco = true;
int filadelReyGlobal;
int columnadelReyGlobal;
bool reyBlancoMovido = false;
bool torreBlancaDerechaMovida = false;
bool torreBlancaIzquierdaMovida = false;
bool reyNegroMovido = false;
bool torreNegraDerechaMovida = false;
bool torreNegraIzquierdaMovida = false;
unsigned long tiempoBlanco = 600000*2 ; // EL TIEMPO EN MILISEGUNDOS
unsigned long tiempoNegro = 600000*2 ; 
unsigned long tiempoAnterior = 0;

char piezaSeleccionada = ' '; 


// las piezas
const unsigned char peon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 
  0x01, 0xff, 0x80, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x0f, 
  0xff, 0xf0, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x0f, 0xff, 
  0xf0, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x1f, 0xff, 0xf8, 
  0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 
  0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 
  0x01, 0xff, 0x80, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x01, 
  0xff, 0x80, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 
  0xf0, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xfe, 
  0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xff, 0xc0, 
  0x07, 0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x0f, 
  0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char peoncontorno[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x01, 0xc3, 0x80, 0x00, 0x00, 
  0x06, 0x00, 0x60, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x08, 0x00, 0x00, 0x10, 
  0x00, 0x08, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00, 
  0x08, 0x00, 0x00, 0x10, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x18, 0x00, 0x00, 0x60, 0x00, 0x06, 
  0x00, 0x00, 0x80, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 
  0x00, 0x3e, 0x00, 0x7c, 0x00, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 
  0x02, 0x00, 0x40, 0x00, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x02, 
  0x00, 0x40, 0x00, 0x00, 0x04, 0x00, 0x20, 0x00, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 
  0x08, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x01, 
  0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x00, 0x20, 
  0x08, 0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x08, 0x10, 
  0x00, 0x00, 0x00, 0x08, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char alfilcontorno[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x08, 0x00, 0x40, 0x00, 0x00, 
  0x08, 0x00, 0xc0, 0x00, 0x00, 0x0e, 0x03, 0x80, 0x00, 0x00, 0x03, 0x06, 0x00, 0x00, 0x00, 0x06, 
  0x0c, 0x00, 0x00, 0x00, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x18, 0x08, 0xc0, 0x00, 0x00, 0x30, 0x18, 
  0xe0, 0x00, 0x00, 0x20, 0x11, 0xb0, 0x00, 0x00, 0x60, 0x31, 0x18, 0x00, 0x00, 0x40, 0x23, 0x0c, 
  0x00, 0x00, 0xc0, 0x62, 0x06, 0x00, 0x00, 0x80, 0xc6, 0x03, 0x00, 0x01, 0x80, 0x84, 0x01, 0x80, 
  0x01, 0x00, 0x8c, 0x00, 0x80, 0x03, 0x00, 0x88, 0x00, 0xc0, 0x02, 0x01, 0x88, 0x00, 0x60, 0x02, 
  0x01, 0x18, 0x00, 0x60, 0x06, 0x01, 0x10, 0x00, 0x30, 0x04, 0x01, 0xf0, 0x00, 0x10, 0x04, 0x00, 
  0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 
  0x00, 0x10, 0x06, 0x00, 0x00, 0x00, 0x10, 0x02, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00, 0x00, 0x00, 
  0x60, 0x03, 0x00, 0x00, 0x00, 0x40, 0x01, 0x80, 0x00, 0x00, 0xc0, 0x00, 0x80, 0x00, 0x01, 0x80, 
  0x00, 0xc0, 0x00, 0x01, 0x00, 0x07, 0xc0, 0x00, 0x01, 0xe0, 0x08, 0x00, 0x00, 0x00, 0x10, 0x10, 
  0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x08, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char alfil[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 
  0x07, 0xff, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x01, 
  0xf0, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x0f, 0xe0, 
  0x00, 0x00, 0x00, 0x1f, 0xe0, 0x40, 0x00, 0x00, 0x1f, 0xc0, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0xf0, 
  0x00, 0x00, 0x3f, 0x81, 0xf8, 0x00, 0x00, 0x7f, 0x01, 0xfc, 0x00, 0x00, 0x7f, 0x03, 0xfe, 0x00, 
  0x00, 0xff, 0x03, 0xff, 0x00, 0x00, 0xff, 0x07, 0xff, 0x00, 0x01, 0xfe, 0x07, 0xff, 0x80, 0x01, 
  0xfe, 0x07, 0xff, 0x80, 0x01, 0xfe, 0x0f, 0xff, 0xc0, 0x03, 0xfe, 0x0f, 0xff, 0xe0, 0x03, 0xff, 
  0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 
  0xff, 0xe0, 0x01, 0xff, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xff, 
  0x80, 0x00, 0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0xfe, 0x00, 
  0x00, 0x3f, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xfe, 0x00, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x0f, 
  0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char caballo[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x1c, 0x78, 0x00, 0x00, 0x00, 0x3f, 
  0xfc, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x70, 0xfe, 0x00, 0x00, 0x00, 0xe3, 0xff, 
  0x00, 0x00, 0x00, 0xf7, 0xff, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x03, 0xff, 0xff, 0x80, 
  0x00, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 
  0x07, 0xff, 0xff, 0xf0, 0x00, 0x03, 0xcf, 0x8f, 0xf0, 0x00, 0x01, 0x80, 0x1f, 0xf0, 0x00, 0x00, 
  0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 
  0xff, 0xf8, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x00, 0x00, 0x0f, 0xff, 
  0xf8, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xf8, 
  0x00, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xe0, 0x00, 
  0x00, 0x3f, 0xff, 0xe0, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 
  0x3f, 0xff, 0x80, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char caballocontorno[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 
  0x00, 0xc4, 0x00, 0x00, 0x00, 0x3e, 0x84, 0x00, 0x00, 0x00, 0x63, 0x86, 0x00, 0x00, 0x00, 0xc0, 
  0x02, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x01, 0x8f, 0x01, 0x80, 0x00, 0x01, 0x1c, 0x00, 
  0x80, 0x00, 0x03, 0x08, 0x00, 0xc0, 0x00, 0x06, 0x00, 0x00, 0x40, 0x00, 0x04, 0x00, 0x00, 0x60, 
  0x00, 0x0c, 0x00, 0x00, 0x20, 0x00, 0x18, 0x00, 0x00, 0x30, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 
  0x18, 0x00, 0x00, 0x08, 0x00, 0x0c, 0x30, 0x70, 0x08, 0x00, 0x06, 0x7f, 0xe0, 0x0c, 0x00, 0x03, 
  0xc0, 0x20, 0x06, 0x00, 0x00, 0x00, 0xc0, 0x06, 0x00, 0x00, 0x01, 0x80, 0x06, 0x00, 0x00, 0x07, 
  0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x06, 0x00, 0x00, 0x10, 0x00, 
  0x06, 0x00, 0x00, 0x30, 0x00, 0x06, 0x00, 0x00, 0x20, 0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x04, 
  0x00, 0x00, 0xc0, 0x00, 0x0c, 0x00, 0x00, 0xc0, 0x00, 0x0c, 0x00, 0x00, 0xc0, 0x00, 0x1c, 0x00, 
  0x00, 0xc0, 0x00, 0x18, 0x00, 0x00, 0xc0, 0x00, 0x38, 0x00, 0x00, 0xc0, 0x00, 0x70, 0x00, 0x1f, 
  0xc0, 0x00, 0x7f, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x08, 0x1f, 0xff, 
  0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char torre[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06, 
  0x00, 0x60, 0x60, 0x06, 0x06, 0x00, 0x60, 0x60, 0x06, 0x06, 0x00, 0x60, 0x60, 0x07, 0xff, 0xff, 
  0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 
  0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 
  0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 
  0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 
  0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 
  0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 
  0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 
  0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x0f, 
  0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char torrecontorno[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0xf0, 0xf0, 0x09, 0x09, 
  0x00, 0x90, 0x90, 0x09, 0x09, 0x00, 0x90, 0x90, 0x09, 0xf9, 0xff, 0x9f, 0x90, 0x08, 0x00, 0x00, 
  0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x10, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x02, 0x00, 0x00, 0x00, 
  0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 
  0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 
  0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 
  0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 
  0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 
  0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 
  0x02, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x40, 0x1e, 0x00, 0x00, 0x00, 0x78, 0x10, 
  0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x08, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char dama[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0xc0, 0x00, 0x00, 0x3e, 
  0x03, 0xe0, 0x00, 0x00, 0x7f, 0x07, 0xf0, 0x00, 0x00, 0x7f, 0x07, 0xf0, 0x00, 0x00, 0x7f, 0x07, 
  0xf0, 0x00, 0x00, 0x3e, 0x03, 0xe0, 0x00, 0x00, 0x1c, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x02, 0x00, 
  0x00, 0x00, 0x0c, 0x03, 0x80, 0x00, 0x00, 0x0e, 0x07, 0x80, 0x00, 0x00, 0x0e, 0x07, 0x80, 0x00, 
  0x1c, 0x06, 0x07, 0x80, 0x70, 0x3e, 0x07, 0x07, 0x80, 0xf8, 0x3e, 0x07, 0x8f, 0x80, 0xf8, 0x3e, 
  0x07, 0x8f, 0x80, 0xf8, 0x1c, 0x07, 0xdf, 0xc0, 0x70, 0x03, 0x07, 0xff, 0xc0, 0x00, 0x07, 0x87, 
  0xff, 0xc1, 0xe0, 0x07, 0x87, 0xff, 0xcf, 0xe0, 0x03, 0xc7, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 
  0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xff, 0xfc, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x0f, 
  0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char damacontorno[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0xc0, 0x00, 0x00, 0x63, 0x06, 0x30, 0x00, 0x00, 0x41, 
  0x04, 0x10, 0x00, 0x00, 0x80, 0x88, 0x08, 0x00, 0x00, 0x80, 0x88, 0x08, 0x00, 0x00, 0x80, 0x88, 
  0x08, 0x00, 0x00, 0x41, 0x0c, 0x10, 0x00, 0x00, 0x62, 0x06, 0x20, 0x00, 0x00, 0x1e, 0x05, 0xc0, 
  0x00, 0x00, 0x13, 0x04, 0x40, 0x00, 0x00, 0x11, 0x08, 0x40, 0x00, 0x1c, 0x11, 0x08, 0x40, 0x70, 
  0x22, 0x19, 0x88, 0x40, 0x88, 0x41, 0x08, 0xd8, 0x41, 0x04, 0x41, 0x08, 0x50, 0x41, 0x04, 0x41, 
  0x08, 0x70, 0x61, 0x04, 0x23, 0x88, 0x20, 0x20, 0x88, 0x1c, 0xc8, 0x00, 0x23, 0xf8, 0x18, 0x48, 
  0x00, 0x3e, 0x10, 0x08, 0x68, 0x00, 0x30, 0x10, 0x0c, 0x38, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 
  0x00, 0x60, 0x06, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 0x00, 
  0x80, 0x01, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x80, 0x00, 0x03, 0x00, 
  0x1f, 0xff, 0xff, 0xff, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x08, 0x10, 
  0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x08, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char rey[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x38, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x38, 
  0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x07, 0xe0, 0x7c, 0x03, 0xc0, 0x0f, 0xfc, 0x7c, 0x0f, 
  0xf0, 0x0f, 0xfc, 0x7c, 0x3f, 0xf8, 0x0f, 0xfe, 0x7e, 0x7f, 0xf8, 0x1f, 0xff, 0x7e, 0xff, 0xf8, 
  0x1e, 0x01, 0xff, 0xc1, 0xf8, 0x1c, 0xfc, 0xfe, 0x1c, 0x78, 0x1d, 0xfe, 0x3c, 0x7f, 0x78, 0x1d, 
  0xff, 0xbd, 0xff, 0x78, 0x1d, 0xff, 0xbd, 0xff, 0x78, 0x1d, 0xff, 0xbd, 0xff, 0x78, 0x1d, 0xff, 
  0xbd, 0xfe, 0x78, 0x1e, 0xff, 0xbd, 0xfe, 0xf8, 0x0e, 0xff, 0xbd, 0xfe, 0xf0, 0x0e, 0x7f, 0xbd, 
  0xf9, 0xf0, 0x0f, 0x7f, 0xbd, 0xf3, 0xe0, 0x07, 0x1f, 0xbd, 0x87, 0xc0, 0x07, 0xc0, 0x3c, 0x7f, 
  0xc0, 0x03, 0xff, 0xff, 0xff, 0x80, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xff, 0xfc, 0x00, 
  0x00, 0x1f, 0xff, 0xfc, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0x0f, 
  0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char reycontorno[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 
  0x00, 0x44, 0x00, 0x00, 0x00, 0x03, 0xc7, 0x80, 0x00, 0x00, 0x02, 0x00, 0x80, 0x00, 0x00, 0x03, 
  0xc7, 0x80, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0xc6, 
  0x00, 0x00, 0x0f, 0xf0, 0x82, 0x07, 0xe0, 0x08, 0x1e, 0x82, 0x1c, 0x38, 0x10, 0x02, 0x82, 0x70, 
  0x0c, 0x10, 0x03, 0x83, 0xc0, 0x04, 0x30, 0x01, 0x81, 0x80, 0x04, 0x20, 0x00, 0x81, 0x00, 0x04, 
  0x21, 0xfe, 0x00, 0x3e, 0x04, 0x23, 0x03, 0x01, 0xe3, 0x84, 0x22, 0x01, 0xc3, 0x80, 0x84, 0x22, 
  0x00, 0x42, 0x00, 0x84, 0x22, 0x00, 0x42, 0x00, 0x84, 0x22, 0x00, 0x42, 0x00, 0x84, 0x22, 0x00, 
  0x42, 0x01, 0x84, 0x21, 0x00, 0x42, 0x01, 0x04, 0x31, 0x00, 0x42, 0x01, 0x08, 0x11, 0x80, 0x42, 
  0x06, 0x08, 0x10, 0x80, 0x42, 0x0c, 0x18, 0x08, 0xe0, 0x42, 0x78, 0x30, 0x08, 0x3f, 0xc3, 0x80, 
  0x20, 0x0c, 0x00, 0x00, 0x00, 0x60, 0x03, 0x00, 0x00, 0x00, 0xc0, 0x00, 0xe0, 0x00, 0x03, 0x00, 
  0x1f, 0xe0, 0x00, 0x03, 0xf8, 0x10, 0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x08, 0x10, 
  0x00, 0x00, 0x00, 0x08, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
    // uint16_t tmp;
    Serial.begin(9600);
    drawChessboard(); // ESE PARA VERLO EN LA PANTALLA
    // imprimirTablero(); //ESE ES PARA VERLO EN EL SERIAL
    tiempoAnterior = millis();
}


void loop() {

    if (juegoGanado) {
            pantallaVictoria("POR JAQUE MATE");
            reiniciar = false;
            drawChessboard();
    }

    //Muestra este mensaje cuando se le acaba el tiempo a alguno de los jugadores
    if (tiempoBlanco <= 0 || tiempoNegro <= 0) {
       // Serial.println("Tiempo agotado para el jugador " + String((tiempoBlanco <= 0 ? "Blanco" : "Negro")));
        // imprimirTablero();
          pantallaVictoria("POR TIEMPOOOO");
          reiniciar = false;
          drawChessboard();
    }


    // Actualiza el tiempo restante del jugador actual
    unsigned long tiempoActual = millis();
    if (tiempoActual - tiempoAnterior >= 1000) { // Restar un segundo cada 1000 milisegundos (1 segundo)
        if (turnoBlanco) {
            tiempoBlanco -= 1000;
        } else {
            tiempoNegro -= 1000;
        }
        tiempoAnterior = tiempoActual;
    }

    // Verificar el toque en la pantalla
    if (seDejodepresionar()) {
        tp = ts.getPoint(); 

        uint16_t xpos, ypos;  // Coordenadas de la pantalla

        // Calcular las coordenadas del área del tablero ajustadas por la posición del tablero en la pantalla
        int startX = (tft.width() - (SQUARE_SIZE * CHESSBOARD_SIZE)) / 2;
        int startY = (tft.height() - (SQUARE_SIZE * CHESSBOARD_SIZE)) / 2;
        int endX = startX + (SQUARE_SIZE * CHESSBOARD_SIZE);
        int endY = startY + (SQUARE_SIZE * CHESSBOARD_SIZE);

        // Si no estamos esperando la segunda selección y se detecta un toque válido
        if (!esperandoSegundaSeleccion && tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
            xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
            ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
            
            // Verificar si el toque está dentro del área del tablero
            if (xpos >= startX && xpos < endX && ypos >= startY && ypos < endY) {
                // Si el toque está dentro del área del tablero
                primerFila = ((ypos - startY) / SQUARE_SIZE) + 1; 
                primerColumna = ((xpos - startX) / SQUARE_SIZE) + 1;
                esperandoSegundaSeleccion = true;
                delay(100);
                drawChessboard();
            } else {
                Serial.println("Tocaste fuera del tablero");
            }
        }
        // Si estamos esperando la segunda selección y se detecta un toque válido
        else if (esperandoSegundaSeleccion && tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
            xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
            ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
            
            // Verificar si el toque está dentro del área del tablero
            if (xpos >= startX && xpos < endX && ypos >= startY && ypos < endY) {
                // Si el toque está dentro del área del tablero
                filamovimiento = ((ypos - startY) / SQUARE_SIZE)+  1;
                columnamovimiento = ((xpos - startX) / SQUARE_SIZE)  + 1;

            
                sprintf(movimiento, "%d%d%d%d", primerFila, primerColumna, filamovimiento, columnamovimiento);
                esperandoSegundaSeleccion = false;
                delay(1000);
                manejarMovimientoAjedrez(movimiento);
                
                
            } else {
                Serial.println("Tocaste fuera del tablero");
                primerFila = -1;
                primerColumna = -1;
                filamovimiento = -1; //CREOq puedo quitar eso y dejar solo el esperandos....
                columnamovimiento = -1;
                esperandoSegundaSeleccion = false;
            }
        }

    }
}

void reiniciarVariables() {
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    juegoGanado = false;
    turnoBlanco = true;
    reyBlancoMovido = false;
    torreBlancaDerechaMovida = false;
    torreBlancaIzquierdaMovida = false;
    reyNegroMovido = false;
    torreNegraDerechaMovida = false;
    torreNegraIzquierdaMovida = false;
    tiempoBlanco = 600000*2;
    tiempoNegro = 600000*2;
    tiempoAnterior = 0;

    char nuevoTablero[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    memcpy(tablero, nuevoTablero, sizeof(tablero));
}


void manejarMovimientoAjedrez(char* movimiento) {

    Serial.println("Movimiento recibido desde la pantalla táctil: " + String(movimiento));

    if (strlen(movimiento)== 4) {
        int filaOrigen = movimiento[0] - '1';
        int columnaOrigen = movimiento[1] - '1';
        int filaDestino = movimiento[2] - '1';
        int columnaDestino = movimiento[3] - '1';

        unsigned long tiempoActual = millis();
        if (tiempoActual - tiempoAnterior >= 1000) { // Restar un segundo cada 1000 milisegundos (1 segundo)
            if (turnoBlanco) {
                tiempoBlanco -= 1000;
            } else {
                tiempoNegro -= 1000;
            }
            tiempoAnterior = tiempoActual;
        }

        if ((turnoBlanco && isupper(tablero[filaOrigen][columnaOrigen])) || (!turnoBlanco && islower(tablero[filaOrigen][columnaOrigen]))) {
            if (!enJaque()) {
                if (!verificarEnroque(tablero[filaOrigen][columnaOrigen], filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
                    if (validarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
                        moverPieza(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        // Serial.println("Movimiento válido.");
                  // imprimirTablero();
                        turnoBlanco = !turnoBlanco;
                    } else {
                        // Serial.println("Movimiento inválido.");
                        pantallaErrorMovimiento("Movimiento invalido");

                      // imprimirTablero();
                    }
                } else {
                    Serial.println("Enroque realizado.");
                 // imprimirTablero();
                    turnoBlanco = !turnoBlanco;
                }
            } else { // Si estamos en jaque
                if (quitaJaque(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
                      if (validarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
                         moverPieza(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        // Serial.println("Movimiento válido. Quita del Jaque");
                        //  imprimirTablero();
                          turnoBlanco = !turnoBlanco;
                      } else {
                        //Serial.println("Movimiento inválido.");
                       // Serial.println("Estás en jaque, debes mover al rey a un lugar seguro o protegerlo.");
                        //    imprimirTablero();
                    }
                    
                } else pantallaErrorMovimiento("estas en jaque :p"); // ahi ta raro las llaves no se como van pero bno
                 
            }
        } else {
          pantallaErrorMovimiento("No es tu turno");
            //Serial.println((!turnoBlanco ? "Blanco" : "Negro") + String(" no puede mover esa pieza en este turno O pusiste mal las coordenadas.")); //O OTRAS COSAS
        }

        drawChessboard();
        
        if (esJaqueMate()) {
            Serial.println("Jaque mate. El jugador " + String((turnoBlanco ? "Negro" : "Blanco")) + " ha ganado la partida");
          // imprimirTablero();
            juegoGanado = true;
            return;  
        }

        
    }
}

bool seDejodepresionar() {
    tp = ts.getPoint();
    //Serial.println(tp.z);
    if (tp.z == 0) {
        return true;
    } else {
        return false;
    }
}

void drawChessboard() {

    tft.reset();
    ID = tft.readID();
    tft.begin(ID);

    int screenWidth = tft.width();
    int screenHeight = tft.height();
    int startX = (screenWidth - (SQUARE_SIZE * CHESSBOARD_SIZE)) / 2;
    int startY = (screenHeight - (SQUARE_SIZE * CHESSBOARD_SIZE)) / 2;

    for (int row = 0; row < CHESSBOARD_SIZE; row++) {
        for (int col = 0; col < CHESSBOARD_SIZE; col++) {
            int color = (row + col) % 2 == 0 ? WHITE : BLACK;
            int x = startX + col * SQUARE_SIZE;
            int y = startY + row * SQUARE_SIZE;

            if (row + 1 == primerFila && col + 1 == primerColumna) {
                color = BLUE;
            }

            tft.fillRect(x, y, SQUARE_SIZE, SQUARE_SIZE, color);
            
            char piece = tablero[row][col];

            if (piece != '.') {
                mostrarpieza(row, col, piece);
            }
        }
    }

    // Dibuja los cronómetros
    tft.setTextSize(4);
    tft.setTextColor(WHITE);

    // Cronómetro para el jugador negro (izquierda)
    int offsetX = 120; 
    int offsetY = 190;

    tft.setCursor(startX / 2 + offsetX, startY + (CHESSBOARD_SIZE * SQUARE_SIZE) / 2 + offsetY);
   
    unsigned long minutosNegro = tiempoNegro / 60000; 
    unsigned long segundosNegro = (tiempoNegro % 60000) / 1000; 
    unsigned long minutosBlanco = tiempoBlanco / 60000;
    unsigned long segundosBlanco = (tiempoBlanco % 60000) / 1000; 
    
    
    if (turnoBlanco) {
        tft.fillRect(0, startY + CHESSBOARD_SIZE * SQUARE_SIZE, screenWidth, screenHeight, GREEN);
    } 
    else {
        tft.fillRect(0, 0, screenWidth, startY, GREEN);
    }

    // Imprime los minutos y segundos
    tft.print(minutosBlanco);
    tft.print(":");
    tft.print(segundosBlanco);

    tft.setRotation(2); 
    tft.setCursor(startX + CHESSBOARD_SIZE * SQUARE_SIZE + startX / 2, startY + (CHESSBOARD_SIZE * SQUARE_SIZE) / 2 + offsetY - 20);
    tft.print(minutosNegro);
    tft.print(":");
    tft.print(segundosNegro);
    tft.setRotation(0);
}

void pantallaVictoria(const char* mensaje) {
    
    turnoBlanco = !turnoBlanco;
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    tft.fillScreen(TFT_BLACK);

    int anchoCaja = tft.width() - 40;
    int altoCaja = 100;
    int posXCaja = (tft.width() - anchoCaja) / 2;
    int posYCaja = 20; 

    tft.fillRect(posXCaja, posYCaja, anchoCaja, altoCaja, TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK);

    char ganador = turnoBlanco ? 'B' : 'N'; 
    int posXTexto, posYTexto; 
    uint16_t fillColor = TFT_WHITE;
    uint16_t outlineColor = TFT_BLACK;
    posYTexto = posYTexto = posYCaja + (altoCaja - 16) / 2;

    int longitudMensaje = strlen("EL REY BLANCO GANA"); // La longitud de los mensajes es la misma en ambos casos

    // Calcular la posición X para centrar el mensaje en la caja
    posXTexto = posXCaja + (anchoCaja - longitudMensaje * 6) / 2; // Multiplicar por 6 para ajustar el tamaño de la fuente

    // Imprimir el mensaje centrado horizontalmente
    tft.setCursor(posXTexto - 50, posYTexto);
    tft.print("EL REY ");
    if (ganador == 'B') {
        tft.print("BLANCO GANA");
    } else {
        tft.print("NEGRO GANA");
        fillColor = TFT_BLACK;
        outlineColor = TFT_WHITE;
    }
    
      int posYMensaje = posYCaja + altoCaja + 20;
      int posXMensaje = posXCaja - 40 + (anchoCaja - strlen(mensaje) * 6) / 2;
      int botonAncho = 127;
      int botonAlto = 45;
      int posYBotones = tft.height() - 60;
      int posXBotonReiniciar = (tft.width() - botonAncho) / 2; 
    

      tft.setCursor(posXMensaje, posYMensaje);
      tft.setTextSize(2);
      tft.setTextColor(TFT_WHITE);
      tft.print(mensaje);

      
      tft.setCursor(posXMensaje, posYMensaje + 150);
      tft.setTextSize(2);
      tft.setTextColor(TFT_WHITE);
      tft.print("Gracias");
        
    tft.fillRect(posXBotonReiniciar, posYBotones, botonAncho, botonAlto, TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setCursor(posXBotonReiniciar + 10, posYBotones + 10);
    tft.print("REINICIAR");

    Serial.println(posYMensaje);
    Serial.println(posYBotones);

    while (!reiniciar) {
        TSPoint reiniciartsp = ts.getPoint();

        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        int nuevax = map(reiniciartsp.x, TS_LEFT, TS_RT, 0, tft.width());
        int nuevay = map(reiniciartsp.y, TS_TOP, TS_BOT, 0, tft.height());

        Serial.println(reiniciartsp.z);

        if (reiniciartsp.z > 0) {
            if (nuevax > posXBotonReiniciar && nuevax < posXBotonReiniciar + botonAncho && nuevay > posYBotones && nuevay < posYBotones + botonAlto) {
              
              reiniciarVariables();
              reiniciar = true;
            }
        }
    }
}


void pantallaErrorMovimiento(const char* mensaje) {
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);
    tft.fillScreen(TFT_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);

    int longitudMensaje = strlen(mensaje);
    int anchoTexto = longitudMensaje * 12;

    int posXTexto = (tft.width() - anchoTexto) / 2;
    int posYTexto = tft.height() - 250;

    // Mostrar el mensaje
    tft.setCursor(posXTexto, posYTexto);
    tft.print(mensaje);
    delay(1000);
}


void mostrarpieza(int fila, int columna, char pieza) {
    int screenWidth = tft.width();
    int screenHeight = tft.height();
    int startX = (screenWidth - (SQUARE_SIZE * CHESSBOARD_SIZE)) / 2;
    int startY = (screenHeight - (SQUARE_SIZE * CHESSBOARD_SIZE)) / 2;

    // Calcular las coordenadas de la esquina superior izquierda del cuadro donde se debe mostrar la pieza
    int x = startX + columna * SQUARE_SIZE + (SQUARE_SIZE - 40) / 2;
    int y = startY + fila * SQUARE_SIZE + (SQUARE_SIZE - 40) / 2;

    // Definir colores para el contorno y el fondo
    uint16_t fillColor = WHITE;
    uint16_t outlineColor = BLACK;

    if (islower(pieza)) {
        fillColor = BLACK;
        outlineColor = WHITE;
    }

    switch (toupper(pieza)) { 
        case 'P':
            tft.drawBitmap(x, y, peoncontorno, 40, 40, outlineColor);
            tft.drawBitmap(x, y, peon, 40, 40, fillColor);
            break;
        case 'R':
            tft.drawBitmap(x, y, torrecontorno, 40, 40, outlineColor);
            tft.drawBitmap(x, y, torre, 40, 40, fillColor);
            break;
        case 'N':
            tft.drawBitmap(x, y, caballocontorno, 40, 40, outlineColor);
            tft.drawBitmap(x, y, caballo, 40, 40, fillColor);
            break;
        case 'B':
            tft.drawBitmap(x, y, alfilcontorno, 40, 40, outlineColor);
            tft.drawBitmap(x, y, alfil, 40, 40, fillColor);
            break;
        case 'Q':
            tft.drawBitmap(x, y, damacontorno, 40, 40, outlineColor);
            tft.drawBitmap(x, y, dama, 40, 40, fillColor);
            break;
        case 'K':
            tft.drawBitmap(x, y, reycontorno, 40, 40, outlineColor);
            tft.drawBitmap(x, y, rey, 40, 40, fillColor);
            break;
        default:
            break;
    }
}


bool quitaJaque(int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino) {
   
    // Guardamos el estado actual de las piezas involucradas
    char piezaOrigen = tablero[filaOrigen][columnaOrigen];
    char piezaDestino = tablero[filaDestino][columnaDestino];

    // Realizamos el movimiento temporalmente
    tablero[filaDestino][columnaDestino] = piezaOrigen;
    tablero[filaOrigen][columnaOrigen] = '.';

    // Verificamos si el rey sigue en jaque después del movimiento
    buscarCoordenadasReyDelTurno();
    int filaRey = filadelReyGlobal, columnaRey = columnadelReyGlobal;

    bool sigueEnJaque = verificarEnjaque(filaRey,columnaRey);

    // Deshacemos el movimiento temporal
    tablero[filaOrigen][columnaOrigen] = piezaOrigen;
    tablero[filaDestino][columnaDestino] = piezaDestino;
    
    return !sigueEnJaque;
}


bool estaPiezaClavada(int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino) {
    
    // Guardamos el estado actual de las piezas involucradas
    char piezaOrigen = tablero[filaOrigen][columnaOrigen];
    char piezaDestino = tablero[filaDestino][columnaDestino];

    // Realizamos el movimiento temporalmente
    tablero[filaDestino][columnaDestino] = piezaOrigen;
    tablero[filaOrigen][columnaOrigen] = '.';

    buscarCoordenadasReyDelTurno();
    int filaRey = filadelReyGlobal, columnaRey = columnadelReyGlobal; 

    // Verificamos si mover la pieza expone al rey a un jaque
    bool exponeAlReyAJaque = verificarEnjaque(filaRey,columnaRey); //este lo puedo unir con el otro pero primero veo q jale

    // Deshacemos el movimiento temporal
    tablero[filaOrigen][columnaOrigen] = piezaOrigen;
    tablero[filaDestino][columnaDestino] = piezaDestino;

    // Si mover la pieza expone al rey a un jaque, entonces la pieza está clavada
    return exponeAlReyAJaque;
}

bool esJaqueMate() {
  
  solopaelbugcuriosodelenroque = true;

      buscarCoordenadasReyDelTurno();

    // Verificar si el rey está en jaque
    if (!enJaque()) {
      solopaelbugcuriosodelenroque = false;
        return false; 
    }

    // Realizar movimientos temporales para verificar si ninguna pieza puede salvar al rey
    for (int fila = 0; fila < 8; fila++) {
        for (int columna = 0; columna < 8; columna++) {
            char pieza = tablero[fila][columna];
            if ((turnoBlanco && isupper(pieza)) || (!turnoBlanco && islower(pieza))) {
                for (int destinoFila = 0; destinoFila < 8; destinoFila++) {
                    for (int destinoColumna = 0; destinoColumna < 8; destinoColumna++) {
                        if (validarMovimiento(fila, columna, destinoFila, destinoColumna)) {
                            char piezaOrigen = tablero[fila][columna];
                            char piezaDestino = tablero[destinoFila][destinoColumna];

                            // Realizar el movimiento temporalmente
                            tablero[destinoFila][destinoColumna] = piezaOrigen;
                            tablero[fila][columna] = '.';

                            // Verificar si el rey sigue en jaque mate después del movimiento
                            if (!enJaque()) {
                                    // Deshacer el movimiento temporal
                                    tablero[fila][columna] = piezaOrigen;
                                    tablero[destinoFila][destinoColumna] = piezaDestino;
                                    solopaelbugcuriosodelenroque = false;
                                    return false; // El rey puede ser salvado
                            }

                            // Deshacer el movimiento temporal
                            tablero[fila][columna] = piezaOrigen;
                            tablero[destinoFila][destinoColumna] = piezaDestino;
                        } else {
                        }
                    }
                }
            }
        }
    }
    solopaelbugcuriosodelenroque = false; // ni idea si lo arregla pero ...
    return true;
}

bool enJaque() {

  // Recorremos el tablero para encontrar las piezas que pueden atacar al rey
  for (int fila = 0; fila < 8; fila++) {
      for (int columna = 0; columna < 8; columna++) {
          char pieza = tablero[fila][columna];
          if ((turnoBlanco && islower(pieza)) || (!turnoBlanco && isupper(pieza))) {
          if (pieza == '.') {
              continue; 
          }
          char rey = (turnoBlanco) ? 'K' : 'k';
              if (validarMovimiento(fila, columna, filadelReyGlobal, columnadelReyGlobal)) {
                  return true;
              }
          }
      }
  }
  
  return false; // El rey no está en jaque
}

//solo para darle valores pero lo puedo juntar con el de arriba solo q luego veo
bool verificarEnjaque(int filaRey, int columnaRey) {
    char rey = (turnoBlanco) ? 'K' : 'k';
    for (int fila = 0; fila < 8; fila++) {
        for (int columna = 0; columna < 8; columna++) {
            char pieza = tablero[fila][columna];
            if ((turnoBlanco && islower(pieza)) || (!turnoBlanco && isupper(pieza))) {
                if (validarMovimiento(fila, columna, filaRey, columnaRey)) {
                    return true;
                }
            }
        }
    }
    return false; 
}

void buscarCoordenadasReyDelTurno() {
    char reyBuscado = (turnoBlanco) ? 'K' : 'k'; // Rey blanco si es turno de los blancos, rey negro si es turno de los negros
    // Recorrer el tablero para encontrar las coordenadas del rey
    for (int fila = 0; fila < 8; ++fila) {
        for (int columna = 0; columna < 8; ++columna) {
            if (tablero[fila][columna] == reyBuscado) {
                filadelReyGlobal = fila;
                columnadelReyGlobal = columna;
                return;
            }
        }
    }
}

/* Creo que no lo voy a utilizar
void imprimirTablero() {
  Serial.write(27);
  Serial.println("Tablero de Ajedrez\n");
  Serial.println("  1 2 3 4 5 6 7 8");
  for (int fila = 0; fila < 8; fila++) {
    Serial.print(1 + fila);
    Serial.print(" ");
    for (int columna = 0; columna < 8; columna++) {
      Serial.print(tablero[fila][columna]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("\n");
}
*/

void enrocarMovimiento(int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino) {

    // Realizar el enroque corto del rey blanco
    if (filaOrigen == 7 && columnaOrigen == 4 && filaDestino == 7 && columnaDestino == 6) {
        tablero[7][5] = 'R'; 
        tablero[7][6] = 'K'; 
        tablero[7][4] = '.'; 
        tablero[7][7] = '.'; 
        reyBlancoMovido = true; 
        torreBlancaDerechaMovida = true;
    }

    // Realizar el enroque largo del rey blanco
    if (filaOrigen == 7 && columnaOrigen == 4 && filaDestino == 7 && columnaDestino == 2) {
        tablero[7][3] = 'R'; 
        tablero[7][2] = 'K'; 
        tablero[7][4] = '.'; 
        tablero[7][0] = '.'; 
        reyBlancoMovido = true; 
        torreBlancaIzquierdaMovida = true; 
    }

    // Realizar el enroque corto del rey negro
    if (filaOrigen == 0 && columnaOrigen == 4 && filaDestino == 0 && columnaDestino == 6) {
        tablero[0][5] = 'r'; 
        tablero[0][6] = 'k'; 
        tablero[0][4] = '.'; 
        tablero[0][7] = '.'; 
        reyNegroMovido = true; 
        torreNegraDerechaMovida = true; 
    }

    // Realizar el enroque largo del rey negro
    if (filaOrigen == 0 && columnaOrigen == 4 && filaDestino == 0 && columnaDestino == 2) {
        tablero[0][3] = 'r'; 
        tablero[0][2] = 'k'; 
        tablero[0][4] = '.'; 
        tablero[0][0] = '.'; 
        reyNegroMovido = true; 
        torreNegraIzquierdaMovida = true;
    }

    //imprimirTablero();

}

bool verificarEnroque(char pieza, int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino) {
    // Verificar si el rey está en jaque
    if (enJaque()) {
        return false; 
    }
    
    if (pieza == 'k') {
        int dx = abs(columnaDestino - columnaOrigen);
        int dy = abs(filaDestino - filaOrigen);

        if (dx == 2 && dy == 0) { 
            // Verificar condiciones específicas para el enroque del rey negro
            if (columnaOrigen == 4 && filaOrigen == 0) {
                if (columnaDestino == 6 && !reyNegroMovido && !torreNegraDerechaMovida) {
                    if (!casillaBajoAtaque(0, 5) && !casillaBajoAtaque(0, 6) && tablero[0][5] == '.' && tablero[0][6] == '.') {
                        enrocarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        return true;
                    }
                }
                else if (columnaDestino == 2 && !reyNegroMovido && !torreNegraIzquierdaMovida) {  
                    if (!casillaBajoAtaque(0, 2) && !casillaBajoAtaque(0, 3) && tablero[0][1] == '.' && tablero[0][2] == '.' && tablero[0][3] == '.') {
                        enrocarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        return true;
                    }
                }
            }
        }
    }

    else if (pieza == 'K') {
        int dx = abs(columnaDestino - columnaOrigen);
        int dy = abs(filaDestino - filaOrigen);

        if (dx == 2 && dy == 0) { 
            // Verificar condiciones específicas para el enroque del rey blanco
            if (columnaOrigen == 4 && filaOrigen == 7) {
                if (columnaDestino == 6 && !reyBlancoMovido && !torreBlancaDerechaMovida) {
                    if (!casillaBajoAtaque(7, 5) && !casillaBajoAtaque(7, 6) && tablero[7][5] == '.' && tablero[7][6] == '.') {
                        enrocarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        return true;
                    }
                }
                else if (columnaDestino == 2 && !reyBlancoMovido && !torreBlancaIzquierdaMovida) {
                    if (!casillaBajoAtaque(7, 2) && !casillaBajoAtaque(7, 3) && tablero[7][1] == '.' && tablero[7][2] == '.' && tablero[7][3] == '.') {
                        enrocarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        return true;
                    }
                }
            }
        }
    }

    return false; 
}

bool casillaBajoAtaque(int fila, int columna) {
    for (int filaTablero = 0; filaTablero < 8; filaTablero++) {
        for (int columnaTablero = 0; columnaTablero < 8; columnaTablero++) {
            char pieza = tablero[filaTablero][columnaTablero];
            if (pieza != '.') { 
                bool esEnemigo = (turnoBlanco && islower(pieza)) || (!turnoBlanco && isupper(pieza));
                if (esEnemigo) {
                    if (validarMovimiento(filaTablero, columnaTablero, fila, columna)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool validarMovimiento(int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino) {

    char pieza = tablero[filaOrigen][columnaOrigen];
    char destinoPieza = tablero[filaDestino][columnaDestino];

    // Verificar si el movimiento es dentro del tablero
    if (columnaDestino < 0 || columnaDestino >= 8 || filaDestino < 0 || filaDestino >= 8) {
        return false;
    }

    // Verificar si la casilla de destino contiene una pieza del mismo color
    if ((islower(pieza) && islower(destinoPieza)) || (isupper(pieza) && isupper(destinoPieza))) {
        return false;
    }

    //Verifica si la pieza que se quiere mover está clavada
    if ((!turnoBlanco && islower(pieza)) || (turnoBlanco && isupper(pieza))) {
        if (estaPiezaClavada(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
        return false;
      }
    }

    // Si es un caballo
    if (pieza == 'N' || pieza == 'n') {
        int dx = abs(columnaDestino - columnaOrigen);
        int dy = abs(filaDestino - filaOrigen);
        return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
    }

    // Si es un alfil
    if (pieza == 'B' || pieza == 'b') {
        int dx = columnaDestino - columnaOrigen;
        int dy = filaDestino - filaOrigen;

        // Verificar si el movimiento es en diagonal
        if (abs(dx) != abs(dy)) {
            return false; // Movimiento inválido
        }

        // Determinar el sentido del movimiento en base a los signos de dx y dy
        int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

        // Verificar si hay piezas en el camino
        int x = columnaOrigen + stepX;
        int y = filaOrigen + stepY;
        while (x != columnaDestino || y != filaDestino) {
            if (tablero[y][x] != '.') {
                return false; // Hay una pieza en el camino
            }
            x += stepX;
            y += stepY;
        }
        return true; // Movimiento válido
    }

    // Si es una torre
    if (pieza == 'R' || pieza == 'r') {
        int dx = columnaDestino - columnaOrigen;
        int dy = filaDestino - filaOrigen;

        // Verificar si el movimiento es horizontal o vertical
        if (dx != 0 && dy != 0) {
            return false; // La torre solo puede moverse en línea recta
        }

        // Determinar el sentido del movimiento en base a los signos de dx y dy
        int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
        int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

        // Verificar si hay piezas en el camino
        int x = columnaOrigen + stepX;
        int y = filaOrigen + stepY;
        while (x != columnaDestino || y != filaDestino) {
            if (tablero[y][x] != '.') {
                return false; // Hay una pieza en el camino
            }
            x += stepX;
            y += stepY;
        }

        // Si estamos moviendo una torre blanca, actualizamos las variables correspondientes
        if (pieza == 'R') {
            if (filaOrigen == 7 && columnaOrigen == 0 && !solopaelbugcuriosodelenroque) {
                torreBlancaIzquierdaMovida = true;
            } else if (filaOrigen == 7 && columnaOrigen == 7 && !solopaelbugcuriosodelenroque) {
                torreBlancaDerechaMovida = true;
            }
        }
        // Si estamos moviendo una torre negra, actualizamos las variables correspondientes
        else if (pieza == 'r') {
            if (filaOrigen == 0 && columnaOrigen == 0 && !solopaelbugcuriosodelenroque) {
                torreNegraIzquierdaMovida = true;
            } else if (filaOrigen == 0 && columnaOrigen == 7 && !solopaelbugcuriosodelenroque) {
                torreNegraDerechaMovida = true;
            }
        }
        return true; // El movimiento es válido
    }

    // Si es una reina
    if (pieza == 'Q' || pieza == 'q') {
        int dx = abs(columnaDestino - columnaOrigen);
        int dy = abs(filaDestino - filaOrigen);
        
        // Verificar si el movimiento es horizontal, vertical o diagonal
        if (!((dx != 0 && dy != 0) || (dx == 0 && dy != 0) || (dx != 0 && dy == 0))) {
            return false; // La reina solo puede moverse en línea recta o en diagonal
        }

        // Determinar el sentido del movimiento en base a los signos de dx y dy
        int stepX = (dx > 0) ? (columnaDestino - columnaOrigen) / dx : 0;
        int stepY = (dy > 0) ? (filaDestino - filaOrigen) / dy : 0;

        // Verificar si hay piezas en el camino
        int x = columnaOrigen + stepX;
        int y = filaOrigen + stepY;
        while (x != columnaDestino || y != filaDestino) {
            if (tablero[y][x] != '.') {
                return false; // Hay una pieza en el camino
            }
            x += stepX;
            y += stepY;
        }

        return true; // El movimiento es válido
    }

      // Si es un rey
      if (pieza == 'K' || pieza == 'k') {
          int dx = abs(columnaDestino - columnaOrigen);
          int dy = abs(filaDestino - filaOrigen);

          if (dx <= 1 && dy <= 1) {
              if (pieza == 'K' && !solopaelbugcuriosodelenroque) {
                Serial.println("rey blanco movido");
                  reyBlancoMovido = true;
              }
              else if (pieza == 'k' && !solopaelbugcuriosodelenroque) {
                Serial.println("rey negro movido");
                  reyNegroMovido = true;
              }
              return true;
          }

          return false;
      }

    // Si es un peón blanco
    if (pieza == 'P') {
        // Si está en su casilla inicial
        if (filaOrigen == 6) {
            // Puede avanzar 1 o 2 casillas hacia adelante
            if ((filaDestino == filaOrigen - 1 || filaDestino == filaOrigen - 2) && columnaDestino == columnaOrigen && destinoPieza == '.') {
                // Verificar si no hay piezas en el camino si se mueve 2 casillas hacia adelante
                if (filaDestino == filaOrigen - 2 && tablero[filaOrigen - 1][columnaOrigen] == '.' && destinoPieza == '.') {
                    return true;
                }
                // Verificar si no hay piezas en la casilla de destino
                if (filaDestino == filaOrigen - 1 && destinoPieza == '.') {
                    return true;
                }
            }
        } else {
            // Si no está en su casilla inicial, solo puede avanzar 1 casilla hacia adelante
            if (filaDestino == filaOrigen - 1 && columnaDestino == columnaOrigen && destinoPieza == '.') {
                return true;
            }
        }
        // Si está comiendo en diagonal
        if (abs(columnaDestino - columnaOrigen) == 1 && filaDestino == filaOrigen - 1 && islower(destinoPieza)) {
            return true;
        }
        // Si llega al final del tablero
        if (filaDestino == 0 && destinoPieza != 'k'){  //MUCHO MIEDO A ESTO PERO CREO QUE FUNCIONA ASÍ QUE LO DEJARE ASÍ
          if ((filaOrigen - filaDestino == 1 && columnaOrigen == columnaDestino && tablero[filaDestino][columnaDestino] == '.') || 
              (abs(columnaOrigen - columnaDestino) == 1 && filaOrigen - filaDestino == 1 && tablero[filaDestino][columnaDestino] != '.')) {
            return true;
           }
        }
    }

    // Si es un peón negro
    if (pieza == 'p') {
        // Si está en su casilla inicial
        if (filaOrigen == 1) {
            // Puede avanzar 1 o 2 casillas hacia adelante
            if ((filaDestino == filaOrigen + 1 || filaDestino == filaOrigen + 2) && columnaDestino == columnaOrigen && destinoPieza == '.') {
                // Verificar si no hay piezas en el camino si se mueve 2 casillas hacia adelante
                if (filaDestino == filaOrigen + 2 && tablero[filaOrigen + 1][columnaOrigen] == '.' && destinoPieza == '.') {
                    return true;
                }
                // Verificar si no hay piezas en la casilla de destino
                if (filaDestino == filaOrigen + 1 && destinoPieza == '.') {
                    return true;
                }
            }
        } else {
            // Si no está en su casilla inicial, solo puede avanzar 1 casilla hacia adelante
            if (filaDestino == filaOrigen + 1 && columnaDestino == columnaOrigen && destinoPieza == '.') {
                return true;
            }
        }
        // Si está comiendo en diagonal
        if (abs(columnaDestino - columnaOrigen) == 1 && filaDestino == filaOrigen + 1 && isupper(destinoPieza)) {
            return true;
        }
        // Si llega al final del tablero
        if (filaDestino == 7 && destinoPieza != 'K') { //MUCHO MIEDO A ESTO PERO CREO QUE FUNCIONA ASÍ QUE LO DEJARE ASÍ
          if ((filaOrigen - filaDestino == 1 && columnaOrigen == columnaDestino && tablero[filaDestino][columnaDestino] == '.') || 
              (abs(columnaOrigen - columnaDestino) == 1 && filaOrigen - filaDestino == 1 && tablero[filaDestino][columnaDestino] != '.')) {
              return true;
            }
      }
    }
        return false;
}

void moverPieza(int origenFila, int origenColumna, int destinoFila, int destinoColumna) {
    char pieza = tablero[origenFila][origenColumna];
    char destinoPieza = tablero[destinoFila][destinoColumna];

    tablero[origenFila][origenColumna] = '.';

    if ((pieza == 'P' && destinoFila == 0) || (pieza == 'p' && destinoFila == 7)) {
        char color = (pieza == 'P') ? 'w' : 'b';
        coronarPeon(destinoFila, destinoColumna, color);
    } else {
        tablero[destinoFila][destinoColumna] = pieza;
    }
}

char seleccionarPiezaCoronacion(char color) {
  tft.reset();
  ID = tft.readID();
  tft.begin(ID);
  char opcionSeleccionada = '\0';

  const char *textoDama = "Dama";
  const char *textoTorre = "Torre";
  const char *textoAlfil = "Alfil";
  const char *textoCaballo = "Caballo";

  tft.fillScreen(TFT_BLACK);

  // Muestra el rectángulo y las opciones en la pantalla
  int rectWidth = tft.width();
  int rectHeight = 140;
  int rectX = 0;
  int rectY = 0;
  tft.fillRect(rectX, rectY, rectWidth, rectHeight, TFT_WHITE);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(rectX + 10, rectY + 10);
  tft.println("Selecciona a que quieres");
  tft.println("    coronar el peon:");
  tft.setCursor(rectX + 20, rectY + 50);
  tft.println("1. " + String(textoDama));
  tft.setCursor(rectX + 20, rectY + 70);
  tft.println("2. " + String(textoTorre));
  tft.setCursor(rectX + 20, rectY + 90);
  tft.println("3. " + String(textoAlfil));
  tft.setCursor(rectX + 20, rectY + 110);
  tft.println("4. " + String(textoCaballo));


   for (int i = 0; i < 4; ++i) {
        int buttonX = rectX + 65 + (50 * i);
        int buttonY = rectY + 300;
        tft.fillRect(buttonX, buttonY, 40, 40, TFT_WHITE);
        tft.setTextColor(TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(buttonX + 12, buttonY + 10);
        tft.print(i + 1);
    }

    int piezaRectWidth = tft.width() - 40;
    int piezaRectHeight = tft.height() - rectHeight - 40;
    int piezaRectX = 20;
    int piezaRectY = rectHeight + 20;
    tft.drawRect(piezaRectX, piezaRectY, piezaRectWidth, piezaRectHeight, TFT_WHITE);
    tft.setTextSize(1);

  int gapX = (piezaRectWidth - (4 * piezaRectWidth / 5)) / 5; // Espacio entre las piezas en X
  int gapY = (piezaRectHeight - (2 * piezaRectHeight / 3)) / 3; // Espacio entre las piezas en Y
  int startX = piezaRectX + gapX;

  for (int i = 0; i < 4; ++i) {
      uint16_t fillColor = TFT_WHITE;
      uint16_t outlineColor = TFT_BLACK;

      if (color == 'b') {
          fillColor = TFT_BLACK;
          outlineColor = TFT_WHITE;
      }

      int x = startX + i * (piezaRectWidth / 5 + gapX);
      int y = piezaRectY + gapY;

      switch (i) {
          case 0:
              tft.drawBitmap(x, y, damacontorno, 40, 40, outlineColor);
              tft.drawBitmap(x, y, dama, 40, 40, fillColor);
              break;
          case 1:
              tft.drawBitmap(x, y, torrecontorno, 40, 40, outlineColor);
              tft.drawBitmap(x, y, torre, 40, 40, fillColor);
              break;
          case 2:
              tft.drawBitmap(x, y, alfilcontorno, 40, 40, outlineColor);
              tft.drawBitmap(x, y, alfil, 40, 40, fillColor);
              break;
          case 3: 
              tft.drawBitmap(x, y, caballocontorno, 40, 40, outlineColor);
              tft.drawBitmap(x, y, caballo, 40, 40, fillColor);
              break;
      }
  }

        // Bucle de selección
      
    while (opcionSeleccionada == '\0') {
        TSPoint touch = ts.getPoint();

        // Restaurar si no hay toque
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);

        int xpos = map(touch.x, TS_LEFT, TS_RT, 0, tft.width());
        int ypos = map(touch.y, TS_TOP, TS_BOT, 0, tft.height());

        if (touch.z > 0) {
            // Verificar si se toca un botón
            for (int i = 0; i < 4; ++i) {
                int buttonX = rectX + 65 + (50 * i);
                int buttonY = rectY + 300;
                if (xpos > buttonX && xpos < buttonX + 40 && ypos > buttonY && ypos < buttonY + 40) {
                    // Asignar la letra correspondiente según la opción seleccionada
                    char opcion = '1' + i;
                    if (opcion >= '1' && opcion <= '4') {
                        switch (opcion) {
                            case '1':
                                opcionSeleccionada = (color == 'w') ? 'Q' : 'q';
                                break;
                            case '2':
                                opcionSeleccionada = (color == 'w') ? 'R' : 'r';
                                break;
                            case '3':
                                opcionSeleccionada = (color == 'w') ? 'B' : 'b';
                                break;
                            case '4':
                                opcionSeleccionada = (color == 'w') ? 'N' : 'n';
                                break;
                        }
                    }
                }
            }
        }
    }


    return opcionSeleccionada;
}

void coronarPeon(int destinoFila, int destinoColumna, char color) {
    char nuevaPieza = seleccionarPiezaCoronacion(color);
    tablero[destinoFila][destinoColumna] = nuevaPieza;
}







