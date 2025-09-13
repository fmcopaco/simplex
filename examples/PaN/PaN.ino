#include "simplex.h"        // Librería de acciones simples de Paco Cañada 2025

                            // Define los nombres aquí
SET_NAME(BARRERA, 6)                // Nombre para el pin del servo
SET_NAME(IZQUIERDA, 3)              // Nombre para el pin del sensor izquierda
SET_NAME(DERECHA, 2)                // Nombre para el pin del sensor derecha
SET_NAME(LUZ1, 4)                   // Nombre para el pin del LED rojo
SET_NAME(LUZ2, 5)                   // Nombre para el pin del LED rojo

                            // Define los estados aquí
FSM_NAME(LIBRE)                     // paso a nivel libre
FSM_NAME(ENTRA_DERECHA)             // tren entrando por la derecha
FSM_NAME(ENTRA_IZQUIERDA)           // tren entrando por la izquierda
FSM_NAME(SALE_DERECHA)              // tren saliendo por la derecha
FSM_NAME(SALE_IZQUIERDA)            // tren saliendo por la izquierda

                            // Define funciones adicionales aquí
FUNCTION(BajarBarrera) {
  SET(LUZ1)
  SET(LUZ2)
  SERVO(BARRERA, 0)
}

FUNCTION(SubirBarrera) {
  RESET(LUZ1)
  RESET(LUZ2)
  SERVO(BARRERA, 90)
}


void setup() {
                            // Define los pines aquí
  PIN_SERVO(BARRERA)                // servo que mueve la campana
  PIN_TIME(BARRERA, 10)             // velocidad del servo
  PIN_EFFECT(LUZ1)                  // LED rojo señal
  PIN_MODE(LUZ1, INVERT)
  PIN_MODE(LUZ1, FLASH)
  PIN_TIME(LUZ1, 1000)
  PIN_EFFECT(LUZ2)                  // otro LED rojo señal
  PIN_MODE(LUZ2, FLASH)
  PIN_TIME(LUZ2, 1000)
  PIN_SENSOR(DERECHA)               // sensor derecha
  PIN_TIME(DERECHA, 2000)           // para sensor infrarrojo aumentar tiempo
  PIN_SENSOR(IZQUIERDA)             // sensor izquierda
  PIN_TIME(IZQUIERDA, 2000)         // para sensor infrarrojo aumentar tiempo

                            // Define el estado inicial aquí
  CALL(SubirBarrera)

  FSM_GO(LIBRE)                     // estado inicial
}


void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  FSM_STATE(LIBRE) {
    ACTIVE(DERECHA) {
      CALL(BajarBarrera)
      FSM_GO(ENTRA_DERECHA)                     
    }
    ACTIVE(IZQUIERDA) {
      CALL(BajarBarrera)
      FSM_GO(ENTRA_IZQUIERDA)                     
    }
  }

  FSM_STATE(ENTRA_DERECHA) {
    ACTIVE(IZQUIERDA) {
      FSM_GO(SALE_IZQUIERDA)                     
    }
  }

  FSM_STATE(ENTRA_IZQUIERDA) {
    ACTIVE(DERECHA) {
      FSM_GO(SALE_DERECHA)                     
    }
  }

  FSM_STATE(SALE_DERECHA) {
    FREE(DERECHA) {
      CALL(SubirBarrera)
      FSM_GO(LIBRE)                     
    }
  }

  FSM_STATE(SALE_IZQUIERDA) {
    FREE(IZQUIERDA) {
      CALL(SubirBarrera)
      FSM_GO(LIBRE)                     
    }
  }
}
