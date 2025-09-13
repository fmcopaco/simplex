#include "simplex.h"        // Librería de acciones simples de Paco Cañada 2025

                            // Define los nombres aquí
SET_NAME(BARRERA, 6)                // Nombre para el pin del servo
SET_NAME(IZQUIERDA_N, 3)            // Nombre para el pin del sensor izquierda vía norte
SET_NAME(DERECHA_N, 2)              // Nombre para el pin del sensor derecha vía norte
SET_NAME(IZQUIERDA_S, 3)            // Nombre para el pin del sensor izquierda vía sur
SET_NAME(DERECHA_S, 2)              // Nombre para el pin del sensor derecha vía sur
SET_NAME(LUZ1, 4)                   // Nombre para el pin del LED rojo
SET_NAME(LUZ2, 5)                   // Nombre para el pin del LED rojo

                            // Define los estados aquí
FSM_NAME(LIBRE_N)                   // paso a nivel libre vía Norte
FSM_NAME(ENTRA_DERECHA_N)           // tren entrando por la derecha
FSM_NAME(ENTRA_IZQUIERDA_N)         // tren entrando por la izquierda
FSM_NAME(SALE_DERECHA_N)            // tren saliendo por la derecha
FSM_NAME(SALE_IZQUIERDA_N)          // tren saliendo por la izquierda
FSM_NAME(LIBRE_S)                   // paso a nivel libre vía Sur
FSM_NAME(ENTRA_DERECHA_S)           // tren entrando por la derecha
FSM_NAME(ENTRA_IZQUIERDA_S)         // tren entrando por la izquierda
FSM_NAME(SALE_DERECHA_S)            // tren saliendo por la derecha
FSM_NAME(SALE_IZQUIERDA_S)          // tren saliendo por la izquierda
FSM_NAME(PAN_LIBRE)                 // Paso a nivel libre
FSM_NAME(TREN_DETECTADO)            // Tren detectado en paso a nivel

                            // Define las variables aquí
VAR_NAME(trenEnViaN)
VAR_NAME(trenEnViaS)
VAR_NAME(FSM_VIA_N)
VAR_NAME(FSM_VIA_S)
VAR_NAME(FSM_BARRERA)

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
  SET_VAR(FSM_VIA_N, LIBRE_N)
  SET_VAR(FSM_VIA_S, LIBRE_S)
  SET_VAR(FSM_BARRERA, PAN_LIBRE)
}


void loop() {
  RUN_SIMPLEX                       // Run Simplex debe estar al principio

  CALL(VIA_NORTE)
  CALL(VIA_SUR)
  CALL(PASO_A_NIVEL)
}


FUNCTION(VIA_NORTE) {               // Vía Norte
  EQUAL(FSM_VIA_N, LIBRE_N){     
    ACTIVE(DERECHA_N) {
      SET_VAR(trenEnViaN, 1)
      SET_VAR(FSM_VIA_N, ENTRA_DERECHA_N)         
    }
    ACTIVE(IZQUIERDA_N) {
      SET_VAR(trenEnViaN, 1)
      SET_VAR(FSM_VIA_N, ENTRA_IZQUIERDA_N)       
    }
  }

  EQUAL(FSM_VIA_N, ENTRA_DERECHA_N) {
    ACTIVE(IZQUIERDA_N) {
     SET_VAR(FSM_VIA_N, SALE_IZQUIERDA_N)
    }
  }

  EQUAL(FSM_VIA_N, ENTRA_IZQUIERDA_N) {
    ACTIVE(DERECHA_N) {
     SET_VAR(FSM_VIA_N, SALE_DERECHA_N)
    }
  }

  EQUAL(FSM_VIA_N, SALE_DERECHA_N) {
    FREE(DERECHA_N) {
     SET_VAR(trenEnViaN, 0)
     SET_VAR(FSM_VIA_N, LIBRE_N)                     
    }
  }


EQUAL(FSM_VIA_N, SALE_IZQUIERDA_N) {
    FREE(IZQUIERDA) {
     SET_VAR(trenEnViaN, 0)
     SET_VAR(FSM_VIA_N, LIBRE_N)                     
    }
  }

}


FUNCTION(VIA_SUR) {               // Vía Sur
  EQUAL(FSM_VIA_S, LIBRE_S){     
    ACTIVE(DERECHA_S) {
      SET_VAR(trenEnViaS, 1)
      SET_VAR(FSM_VIA_S, ENTRA_DERECHA_S)         
    }
    ACTIVE(IZQUIERDA_S) {
      SET_VAR(trenEnViaS, 1)
      SET_VAR(FSM_VIA_S, ENTRA_IZQUIERDA_S)       
    }
  }

EQUAL(FSM_VIA_S, ENTRA_DERECHA_S) {
    ACTIVE(IZQUIERDA_S) {
     SET_VAR(FSM_VIA_S, SALE_IZQUIERDA_S)
    }
  }

EQUAL(FSM_VIA_S, ENTRA_IZQUIERDA_S) {
    ACTIVE(DERECHA_S) {
     SET_VAR(FSM_VIA_S, SALE_DERECHA_S)
    }
  }

EQUAL(FSM_VIA_S, SALE_DERECHA_S) {
    FREE(DERECHA_S) {
     SET_VAR(trenEnViaS, 0)
     SET_VAR(FSM_VIA_S, LIBRE_S)                     
    }
  }

EQUAL(FSM_VIA_S, SALE_IZQUIERDA_S) {
    FREE(IZQUIERDA) {
     SET_VAR(trenEnViaS, 0)
     SET_VAR(FSM_VIA_S, LIBRE_S)                     
    }
  }


FUNCTION(PASO_A_NIVEL) {               // Paso a nivel
  EQUAL(FSM_BARRERA, PAN_LIBRE) {
    OR(trenEnViaN, trenEnViaS) {
      CALL(BajarBarrera)
      SET_VAR(FSM_BARRERA, TREN_DETECTADO)
    }
  }

  EQUAL(FSM_BARRERA, TREN_DETECTADO) {
    AND(NOT(trenEnViaN), NOT(trenEnViaS)) {
      CALL(SubirBarrera)
      SET_VAR(FSM_BARRERA, PAN_LIBRE)
    }
  }
}
