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


/*
   Wokwi diagram.json. https://wokwi.com/projects/new/arduino-uno
  {
  "version": 1,
  "author": "Anonymous maker",
  "editor": "wokwi",
  "parts": [
    { "type": "wokwi-arduino-uno", "id": "uno", "top": 0, "left": 0, "attrs": {} },
    {
      "type": "wokwi-led",
      "id": "led1",
      "top": -78,
      "left": 323.4,
      "rotate": 90,
      "attrs": { "color": "red" }
    },
    {
      "type": "wokwi-led",
      "id": "led2",
      "top": -68.4,
      "left": 381,
      "rotate": 90,
      "attrs": { "color": "red" }
    },
    {
      "type": "wokwi-led",
      "id": "led3",
      "top": -58.8,
      "left": 438.6,
      "rotate": 90,
      "attrs": { "color": "red" }
    },
    { "type": "wokwi-servo", "id": "servo1", "top": 238, "left": 220.8, "attrs": {} },
    {
      "type": "wokwi-pushbutton",
      "id": "btn1",
      "top": 198.2,
      "left": 374.4,
      "attrs": { "color": "green", "xray": "1" }
    }
  ],
  "connections": [
    [ "uno:13", "led1:A", "green", [ "v0" ] ],
    [ "uno:GND.1", "led1:C", "black", [ "v0" ] ],
    [ "uno:12", "led2:A", "green", [ "v0" ] ],
    [ "led2:C", "led1:C", "green", [ "h0", "v-28.4", "h-57.6" ] ],
    [ "uno:11", "led3:A", "green", [ "v-37.8", "h297.6" ] ],
    [ "led3:C", "led2:C", "green", [ "h0", "v-38", "h-57.6" ] ],
    [ "servo1:GND", "uno:GND.3", "black", [ "h0" ] ],
    [ "servo1:V+", "uno:5V", "green", [ "h0" ] ],
    [ "servo1:PWM", "uno:9", "green", [ "h-249.6", "v-307", "h192" ] ],
    [ "btn1:2.l", "uno:GND.3", "green", [ "h-192", "v-19" ] ],
    [ "btn1:1.l", "uno:A2", "green", [ "h0" ] ]
  ],
  "dependencies": {}
  }

*/
