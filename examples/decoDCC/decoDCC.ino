#include "simplex.h"        // Librería de acciones simples de Paco Cañada 2025

                            // Define los nombres aquí
SET_NAME(ROJO_1, 3)                  // Nombres para los pines de la señal de tres focos
SET_NAME(VERDE_1, 4)                
SET_NAME(AMARILLO_1, 5)              
SET_NAME(VERDE_AV, 6)                // Nombres para los pines de la señal avanzada
SET_NAME(AMARILLO_AV, 7)             
SET_NAME(ROJO_2, 8)                  // Nombres para los pines de la señal de dos focos
SET_NAME(VERDE_2, 9) 
SET_NAME(MECANICA, 10)               // Nombre para el pin del servo de la señal mecánica


void setup() {
                            // Define los pines aquí
  PIN_EFFECT(ROJO_1)                 // pines de salida de luces con efecto
  PIN_EFFECT(VERDE_1)
  PIN_EFFECT(AMARILLO_1)
  PIN_EFFECT(VERDE_AV)
  PIN_EFFECT(AMARILLO_AV)
  PIN_EFFECT(ROJO_2)
  PIN_EFFECT(VERDE_2)
  PIN_MODE(ROJO_1, DIMMER)           // efecto de encendido y apagado lento
  PIN_MODE(VERDE_1, DIMMER)
  PIN_MODE(AMARILLO_1, DIMMER)
  PIN_MODE(VERDE_AV, DIMMER)
  PIN_MODE(AMARILLO_AV, DIMMER)
  PIN_MODE(ROJO_2, DIMMER)
  PIN_MODE(VERDE_2, DIMMER)
  PIN_SERVO(MECANICA)                // pin del servo de la señal mecánica
  PIN_DCC()                          // pin de entrada DCC
                            // Define el estado inicial aquí
  SET(ROJO_1)                          // señal principal y avanzada
  SET(AMARILLO_AV)
  SET(ROJO_2)                          // señal de dos focos
  SERVO(MECANICA, 45)                  // señal mecánica, ángulo para aspecto de parada
}


void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  DCC_ACC_RED(20) {                 // accesorio 20 ROJO: principal parada, avanzada anuncio parada
    SET(ROJO_1)
    RESET(VERDE_1)
    RESET(AMARILLO_1)
    RESET(VERDE_AV)
    SET(AMARILLO_AV)
  }
  DCC_ACC_GREEN(20) {               // accesorio 20 VERDE: principal vía libre, avanzada vía libre
    RESET(ROJO_1)
    SET(VERDE_1)
    RESET(AMARILLO_1)
    SET(VERDE_AV)
    RESET(AMARILLO_AV)
  }
  DCC_ACC_RED(21) {                 // accesorio 21 ROJO: principal anuncio parada, avanzada anuncio precaución
    RESET(ROJO_1)
    RESET(VERDE_1)
    SET(AMARILLO_1)
    SET(VERDE_AV)
    SET(AMARILLO_AV)
  }
  DCC_ACC_GREEN(21) {               // accesorio 21 VERDE: principal anuncio precaución, avanzada anuncio precaución
    RESET(ROJO_1)
    SET(VERDE_1)
    SET(AMARILLO_1)
    SET(VERDE_AV)
    SET(AMARILLO_AV)
  }
  DCC_ACC_RED(25) {                 // accesorio 25 ROJO: señal 2: parada
    SET(ROJO_2)
    RESET(VERDE_2)
  }
  DCC_ACC_GREEN(25) {               // accesorio 25 VERDE: señal 2: via libre
    RESET(ROJO_2)
    SET(VERDE_2)
  }
  DCC_ACC_RED(28) {                 // accesorio 28 ROJO: señal mecánica: parada
    SERVO(MECANICA, 45)
  }
  DCC_ACC_GREEN(28) {               // accesorio 28 VERDE: señal mecánica: vía libre
    SERVO(MECANICA, 135)
  }
}
