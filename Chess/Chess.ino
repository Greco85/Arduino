char tablero[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', 'Q', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};


bool turnoBlanco = true;

void setup() {
  Serial.begin(9600);
  imprimirTablero();
}

void loop() {
  if (Serial.available() > 0) {
    String movimiento = Serial.readStringUntil('\n');
    Serial.println("Movimiento recibido: " + movimiento); // Depuración

    if (movimiento.length() == 4) {
      int filaOrigen = movimiento.charAt(0) - '1';
      int columnaOrigen = movimiento.charAt(1) - '1';
      int filaDestino = movimiento.charAt(2) - '1';
      int columnaDestino = movimiento.charAt(3) - '1';

      int filaRey, columnaRey;
      buscarCoordenadasReyDelTurno(filaRey, columnaRey); // Buscar las coordenadas del rey del turno

      Serial.print("Tu rey está en: (");
      Serial.print(filaRey + 1);
      Serial.print(", ");
      Serial.print(columnaRey + 1);
      Serial.println(")");

      if ((turnoBlanco && isupper(tablero[filaOrigen][columnaOrigen])) || (!turnoBlanco && islower(tablero[filaOrigen][columnaOrigen]))) {
          if (!enJaque(filaRey, columnaRey)) { // Si no está en jaque, revisar el movimiento
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
            // Está en jaque, verificar si el movimiento del rival lo quita del jaque
            if (quitaJaque(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
              moverPieza(filaOrigen, columnaOrigen, filaDestino, columnaDestino);
              Serial.println("Movimiento válido.");
              imprimirTablero();
              turnoBlanco = !turnoBlanco;
            } else {
              Serial.println("Estás en jaque, debes mover al rey a un lugar seguro o protegerlo.");
              imprimirTablero();
            }
          }
      } else {
        Serial.println((!turnoBlanco ? "Blanco" : "Negro") + String(" no puede mover esa pieza en este turno.")); //LEER BIEN
      }


    }
  }
}


bool quitaJaque(int filaOrigen, int columnaOrigen, int filaDestino, int columnaDestino) {
    // Guardamos el estado actual de las piezas involucradas
    char piezaOrigen = tablero[filaOrigen][columnaOrigen];
    char piezaDestino = tablero[filaDestino][columnaDestino];

    // Realizamos el movimiento temporalmente
    tablero[filaDestino][columnaDestino] = piezaOrigen;
    tablero[filaOrigen][columnaOrigen] = '.';

    // Buscamos las coordenadas del rey
    int filaRey, columnaRey;
    buscarCoordenadasReyDelTurno(filaRey, columnaRey);

    // Verificamos si el rey sigue en jaque después del movimiento
    bool sigueEnJaque = enJaque(filaRey, columnaRey);

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

    // Buscamos las coordenadas del rey del turno
    int filaRey, columnaRey;
    buscarCoordenadasReyDelTurno(filaRey, columnaRey);

    // Verificamos si mover la pieza expone al rey a un jaque
    bool exponeAlReyAJaque = enJaque(filaRey, columnaRey);

    // Deshacemos el movimiento temporal
    tablero[filaOrigen][columnaOrigen] = piezaOrigen;
    tablero[filaDestino][columnaDestino] = piezaDestino;

    // Si mover la pieza expone al rey a un jaque, entonces la pieza está clavada
    return exponeAlReyAJaque;
}

bool enJaque(int filaRey, int columnaRey) {
    char rey = (turnoBlanco) ? 'K' : 'k';

    // Recorremos el tablero para encontrar las piezas que pueden atacar al rey
    for (int fila = 0; fila < 8; fila++) {
        for (int columna = 0; columna < 8; columna++) {
            char pieza = tablero[fila][columna];
            if ((turnoBlanco && islower(pieza)) || (!turnoBlanco && isupper(pieza))) {
                // Si la pieza enemiga puede atacar al rey, retorna verdadero
                if (validarMovimiento(fila, columna, filaRey, columnaRey)) {
                    return true;
                }
            }
        }
    }
    return false; // El rey no está en jaque
}



void buscarCoordenadasReyDelTurno(int& filaRey, int& columnaRey) {
  char reyBuscado = (turnoBlanco) ? 'K' : 'k'; // Rey blanco si es turno de los blancos, rey negro si es turno de los negros
  
  // Recorrer el tablero para encontrar las coordenadas del rey
  for (int fila = 0; fila < 8; ++fila) {
    for (int columna = 0; columna < 8; ++columna) {
      if (tablero[fila][columna] == reyBuscado) {
        filaRey = fila;
        columnaRey = columna;
        return; // Se encontraron las coordenadas del rey, salir de la función
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

    if (estaPiezaClavada(filaOrigen, columnaOrigen, filaDestino, columnaDestino)) {
      Serial.print("La pieza esta clavada");
        return false;
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
        return (dx <= 1 && dy <= 1);
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
        if (filaDestino == 0 && destinoPieza != 'k'){
            return true;
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
        if (filaDestino == 7 && destinoPieza != 'K') {
            return true;
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
        // Si es una torre
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







