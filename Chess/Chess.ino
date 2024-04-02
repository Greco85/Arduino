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

void setup() {
  Serial.begin(9600);
  imprimirTablero();
}

void loop() {
  if (Serial.available() > 0) {
    String movimiento = Serial.readStringUntil('\n');
    Serial.println("Movimiento recibido: " + movimiento); // Agregamos esta línea para depurar
    
    if (movimiento.length() == 4) {
      int origenX = movimiento.charAt(0) - '1';
      int origenY = movimiento.charAt(1) - '1';
      int destinoX = movimiento.charAt(2) - '1';
      int destinoY = movimiento.charAt(3) - '1';

      Serial.print("Origen X: ");
      Serial.println(origenX);
      Serial.print("Origen Y: ");
      Serial.println(origenY);
      Serial.print("Destino X: ");
      Serial.println(destinoX);
      Serial.print("Destino Y: ");
      Serial.println(destinoY);

      if (validarMovimiento(origenX, origenY, destinoX, destinoY)) {
        moverPieza(origenX, origenY, destinoX, destinoY);
        imprimirTablero();
        Serial.println("Movimiento válido.");
      } else {
        Serial.println("Movimiento inválido.");
      }
    }
  }
}

void imprimirTablero() {
  Serial.write(27);
  Serial.println("Tablero de Ajedrez\n");
  Serial.println("  1 2 3 4 5 6 7 8");
  for (int i = 0; i < 8; i++) {
    Serial.print(1 + i);
    Serial.print(" ");
    for (int j = 0; j < 8; j++) {
      Serial.print(tablero[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("\n");
}

bool validarMovimiento(int origenX, int origenY, int destinoX, int destinoY) {
  char pieza = tablero[origenY][origenX];
  char destinoPieza = tablero[destinoY][destinoX];

  // Verificar si el movimiento es dentro del tablero
  if (destinoX < 0 || destinoX >= 8 || destinoY < 0 || destinoY >= 8) {
    return false;
  }

  // Verificar si la casilla de destino contiene una pieza del mismo color
  if ((islower(pieza) && islower(destinoPieza)) || (isupper(pieza) && isupper(destinoPieza))) {
    return false;
  }

  // Si es un peón blanco
  if (pieza == 'P') {
    // Si está en su casilla inicial
    if (origenY == 6) {
      // Puede avanzar 1 o 2 casillas hacia adelante
      if ((destinoY == origenY - 1 || destinoY == origenY - 2) && destinoX == origenX && destinoPieza == '.') {
        return true;
      }
    } else {
      // Si no está en su casilla inicial, solo puede avanzar 1 casilla hacia adelante
      if (destinoY == origenY - 1 && destinoX == origenX && destinoPieza == '.') {
        return true;
      }
    }
    // Si está comiendo en diagonal
    if (abs(destinoX - origenX) == 1 && destinoY == origenY - 1 && islower(destinoPieza)) {
      return true;
    }
    // Si llega al final del tablero, permitir la coronación
    if (destinoY == 0) {
      seleccionarPiezaCoronacion();
      return true;
    }
  }

  // Si es un peón negro
  if (pieza == 'p') {
    // Si está en su casilla inicial
    if (origenY == 1) {
      // Puede avanzar 1 o 2 casillas hacia adelante
      if ((destinoY == origenY + 1 || destinoY == origenY + 2) && destinoX == origenX && destinoPieza == '.') {
        return true;
      }
    } else {
      // Si no está en su casilla inicial, solo puede avanzar 1 casilla hacia adelante
      if (destinoY == origenY + 1 && destinoX == origenX && destinoPieza == '.') {
        return true;
      }
    }
    // Si está comiendo en diagonal
    if (abs(destinoX - origenX) == 1 && destinoY == origenY + 1 && isupper(destinoPieza)) {
      return true;
    }
    // Si llega al final del tablero, permitir la coronación
    if (destinoY == 7) {
      seleccionarPiezaCoronacion();
      return true;
    }
  }
  return false; // Si no cumple ninguna condición, el movimiento es inválido
}



void moverPieza(int origenX, int origenY, int destinoX, int destinoY) {
  tablero[destinoY][destinoX] = tablero[origenY][origenX];
  tablero[origenY][origenX] = '.';
}

char seleccionarPiezaCoronacion() {
  Serial.println("Selecciona la pieza en la que deseas coronar el peón:");
  Serial.println("1. Dama (Q)");
  Serial.println("2. Torre (R)");
  Serial.println("3. Alfil (B)");
  Serial.println("4. Caballo (N)");

  while (true) {
    if (Serial.available() > 0) {
      char opcion = Serial.read();
      if (opcion == '1' || opcion == '2' || opcion == '3' || opcion == '4') {
        switch (opcion) {
          case '1':
            return 'Q';
          case '2':
            return 'R';
          case '3':
            return 'B';
          case '4':
            return 'N';
        }
      }
    }
  }
}





