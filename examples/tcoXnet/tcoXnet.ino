#include "simplex.h"          // Librería de acciones simples de Paco Cañada 2025

// Define los nombres aquí
SET_NAME(PULSADOR1, 2)
SET_NAME(PULSADOR2, 3)
SET_NAME(PULSADOR3, 4)
SET_NAME(PULSADOR4, 5)

SET_NAME(ROJO1, 6)
SET_NAME(VERDE1, 7)
SET_NAME(ROJO2, 8)
SET_NAME(VERDE2, 9)
SET_NAME(ROJO3, 10)
SET_NAME(VERDE3, 11)
SET_NAME(ROJO4, 12)
SET_NAME(VERDE4, 13)

                              // Define los estados aquí

                              // Define las variables aquí

                              // Define funciones adicionales aquí


void setup() {
                              // Define los pines aquí
  PIN_OUT(ROJO1)
  PIN_OUT(VERDE1)
  PIN_OUT(ROJO2)
  PIN_OUT(VERDE3)
  PIN_OUT(ROJO3)
  PIN_OUT(VERDE3)
  PIN_OUT(ROJO4)
  PIN_OUT(VERDE4)
  PIN_BUTTON(PULSADOR1)
  PIN_BUTTON(PULSADOR2)
  PIN_BUTTON(PULSADOR3)
  PIN_BUTTON(PULSADOR4)

  XNET_ADDR(5)                // Direccion del dispositivo en el bus Xpressnet

                              // Define el estado inicial aquí

}



void loop() {
  RUN_SIMPLEX                 // Run Simplex debe estar al principio

  PRESSED(PULSADOR1) {
    XNET_TOGGLE(1)            // Cambia desvio numero 1
    WAIT_RELEASE(PULSADOR1)
  }
  PRESSED(PULSADOR2) {
    XNET_TOGGLE(2)            // Cambia desvio numero 2
    WAIT_RELEASE(PULSADOR2)
  }
  PRESSED(PULSADOR3) {
    XNET_TOGGLE(3)            // Cambia desvio numero 3
    WAIT_RELEASE(PULSADOR3)
  }
  PRESSED(PULSADOR4) {
    XNET_TOGGLE(4)            // Cambia desvio numero 4
    WAIT_RELEASE(PULSADOR4)
  }

  XNET_CHANGED(1) {           // Modulo RS 1, corresponde a los desvíos 1 a 4
    XNET_ACTIVE(1, 1) {       // Modulo RS 1, entradas 1 y 2 corresponden al desvio numero 1
      SET(ROJO1)
      RESET(VERDE1)
    }
    XNET_ACTIVE(1, 2) {
      RESET(ROJO1)
      SET(VERDE1)
    }
    XNET_ACTIVE(1, 3) {       // Modulo RS 1, entradas 3 y 4 corresponden al desvio numero 2
      SET(ROJO2)
      RESET(VERDE2)
    }
    XNET_ACTIVE(1, 4) {
      RESET(ROJO2)
      SET(VERDE2)
    }
    XNET_ACTIVE(1, 5) {       // Modulo RS 1, entradas 5 y 6 corresponden al desvio numero 3
      SET(ROJO3)
      RESET(VERDE3)
    }
    XNET_ACTIVE(1, 6) {
      RESET(ROJO3)
      SET(VERDE3)
    }
    XNET_ACTIVE(1, 7) {       // Modulo RS 1, entradas 7 y 8 corresponden al desvio numero 4
      SET(ROJO4)
      RESET(VERDE4)
    }
    XNET_ACTIVE(1, 8) {
      RESET(ROJO4)
      SET(VERDE4)
    }
  }

}
