#include "simplex.h"        // Librería de acciones simples de Paco Cañada 2025

// Define los nombres aquí
SET_NAME(BARRERA, 6)                // Nombre para el pin del servo
SET_NAME(IZQUIERDA_N, 3)            // Nombre para el pin del sensor izquierda vía norte
SET_NAME(DERECHA_N, 2)              // Nombre para el pin del sensor derecha vía norte
SET_NAME(IZQUIERDA_S, 9)            // Nombre para el pin del sensor izquierda vía sur
SET_NAME(DERECHA_S, 8)              // Nombre para el pin del sensor derecha vía sur
SET_NAME(LUZ1, 4)                   // Nombre para el pin del LED rojo
SET_NAME(LUZ2, 5)                   // Nombre para el pin del LED rojo

                            // Define los estados aquí (multi FSM)
FSM_NEW(FSM_VIA_N)
FSM_NAME(LIBRE_N)                   // paso a nivel libre vía Norte
FSM_NAME(ENTRA_DERECHA_N)           // tren entrando por la derecha
FSM_NAME(ENTRA_IZQUIERDA_N)         // tren entrando por la izquierda
FSM_NAME(SALE_DERECHA_N)            // tren saliendo por la derecha
FSM_NAME(SALE_IZQUIERDA_N)          // tren saliendo por la izquierda

FSM_NEW(FSM_VIA_S)
FSM_NAME(LIBRE_S)                   // paso a nivel libre vía Sur
FSM_NAME(ENTRA_DERECHA_S)           // tren entrando por la derecha
FSM_NAME(ENTRA_IZQUIERDA_S)         // tren entrando por la izquierda
FSM_NAME(SALE_DERECHA_S)            // tren saliendo por la derecha
FSM_NAME(SALE_IZQUIERDA_S)          // tren saliendo por la izquierda

FSM_NEW(FSM_BARRERA)
FSM_NAME(PAN_LIBRE)                 // Paso a nivel libre
FSM_NAME(TREN_DETECTADO)            // Tren detectado en paso a nivel

                            // Define las variables aquí
VAR_NAME(trenEnViaN)
VAR_NAME(trenEnViaS)

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

FUNCTION(PASO_A_NIVEL) {               // Paso a nivel
  FSM_USE(FSM_BARRERA)
  FSM_STATE(PAN_LIBRE) {
    OR(trenEnViaN, trenEnViaS) {
      CALL(BajarBarrera)
      FSM_GO(TREN_DETECTADO)
    }
  }

  FSM_STATE(TREN_DETECTADO) {
    AND(NOT(trenEnViaN), NOT(trenEnViaS)) {
      CALL(SubirBarrera)
      FSM_GO(PAN_LIBRE)
    }
  }
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
  PIN_SENSOR(DERECHA_N)             // sensor derecha vía norte
  PIN_TIME(DERECHA_N, 2000)         // para sensor infrarrojo aumentar tiempo
  PIN_SENSOR(IZQUIERDA_N)           // sensor izquierda vía norte
  PIN_TIME(IZQUIERDA_N, 2000)       // para sensor infrarrojo aumentar tiempo
  PIN_SENSOR(DERECHA_S)             // sensor derecha vía sur
  PIN_TIME(DERECHA_S, 2000)         // para sensor infrarrojo aumentar tiempo
  PIN_SENSOR(IZQUIERDA_S)           // sensor izquierda vía sur
  PIN_TIME(IZQUIERDA_S, 2000)       // para sensor infrarrojo aumentar tiempo

                            // Define el estado inicial aquí
  CALL(SubirBarrera)
  SET_VAR(trenEnViaN, 0)
  SET_VAR(trenEnViaS, 0)

  FSM_USE(FSM_VIA_N)                // estados iniciales de cada FSM
  FSM_GO(LIBRE_N)
  FSM_USE(FSM_VIA_S)
  FSM_GO(LIBRE_S)
  FSM_USE(FSM_BARRERA)
  FSM_GO(PAN_LIBRE)
}


void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  CALL(VIA_NORTE)
  CALL(VIA_SUR)
  CALL(PASO_A_NIVEL)
}


FUNCTION(VIA_NORTE) {               // Vía Norte
  FSM_USE(FSM_VIA_N)
  FSM_STATE(LIBRE_N) {
    ACTIVE(DERECHA_N) {
      SET_VAR(trenEnViaN, 1)
      FSM_GO(ENTRA_DERECHA_N)
    }
    ACTIVE(IZQUIERDA_N) {
      SET_VAR(trenEnViaN, 1)
      FSM_GO(ENTRA_IZQUIERDA_N)
    }
  }

  FSM_STATE(ENTRA_DERECHA_N) {
    ACTIVE(IZQUIERDA_N) {
      FSM_GO(SALE_IZQUIERDA_N)
    }
  }

  FSM_STATE(ENTRA_IZQUIERDA_N) {
    ACTIVE(DERECHA_N) {
      FSM_GO(SALE_DERECHA_N)
    }
  }

  FSM_STATE(SALE_DERECHA_N) {
    FREE(DERECHA_N) {
      SET_VAR(trenEnViaN, 0)
      FSM_GO(LIBRE_N)
    }
  }


  FSM_STATE(SALE_IZQUIERDA_N) {
    FREE(IZQUIERDA_N) {
      SET_VAR(trenEnViaN, 0)
      FSM_GO(LIBRE_N)
    }
  }

}


FUNCTION(VIA_SUR) {               // Vía Sur
  FSM_USE(FSM_VIA_S)
  FSM_STATE(LIBRE_S) {
    ACTIVE(DERECHA_S) {
      SET_VAR(trenEnViaS, 1)
      FSM_GO(ENTRA_DERECHA_S)
    }
    ACTIVE(IZQUIERDA_S) {
      SET_VAR(trenEnViaS, 1)
      FSM_GO(ENTRA_IZQUIERDA_S)
    }
  }

  FSM_STATE(ENTRA_DERECHA_S) {
    ACTIVE(IZQUIERDA_S) {
      FSM_GO(SALE_IZQUIERDA_S)
    }
  }

  FSM_STATE(ENTRA_IZQUIERDA_S) {
    ACTIVE(DERECHA_S) {
      FSM_GO(SALE_DERECHA_S)
    }
  }

  FSM_STATE(SALE_DERECHA_S) {
    FREE(DERECHA_S) {
      SET_VAR(trenEnViaS, 0)
      FSM_GO(LIBRE_S)
    }
  }

  FSM_STATE(SALE_IZQUIERDA_S) {
    FREE(IZQUIERDA_S) {
      SET_VAR(trenEnViaS, 0)
      FSM_GO(LIBRE_S)
    }
  }
}
