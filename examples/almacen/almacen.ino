#include "simplex.h"                // Simple actions library. Paco Cañada 2025

                                    // Define los nombres aquí
SET_NAME(CARRETILLA_X, 5)
SET_NAME(CARRETILLA_Y, 6)
SET_NAME(GRUA,         9)
SET_NAME(PULSADOR,    A4)
SET_NAME(LUZ,         A5)
SET_NAME(LED,          2)

void setup() {
                                    // Define los pines aquí
  PIN_OUT(LUZ)        
  PIN_OUT(LED)
  PIN_BUTTON(PULSADOR)
  PIN_SERVO(CARRETILLA_X)
  PIN_SERVO(CARRETILLA_Y)
  PIN_SERVO(GRUA)
  PIN_TIME(CARRETILLA_Y, 60)
  PIN_TIME(GRUA, 80)
                                    // Estado inicial
  RESET(LUZ)
  SET(LED)
  SERVO(CARRETILLA_X, 90)
  SERVO(CARRETILLA_Y, 90)
  SERVO(GRUA, 135)
}

void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  PRESSED(PULSADOR) {               // Cuando se pulse el botón
    PIN_MODE(LED, FLASH)            // parpadeo del LED para indicar ocupado
    SET(LUZ)                        // encender luz del almacen 
    WAIT(2000)
    SERVO(CARRETILLA_Y, 60)         // mover la carretilla
    WAIT_SERVO(CARRETILLA_Y)
    PIN_TIME(CARRETILLA_X, 60)
    SERVO(CARRETILLA_X, 10)
    WAIT_SERVO(CARRETILLA_X)
    WAIT(2000)                     
    SERVO(GRUA, 150)                // mover grua a recoger carga
    WAIT_SERVO(GRUA)
    WAIT(1500)
    SERVO(GRUA, 35)                 // mover carga al vagon
    WAIT_SERVO(GRUA)
    WAIT(2500)
    SERVO(GRUA, 135)                // mover grua a posicion inicial
    WAIT_SERVO(GRUA)
    WAIT(3000)                     
    PIN_TIME(CARRETILLA_X, 30)      // mover carretilla a posicion inicial
    SERVO(CARRETILLA_X, 90)
    SERVO(CARRETILLA_Y, 90)
    WAIT_SERVO(CARRETILLA_Y)
    RESET(LUZ)
    WAIT(5000)                      // esperar 5 segundos para otros espectadores
    PIN_MODE(LED, NORMAL)           // modo LED fijo
    SET(LED)                        // enciende LED para indicar listo
  }
}

