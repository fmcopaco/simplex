#include "simplex.h"                // Librería de acciones simples de Paco Cañada 2025

SET_NAME(LED, 13)                   // Damos el nombre LED el valor 13

void setup() {
  PIN_OUT(LED)                      // Definimos el pin como una salida
  PIN_MODE(LED, FLASH)              // La salida será parpadeante
  PIN_TIME(LED, 1000)               // El tiempo de parpadeo será 1 segundo
  SET(LED)                          // activamos la salida
}

void loop() {
  RUN_SIMPLEX                       // Run Simplex must be at the begining
}
