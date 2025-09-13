#include "simplex.h"        // Librería de acciones simples de Paco Cañada 2025

                            // Define los nombres aquí
SET_NAME(PERRITO, 11)               // Nombre para el pin del servo
SET_NAME(PULSADOR, 10)              // Nombre para el pin del pulsador
SET_NAME(SONIDO, 9)                 // Nombre para el pin del módulo de sonido
SET_NAME(LED, 13)                   // Nombre para el pin del LED

void setup() {
                           // Define los pines aquí
  PIN_SERVO(PERRITO)                // servo que mueve el perrito
  PIN_TIME(PERRITO, 80)             // velocidad del servo
  PIN_BUTTON(PULSADOR)              // pulsador del publico
  PIN_COIL(SONIDO)                  // salida impulso para modulo sonido
  PIN_TIME(SONIDO, 500)             // tiempo del pulso para modulo de sonido
  PIN_OUT(LED)                      // LED para el publico
                           // Estado inicial
  RESET(SONIDO)                     // no activar sonido
  SET(LED)                          // enciende LED para indicar listo
  SERVO(PERRITO, 0)                 // mover el perrito a la caseta
}

void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  PRESSED(PULSADOR) {               // Cuando se pulse el botón
    PIN_MODE(LED, FLASH)            // parpadeo del LED para indicar ocupado
    SERVO(PERRITO, 100)             // mover el perrito fuera de la caseta
    WAIT_SERVO(PERRITO)             // esperar a que llegue
    SET(SONIDO)                     // activar el ladrido
    WAIT(6000)                      // esperar 6 segundos
    SERVO(PERRITO, 0)               // mover el perrito a la caseta
    WAIT_SERVO(PERRITO)             // esperar a que llegue
    WAIT(10000)                     // esperar 10 segundos para otros espectadores
    PIN_MODE(LED, NORMAL)           // modo LED fijo
    SET(LED)                        // enciende LED para indicar listo
  }
}
