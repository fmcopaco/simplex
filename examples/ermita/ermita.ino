#include "simplex.h"        // Librería de acciones simples de Paco Cañada 2025

                            // Define los nombres aquí
SET_NAME(CAMPANA, 6)                // Nombre para el pin del servo
SET_NAME(PULSADOR, 3)               // Nombre para el pin del pulsador
SET_NAME(VELA, 4)                   // Nombre para el pin del LED amarillo
SET_NAME(LUZ, 5)                    // Nombre para el pin del LED blanco
SET_NAME(SONIDO, 9)                 // Nombre para el pin del módulo de sonido
SET_NAME(LED, 8)                    // Nombre para el pin del LED verde público

void setup() {
                           // Define los pines aquí
  PIN_SERVO(CAMPANA)                // servo que mueve la campana
  PIN_TIME(CAMPANA, 20)             // velocidad del servo
  PIN_BUTTON(PULSADOR)              // pulsador del publico
  PIN_COIL(SONIDO)                  // salida impulso para modulo sonido
  PIN_TIME(SONIDO, 500)             // tiempo del pulso para modulo de sonido
  PIN_OUT(LED)                      // LED verde para el publico
  PIN_OUT(LUZ)                      // LED blanco luz ermita
  PIN_EFFECT(VELA)                  // LED amarillo vela
  PIN_MODE(VELA, CANDLE)            // Efecto de vela
                          // Define el estado inicial aquí
  RESET(SONIDO)                     // no activar sonido
  SET(LED)                          // enciende LED para indicar listo
  SERVO(CAMPANA, 80)                // campana colgando
}

void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  PRESSED(PULSADOR) {               // Cuando se pulse el botón
    PIN_MODE(LED, FLASH)            // hacer parpadear el LED para indicar ocupado
    SET(LUZ)                        // enciende luz de la ermita
    WAIT(2000)                      // espera al capellan
    SET(SONIDO)                     // activar el sonido
    REPEAT(6) {                     // repetir 6 veces
      SERVO(CAMPANA, 140)           // mover campana a un extremo
      WAIT_SERVO(CAMPANA)           // esperar a que llegue
      SERVO(CAMPANA, 20)            // mover campana al otro extremo
      WAIT_SERVO(CAMPANA)           // esperar a que llegue
    }
    SERVO(CAMPANA, 80)              // campana colgando
    SET(VELA)                       // enciende vela
    WAIT(5000)                      // espera 5 segundos
    RESET(VELA)                     // apaga vela
    RESET(LUZ)                      // apaga luz
    PIN_MODE(LED, NORMAL)           // modo LED fijo
    SET(LED)                        // enciende LED para indicar listo
  }
}
