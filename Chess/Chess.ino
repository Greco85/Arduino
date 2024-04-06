
//DEMASIADO EMOCIONANTE TERMINAR ESTE MUGRERO, CREO Q FUE DEMASIADO RARO Y ME ROMPÍ LA CABEZA COMO MUCHAS VECES
// Igual falta pulirlo mucho para que quede mejor y más optimizado, pero ya quedó la teoría solo falta pasarlo a la pantalla je

#include <Arduino.h>

  char tablero[8][8] = {
    {'.', '.', 'k', 'r', '.', 'b', 'r', '.'},
    {'p', 'p', 'p', '.', 'q', 'p', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', 'p'},
    {'.', '.', 'P', 'n', '.', '.', 'p', 'P'},
    {'P', '.', 'Q', '.', 'B', '.', 'P', '.'},
    {'.', 'P', '.', 'N', '.', 'P', '.', '.'},
    {'R', '.', '.', '.', 'K', '.', '.', 'R'}
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
unsigned long tiempoBlanco = 600000 ; // EL TIEMPO EN MILISEGUNDOS
unsigned long tiempoNegro = 600000 ; 
unsigned long tiempoAnterior = 0;

void setup() {
  Serial.begin(9600);
  imprimirTablero();
  tiempoAnterior = millis();
}

void loop() {

      if (juegoGanado) {
      Serial.println("El juego ha terminado. ¿Desea reiniciar el sistema para jugar nuevamente? (Sí = 'S' o 's' /No = otro)");
      while (Serial.available() == 0) {}
      char respuesta = Serial.read();
      if (respuesta == 'S' || respuesta == 's') { 
          juegoGanado = false;
          turnoBlanco = true;
          reyBlancoMovido = false;
          torreBlancaDerechaMovida = false;
          torreBlancaIzquierdaMovida = false;
          reyNegroMovido = false;
          torreNegraDerechaMovida = false;
          torreNegraIzquierdaMovida = false;
          tiempoBlanco = 600000;
          tiempoNegro = 600000;
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


          Serial.println("Reiniciando el juego...");
          imprimirTablero();
          return;
      } else {
          Serial.println("El sistema no ha sido reiniciado. Hasta luego.");
          while (true) {}
      }
  }

    //Muestra este mensaje cuando se le acaba el tiempo a alguno de los jugadores
    if (tiempoBlanco <= 0 || tiempoNegro <= 0) {
        Serial.println("Tiempo agotado para el jugador " + String((tiempoBlanco <= 0 ? "Blanco" : "Negro")));
        imprimirTablero();
          juegoGanado = true;
    }

    // Mostrar el tiempo restante del jugador actual
    mostrarTiempo(turnoBlanco ? tiempoBlanco : tiempoNegro);

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

    //CUANDO PONER EL MOVIMIENTO QUE VAS A REALIZAR

    if (Serial.available() > 0) {
        String movimiento = Serial.readStringUntil('\n');
        Serial.println("Movimiento recibido: " + movimiento);

        if (movimiento.length() == 4) {
            int filaOrigen = movimiento.charAt(0) - '1';
            int columnaOrigen = movimiento.charAt(1) - '1';
            int filaDestino = movimiento.charAt(2) - '1';
            int columnaDestino = movimiento.charAt(3) - '1';

            tiempoActual = millis();
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
                            Serial.println("Movimiento válido.");
                            imprimirTablero();
                            turnoBlanco = !turnoBlanco;
                        } else {
                          Serial.println("Movimiento inválido.");
                          imprimirTablero();
                        }
                    } else {
                        Serial.println("Enroque realizado.");
                        imprimirTablero();
                        turnoBlanco = !turnoBlanco;
                    }
                } else { //si estas en jaque 
                    if (quitaJaque(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
                        if (validarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
                        moverPieza(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        Serial.println("Movimiento válido. Quita del Jaque");
                        imprimirTablero();
                        turnoBlanco = !turnoBlanco;
                        } else {
                          Serial.println("Movimiento inválido.");
                          Serial.println("Estás en jaque, debes mover al rey a un lugar seguro o protegerlo.");
                          imprimirTablero();
                        }
                    }
                }
            } else {
            Serial.println((!turnoBlanco ? "Blanco" : "Negro") + String(" no puede mover esa pieza en este turno O pusiste mal las coordenadas.")); //O OTRAS COSAS
        }
          if (esJaqueMate()) {
              Serial.println("Jaque mate. El jugador " + String((turnoBlanco ? "Negro" : "Blanco")) + " ha ganado la partida");
              imprimirTablero();
              juegoGanado = true;
              return;  
          }
        }  
    }
}

void mostrarTiempo(unsigned long tiempoRestante) {
    String jugador = turnoBlanco ? "Blanco" : "Negro";
    unsigned long minutos = tiempoRestante / 60000;
    unsigned long segundos = (tiempoRestante % 60000) / 1000;
    Serial.print("Tiempo restante del jugador ");
    Serial.print(jugador);
    Serial.print(": ");
    Serial.print(minutos);
    Serial.print(" min ");
    Serial.print(segundos);
    Serial.println(" seg");
    delay(1000);
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

    // Retornamos verdadero si el movimiento quita el jaque, falso si no
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
  
      buscarCoordenadasReyDelTurno();

    // Verificar si el rey está en jaque
    if (!enJaque()) {
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
                                    return false; // El rey puede ser salvado
                            }

                            // Deshacer el movimiento temporal
                            tablero[fila][columna] = piezaOrigen;
                            tablero[destinoFila][destinoColumna] = piezaDestino;
                        } else {
                          //EN ESTA FUNCIÓN (O OTRA) PUEDE Q HAYA UN PROBLEMA AL MOVER TODAS LAS PIEZAS PUEDE QUE MUEVA EL REY Y ESTE SE CAMBIA A TRUE Y NO SE PODRÍA ENROCAR
                          // MAÑANA LO ARREGLO PORQ  HOY TUVE SUFICIENTE SOLO TENGO ESE ERROR Y QUIZÁS UNA MALA ORGANIZACIÓN EN EL LOOP 
                          // porque hay veces donde no dice si es movimiento válido o no.
                        }
                    }
                }
            }
        }
    }

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

    imprimirTablero();

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
                    if (!casillaBajoAtaque(0, 1) && !casillaBajoAtaque(0, 2) && !casillaBajoAtaque(0, 3) && tablero[0][1] == '.' && tablero[0][2] == '.' && tablero[0][3] == '.') {
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
                    if (!casillaBajoAtaque(7, 5) && tablero[7][5] == '.' && tablero[7][6] == '.') {
                        enrocarMovimiento(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
                        return true;
                    }
                }
                else if (columnaDestino == 2 && !reyBlancoMovido && !torreBlancaIzquierdaMovida) {
                    if (!casillaBajoAtaque(7, 1) && !casillaBajoAtaque(7, 2) && !casillaBajoAtaque(7, 3) && tablero[7][1] == '.' && tablero[7][2] == '.' && tablero[7][3] == '.') {
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
            if (filaOrigen == 7 && columnaOrigen == 0) {
                torreBlancaIzquierdaMovida = true;
            } else if (filaOrigen == 7 && columnaOrigen == 7) {
                torreBlancaDerechaMovida = true;
            }
        }
        // Si estamos moviendo una torre negra, actualizamos las variables correspondientes
        else if (pieza == 'r') {
            if (filaOrigen == 0 && columnaOrigen == 0) {
                torreNegraIzquierdaMovida = true;
            } else if (filaOrigen == 0 && columnaOrigen == 7) {
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
              if (pieza == 'K') {
                Serial.println("rey blanco movido");
                  reyBlancoMovido = true;
              }
              else if (pieza == 'k') {
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

    // Si es un peón y llega al final del tablero
    if ((pieza == 'P' && destinoFila == 0) || (pieza == 'p' && destinoFila == 7)) {
        char color = (pieza == 'P') ? 'w' : 'b'; // Determinar el color del peón
        coronarPeon(destinoFila, destinoColumna, color);
        tablero[origenFila][origenColumna] = '.';
    } else if (pieza == 'N' || pieza == 'n') {
        // Si es un caballo, solo mover la pieza
        tablero[destinoFila][destinoColumna] = pieza;
        tablero[origenFila][origenColumna] = '.';
    } else if (pieza == 'B' || pieza == 'b') {
        // Si es un alfil
        tablero[destinoFila][destinoColumna] = pieza;
        tablero[origenFila][origenColumna] = '.';
    } else if (pieza == 'R' || pieza == 'r') {
        tablero[destinoFila][destinoColumna] = pieza;
        tablero[origenFila][origenColumna] = '.';
    } else if (pieza == 'Q' || pieza == 'q') {
        // Si es una dama
        tablero[destinoFila][destinoColumna] = pieza;
        tablero[origenFila][origenColumna] = '.';
    } else if (pieza == 'K' || pieza == 'k') {
        // Si es un rey
        tablero[destinoFila][destinoColumna] = pieza;
        tablero[origenFila][origenColumna] = '.';
    } else {
        tablero[destinoFila][destinoColumna] = tablero[origenFila][origenColumna];
        tablero[origenFila][origenColumna] = '.';
    }
}

char seleccionarPiezaCoronacion(char color) {
  Serial.println("Selecciona la pieza en la que deseas coronar el peón:");
  if (color == 'w') {
    Serial.println("1. Dama (Q)");
    Serial.println("2. Torre (R)");
    Serial.println("3. Alfil (B)");
    Serial.println("4. Caballo (N)");
  } else if (color == 'b') {
    Serial.println("1. Dama (q)");
    Serial.println("2. Torre (r)");
    Serial.println("3. Alfil (b)");
    Serial.println("4. Caballo (n)");
  }

  while (true) {
    if (Serial.available() > 0) {
      char opcion = Serial.read();
      if (opcion == '1' || opcion == '2' || opcion == '3' || opcion == '4') {
        switch (opcion) {
          case '1':
            return (color == 'w') ? 'Q' : 'q';
          case '2':
            return (color == 'w') ? 'R' : 'r';
          case '3':
            return (color == 'w') ? 'B' : 'b';
          case '4':
            return (color == 'w') ? 'N' : 'n';
        }
      }
    }
  }
}

void coronarPeon(int destinoFila, int destinoColumna, char color) {
    char nuevaPieza = seleccionarPiezaCoronacion(color);
    tablero[destinoFila][destinoColumna] = nuevaPieza;
}







