/*    simplex.h -- F. Cañada 2025 --  https://usuaris.tinet.cat/fmco/
                                      https://github.com/fmcopaco/simplex
*/

#include "simplex.h"

pinData pinDef[_MAX_PINS];
delayData delayDef[_MAX_TIMERS];
Servo servo[_MAX_SERVO];                                                    // Objetos
uint8_t servoUsed = 0;
uint8_t currentStateFSM[_MAX_FSM + 1];
uint8_t nextStateFSM[_MAX_FSM + 1];
uint8_t countFSM = 0;
uint8_t countStateFSM = 0;
uint8_t currentFSM = 0;
uint32_t currentTime;
uint8_t repeatCurrent = 0;
uint16_t repeatVar[_MAX_REPEAT + 1];
uint8_t lastTime;
uint8_t pwmTimer;
uint8_t doEffect;
bool newMS;

#define MAX_BRIGHT 255


void processSimplexBasic();
void receiveSimplexDCC();
void processSimplexDCC();
void processSimplex();
void waitSimplex();
void waitTime(uint32_t timeout);
void waitServoStop(uint8_t num);


void setType(uint8_t num, pinType type) {
  pinDef[num].type = type;
  pinDef[num].active = false;
  pinDef[num].invert = false;
  pinDef[num].state = false;
  pinDef[num].flash = false;
  switch (type) {
    case _OUT:
      pinMode(num, OUTPUT);
      pinDef[num].timeout = _FLASH_TOUT;
      break;
    case _EFFECT:
      pinMode(num, OUTPUT);
      pinDef[num].index = NORMAL;
      pinDef[num].pos = 0;
      pinDef[num].pwm = 0;
      pinDef[num].timeout = _FLASH_TOUT;
      break;
    case _COIL:
      pinMode(num, OUTPUT);
      pinDef[num].timeout = _COIL_TOUT;
      break;
    case _SERVO:
      if (servoUsed < _MAX_SERVO) {
        pinDef[num].index = servoUsed;
        servoUsed++;
        pinDef[num].target = 255;                                           // not initialized
        pinDef[num].timer = millis();
        pinDef[num].timeout = _SPD_SERVO;
      }
      break;
    case _INPUT:
      pinMode(num, INPUT);
      break;
    case _BUTTON:
      pinMode(num, INPUT_PULLUP);
      pinDef[num].timer = millis();
      pinDef[num].timeout = _INPUT_TOUT;
      pinDef[num].active = digitalRead(num);
      break;
    case _SENSOR:
      pinMode(num, INPUT_PULLUP);
      pinDef[num].timer = millis();
      pinDef[num].timeout = _SENSOR_TOUT;
      pinDef[num].active = digitalRead(num);
      break;
    case _DCC:
      break;
    case _XNET:
      break;

  }
}


void setModePin(uint8_t num, modeType mode) {
  if (pinDef[num].type != _SERVO)
    pinDef[num].index = mode;                                               // For effects
  switch (mode) {
    case NORMAL:
      pinDef[num].invert = false;
      pinDef[num].flash = false;
      break;
    case INVERT:
      pinDef[num].invert = true;
      if ((pinDef[num].type == _BUTTON) || (pinDef[num].type == _SENSOR))   // no pull-up
        pinMode(num, INPUT);
      break;
    case FLASH:
      pinDef[num].flash = true;
      break;
  }
}


void setTimePin (uint8_t num, uint32_t timeout) {
  pinDef[num].timeout = timeout;
}


void setPin(uint8_t num, bool state) {
  pinDef[num].active = state;
  if (pinDef[num].invert)
    state = !state;
  switch (pinDef[num].type) {
    case _OUT:
      digitalWrite(num, state ? HIGH : LOW);
      pinDef[num].state = state;
      pinDef[num].timer = currentTime;
      break;
    case _EFFECT:
      if (pinDef[num].active)
        bitSet(pinDef[num].pos, 7);     // start effect
      else
        bitSet(pinDef[num].pos, 6);     // stop effect
      pinDef[num].timer = currentTime;
      break;
    case _COIL:
      digitalWrite(num, state ? HIGH : LOW);
      pinDef[num].timer = currentTime;
      break;
    case _SERVO:
      break;
  }
}


void setServoPos(uint8_t num, uint8_t pos) {
  if (pinDef[num].target == 255) {                                          // First time
    pinDef[num].pos = pos;
  }
  pinDef[num].target = pos;
  pinDef[num].active = true;
  servo[pinDef[num].index].attach(num);
  //servo[pinDef[num].index].write(pinDef[num].pos);
}

bool isServoStopped(uint8_t num) {
  return (pinDef[num].target == pinDef[num].pos);
}

bool isInputActive(uint8_t num) {
  return (pinDef[num].state);
}

void setTimer(uint8_t tmr, uint32_t timeout) {
  if (tmr < _MAX_TIMERS) {
    delayDef[tmr].active = true;
    delayDef[tmr].expired = false;
    delayDef[tmr].timeout = timeout;
    delayDef[tmr].timer = currentTime;
  }
}


bool isTimeout(uint8_t tmr) {
  return (delayDef[tmr].expired);
}

uint8_t getRandom() {
  static uint16_t rnd;
  if (rnd)
    rnd = (rnd >> 1) ^ (-(rnd & 1) & 0x9400);   //0x9400
  else
    rnd = (uint16_t)micros();
  return (uint8_t)rnd;
}

void processPins() {                                                        // every 1 ms
  uint8_t n;
  uint16_t i;
  pwmTimer += 16;
  doEffect = (doEffect + 1) & 0x07;
  for (n = 0; n < _MAX_PINS; n++) {
    if (pinDef[n].type != _UNDEF) {
      switch (pinDef[n].type) {
        case _OUT:
          if (pinDef[n].active && pinDef[n].flash) {                          // Flashing output
            if (currentTime - pinDef[n].timer > pinDef[n].timeout) {
              pinDef[n].timer = currentTime;
              pinDef[n].state = !pinDef[n].state;
              digitalWrite(n, pinDef[n].state ? HIGH : LOW);
            }
          }
          break;

        case _EFFECT:
          digitalWrite(n, (pinDef[n].pwm > pwmTimer) ? HIGH : LOW);           // every 1 ms
          if (doEffect == 0) {
            switch (pinDef[n].index) {                                        // every 8 ms
              case NORMAL:                                                    // for compatibility only, use PIN_OUT instead
              case INVERT:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pwm = pinDef[n].invert ? 0 : MAX_BRIGHT;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pwm = pinDef[n].invert ? MAX_BRIGHT : 0;
                pinDef[n].pos = 0x00;
                break;
              case FLASH:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pos = pinDef[n].invert ? 0x03 : 0x01;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pos = 0x05;
                switch (pinDef[n].pos) {
                  case 0x00:
                    pinDef[n].pwm = 0;
                    break;
                  case 0x01:
                    if (pinDef[n].pwm < 252)
                      pinDef[n].pwm += 4;
                    else
                      pinDef[n].pos = 0x04;
                    break;
                  case 0x02:
                  case 0x04:
                    if (currentTime - pinDef[n].timer > pinDef[n].timeout) {
                      pinDef[n].timer = currentTime;
                      pinDef[n].pos--;
                    }
                    break;
                  case 0x03:
                    if (pinDef[n].pwm < 4)
                      pinDef[n].pos = 0x02;
                    else
                      pinDef[n].pwm -= 4;
                    break;
                  case 0x05:
                    if (pinDef[n].pwm < 4)
                      pinDef[n].pos = 0x00;
                    else
                      pinDef[n].pwm -= 4;
                    break;
                }
                break;
              case DIMMER:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pos = 0x01;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pos = 0x02;
                switch (pinDef[n].pos) {
                  case 0x00:
                    pinDef[n].pwm = 0;
                    break;
                  case 0x01:
                    if (pinDef[n].pwm < 252)
                      pinDef[n].pwm += 4;
                    break;
                  case 0x02:
                    if (pinDef[n].pwm < 4)
                      pinDef[n].pos = 0x00;
                    else
                      pinDef[n].pwm -= 4;
                    break;
                }
                break;
              case FIRE:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pos = 0x10;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pos = 0x00;
                if (pinDef[n].pos > 0) {
                  if (pinDef[n].pos == 0x10) {
                    pinDef[n].pos = 0x19;                                     // prescaler
                    pinDef[n].pwm = pinDef[n].target;
                    i = (pinDef[n].target + getRandom()) / 2;                 // reduce dynamics
                    pinDef[n].target = (i > MAX_BRIGHT) ? MAX_BRIGHT : i;
                  }
                  else
                    pinDef[n].pos--;
                }
                else
                  pinDef[n].pwm = 0;
                break;
              case CANDLE:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pos = 0x10;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pos = 0x00;
                pinDef[n].pwm = bitRead(pinDef[n].pos, 4) ? getRandom() : 0;
                break;
              case WELDING:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pos = 0x01;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pos = 0x00;
                switch (pinDef[n].pos) {
                  case 0x00:
                    pinDef[n].pwm = 0;
                    break;
                  case 0x01:                                                  // weld spot
                    pinDef[n].pos++;
                    pinDef[n].counter = (getRandom() & 0x07) + 10;
                    pinDef[n].pwm = MAX_BRIGHT;
                    pinDef[n].target = 3;
                    break;
                  case 0x02:                                                  // welding
                    pinDef[n].target--;
                    if (pinDef[n].target == 0) {
                      pinDef[n].target = 2;
                      pinDef[n].counter--;
                      if (pinDef[n].counter > 0) {
                        pinDef[n].pwm ^= MAX_BRIGHT;
                      }
                      else {
                        pinDef[n].counter = getRandom() & 0x1F;
                        pinDef[n].counter |= (pinDef[n].counter & 0x03) ? 0x07 : 0x6F;
                        pinDef[n].pos++;
                        pinDef[n].target = 3;
                        pinDef[n].pwm = 0;
                      }
                    }
                    break;
                  case 0x03:                                                  // pause
                    pinDef[n].target--;
                    if (pinDef[n].target == 0) {
                      pinDef[n].target = 2;
                      pinDef[n].counter--;
                      if (pinDef[n].counter > 0) {
                        pinDef[n].pwm = 0;
                      }
                      else {
                        pinDef[n].pos = 0x02;
                        pinDef[n].counter = (getRandom() & 0x6F) + 12;
                        pinDef[n].pwm = MAX_BRIGHT;
                      }
                    }
                    break;
                }
                break;
              case FLUORESCENT:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pos = 0x01;
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pos = 0x00;
                switch (pinDef[n].pos) {
                  case 0x00:
                    pinDef[n].pwm = 0;
                    break;
                  case 0x01:
                    pinDef[n].pos++;
                    pinDef[n].counter = (getRandom() & 0x07) + 3;             // flicker times
                    pinDef[n].pwm = MAX_BRIGHT;
                    pinDef[n].target = 10;
                    break;
                  case 0x02:
                    pinDef[n].target--;
                    if (pinDef[n].target == 0) {
                      pinDef[n].counter--;
                      if (pinDef[n].counter > 0) {                            // flickering
                        pinDef[n].pwm ^= MAX_BRIGHT;
                        pinDef[n].target = (pinDef[n].pwm > 0) ? ((getRandom() & 0x07) + 6) : ((getRandom() & 0x1F) + 20);
                      }
                      else {
                        pinDef[n].pos++;                                      // light on
                        pinDef[n].pwm = MAX_BRIGHT;
                      }
                    }
                    break;
                }
                break;
              case PWM:
                if (bitRead(pinDef[n].pos, 7))                                // start
                  pinDef[n].pwm = (uint8_t)pinDef[n].timeout << 4;            // PWM: 0..15 (62.5Hz)
                if (bitRead(pinDef[n].pos, 6))                                // stop
                  pinDef[n].pwm = 0;
                pinDef[n].pos = 0x00;
                break;
            }
          }
          break;
        case _COIL:
          if (pinDef[n].active) {                                             // Coil deactivation
            if (currentTime - pinDef[n].timer > pinDef[n].timeout) {
              pinDef[n].timer = currentTime;
              pinDef[n].active = false;
              digitalWrite(n, pinDef[n].invert ? HIGH : LOW);
            }
          }
          break;
        case _SERVO:
          if (pinDef[n].active) {                                             // Servo moving
            if (currentTime - pinDef[n].timer > pinDef[n].timeout) {
              pinDef[n].timer = currentTime;
              if (pinDef[n].pos == pinDef[n].target) {
                servo[pinDef[n].index].detach();
                pinDef[n].active = false;
              }
              else {
                if (pinDef[n].target > pinDef[n].pos)
                  pinDef[n].pos++;
                else
                  pinDef[n].pos--;
                servo[pinDef[n].index].write(pinDef[n].pos);
              }
            }
          }
          break;
        case _INPUT:
          pinDef[n].state = digitalRead(n);
          if (pinDef[n].invert)
            pinDef[n].state = !pinDef[n].state;
          break;
        case _BUTTON:
          pinDef[n].flash = digitalRead(n);
          if (pinDef[n].active != pinDef[n].flash) {
            if (currentTime - pinDef[n].timer > pinDef[n].timeout) {
              pinDef[n].timer = currentTime;
              pinDef[n].active = pinDef[n].flash;
              pinDef[n].state = (pinDef[n].flash == LOW);
              if (pinDef[n].invert)
                pinDef[n].state = !pinDef[n].state;
            }
          }
          break;
        case _SENSOR:
          pinDef[n].flash = digitalRead(n);
          if (pinDef[n].flash == pinDef[n].invert) {
            pinDef[n].state = true;
            pinDef[n].timer = currentTime;
          }
          else {
            if (currentTime - pinDef[n].timer > pinDef[n].timeout) {
              pinDef[n].state = false;
              pinDef[n].timer = currentTime;
            }
          }
          break;
      }
    }
  }
}


void processTimers () {
  uint8_t n;
  if (newMS) {
    for (n = 0; n < _MAX_TIMERS; n++) {
      delayDef[n].expired = false;
      if (delayDef[n].active) {
        if (currentTime - delayDef[n].timer > delayDef[n].timeout) {
          delayDef[n].active = false;
          delayDef[n].expired = true;
        }
      }
    }
  }
}


void newRepeat() {
  repeatCurrent++;
  repeatVar[repeatCurrent] = 0;
}

bool repeatSimplex(uint16_t num) {
  if (repeatVar[repeatCurrent] < num) {
    repeatVar[repeatCurrent]++;
    return true;
  }
  else {
    repeatCurrent--;
    return false;
  }
}

uint8_t newFSM() {
  countFSM += (countFSM >= _MAX_FSM) ? 0 : 1;
  return countFSM;
}

uint8_t useFSM(uint8_t num) {
  currentFSM = num;
}

uint8_t newState() {
  countStateFSM++;
  return countStateFSM;
}


void setStateFSM(uint8_t state) {
  nextStateFSM[currentFSM] = state;
}

uint8_t getStateFSM() {
  return currentStateFSM[currentFSM];
}


#if (USE_DCC == true)

bool dccBit;
volatile bool dcc4Bytes, newPacket, prog2x, resetFlag ;
volatile byte dccData1, dccData2, dccData3, dccData4;
byte dccState, preambleCount, dccBytes, dccShift;
uint16_t accAddr;
uint8_t  accOutput;
volatile uint8_t *dccPinPort;
volatile uint8_t dccPinMask;
unsigned int dccEdge;


void dccISR() {
  if (*dccPinPort & dccPinMask) {   // because digitalRead at AVR is slow (~6us), we will read the dcc input in the ISR by direct port access (~125ns).
    dccBit =  ((unsigned int)micros() - dccEdge > 77) ? false : true;       // 77us: between 64us (one) and 90us (zero)
    switch (dccState) {
      case 0:                                                               // Preamble
        if (! newPacket) {                                                  // last packet decoded?
          preambleCount = dccBit ? preambleCount + 1 : 0;
          if (preambleCount > 9)
            dccState++;
        }
        break;
      case 1:                                                               // WaitLow
        if (! dccBit) {
          dccState++;
          dccBytes = 0;
          preambleCount = 0;
          dccData4 = 0;
        }
        break;
      case 9:                                                               // ReadLastBit
        dccShift <<= 1;
        if (dccBit)
          dccShift |= 0x01;
        dccBytes++;
        dccState += dccBytes;
        break;
      case 10:                                                              // End Byte 1
        dccState = dccBit ? 0 : 2;                                          // invalid packet if end bit is 1
        dccData1 = dccShift;
        break;
      case 11:                                                              // End Byte 2
        dccState = dccBit ? 0 : 2;                                          // invalid packet if end bit is 1
        dccData2 = dccShift;
        break;
      case 12:                                                              // End Byte 3
        dccData3 = dccShift;
        if (dccBit) {                                                       // valid packet if bit is 1
          dccState = 0;
          dcc4Bytes = false;
          newPacket = true;
        }
        else {                                                              // read other dcc byte if bit is 0
          dccState = 2;
          dcc4Bytes = true;
        }
        break;
      case 13:                                                              // End Byte 4
        dccData4 = dccShift;
        dccState = 0;
        if (dccBit)                                                         // valid packet if bit is 1
          newPacket = true;
        break;
      default:                                                              // ReadBit
        dccShift <<= 1;
        if (dccBit)
          dccShift |= 0x01;
        dccState++;
        break;
    }
  }
  else
    dccEdge = (unsigned int)micros();
}


void simplexDCC () {
  pinDef[_DCC_PIN].type = _DCC;
  dccState = 0;
  preambleCount = 0;
  newPacket = false;
  accAddr = 0;
  dccPinPort = portInputRegister( digitalPinToPort(_DCC_PIN));
  dccPinMask = digitalPinToBitMask(_DCC_PIN);
  pinMode(_DCC_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_DCC_PIN), dccISR, CHANGE);
}


void dccDecode() {
  uint16_t boardAddr;
  if ((dccData1 ^ dccData2 ^ dccData3 ^ dccData4) == 0) {                   // valid packet?
    if ((dccData1 & 0xC0) == 0x80) {                                        // '10aaaaaa' accessory packet
      if ((dccData2 & 0x88) == 0x88) {                                      // '1AAA1CCx' only activate outputs
        boardAddr = ((((dccData2 ^ 0x70) & 0x70) << 2) | (dccData1 & 0x3F));// 'AAAaaaaaa'
        accAddr = (((boardAddr - 1) << 2)  | ((dccData2 >> 1) & 0x03)) + 1; // 'AAAaaaaaaCC'  1..2047
        accOutput = dccData2 & 0x01;
      }
    }
  }
  newPacket = false;
}


bool isAccessoryDCC(uint16_t adr, uint8_t state) {
  return ((adr == accAddr) && (state == accOutput));
}

#endif


#if (USE_XNET == true)
#define WAIT_FOR_XMIT_COMPLETE {while (!(UCSR0A & (1<<TXC0))); UCSR0A = (1<<TXC0); UCSR0A = 0;}

enum xAnswer {HEADER, DATA1, DATA2, DATA3, DATA4, DATA5};

volatile byte rxBufferXN[20];                                               // Comunicacion Xpressnet
volatile byte txBuffer[14];
volatile byte txBytes;
volatile byte rxBytes;
volatile byte miCallByte;
volatile bool enviaMensaje;                                                 // Envia nuevo mensaje Xpressnet
volatile bool leerDatoXN;
byte rxXOR, rxIndice, txXOR, rxData;
byte csStatus, xnetVersion, xnetCS;
int miDireccionXpressnet;

uint8_t RS[128];
uint8_t changesRS[16];


void headerXN (byte header) {
  while (enviaMensaje) {                                                    // espera a que se envie el ultimo mensaje
  }
  //enviaMensaje = false;                                                   // ahora podemos modificar el buffer
  txBytes = HEADER;                                                         // coloca header en el buffer
  txXOR = header;
  txBuffer[txBytes++] = header;
}


void dataXN (byte dato) {
  txBuffer[txBytes++] = dato;                                               // coloca dato en el buffer
  txXOR ^= dato;
}


void sendXN () {
  txBuffer[txBytes++] = txXOR;                                              // coloca XOR byte en el buffer
  enviaMensaje = true;
}


void processXN() {
  byte n, nibble, modulo, dato;

  switch (rxBufferXN[HEADER]) {                                             // segun el header byte
    default:
      if ((rxBufferXN[HEADER] & 0xF0) == 0x40) {                            // Feedback broadcast / Accessory decoder information response (0x4X,MOD,DATA,...,XOR)
        for (n = HEADER; n < (rxBytes - 2); n += 2) {
          modulo = rxBufferXN[n + 1];
          dato = rxBufferXN[n + 2];
          nibble = (dato & 0x10) ? 0x0F : 0xF0;
          RS[modulo] &= nibble;
          nibble = (dato & 0x10) ? (dato << 4) : (dato & 0x0F);
          RS[modulo] |= nibble;
          bitSet(changesRS[modulo >> 3], modulo & 0x07);
        }
      }
      break;
  }
}


void sendUSART (byte data) {
  while (!(UCSR0A & (1 << UDRE0)));                                         // esperar a que se pueda enviar
  UDR0 = data;
}


ISR(USART_RX_vect)  {                                                       // Interrupcion recepcion datos por Serial
  if (UCSR0A & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {                  // error en la recepcion?
    rxData = UDR0;
    return;
  }
  if (UCSR0B & (1 << RXB80)) {                                              // leer primero 9 bit. Activado: Call Byte
    rxData = UDR0;
    leerDatoXN = false;
    rxIndice = HEADER;
    if (rxData == (miCallByte ^ 0xC0)) {                                    // Call Byte: P10AAAAA. Normal Inquiry
      if (enviaMensaje) {                                                   // Hay mensaje para enviar?
        delayMicroseconds(48);                                              // esperamos el tiempo de tres bit antes de enviar
        digitalWrite (_TXRX_PIN, HIGH);                                     // enviamos mensaje
        delayMicroseconds(8);                                               // esperamos el tiempo de medio bit antes de enviar
        for (rxIndice = HEADER; rxIndice < txBytes; rxIndice++)
          sendUSART (txBuffer[rxIndice]);
        WAIT_FOR_XMIT_COMPLETE
        digitalWrite (_TXRX_PIN, LOW);
        enviaMensaje = false;
      }
    }
    // Call Byte:   P11AAAAA Message,    P0100000 Feedback BC o P1100000 Broadcast
    if ((rxData == (miCallByte | 0x60)) || (rxData == 0xA0) || (rxData == 0x60)) {
      leerDatoXN = true;
      rxXOR = 0;
    }
    if (rxData == miCallByte) {                                             // Call Byte: P00AAAAA. Request ACK
      delayMicroseconds(32);                                                // esperamos el tiempo de dos bit antes de enviar
      digitalWrite (_TXRX_PIN, HIGH);                                       // respuesta inmediata si ha habido error de transmision
      delayMicroseconds(8);                                                 // esperamos el tiempo de medio bit antes de enviar
      sendUSART (0x20);
      sendUSART (0x20);
      WAIT_FOR_XMIT_COMPLETE
      digitalWrite (_TXRX_PIN, LOW);
    }
  }
  else {                                                                    // 9 bit desactivado: Datos
    rxData = UDR0;
    if (leerDatoXN) {                                                       // leer paquete Xpressnet
      rxBufferXN[rxIndice++] = rxData;
      rxXOR ^= rxData;
      if (((rxBufferXN[HEADER] & 0x0F) + 2) == rxIndice) {                  // si se han recibido todos los datos indicados en el paquete
        leerDatoXN = false;
        if (rxXOR == 0) {                                                   // si el paquete es correcto
          rxBytes = rxIndice;
          processXN();                                                      // nuevo paquete recibido, procesarlo
        }
      }
    }
  }
}


byte paridadCallByte (byte xnetAddr) {
  bool paridad = false;
  byte bits;
  xnetAddr &= 0x1F;                                                         // Borra bit 7 de la direccion
  bits = xnetAddr;
  while (bits) {                                                            // mientras haya bits cambia paridad
    paridad = !paridad;
    bits &= (bits - 1);
  }
  if (paridad)                                                              // coloca paridad en bit 7
    xnetAddr |= 0x80;
  return (xnetAddr);                                                        // P00AAAAA
}


void beginXpressNet (byte xnetAddr) {
  miCallByte = paridadCallByte (xnetAddr);
  enviaMensaje = false;
  leerDatoXN = false;
  rxIndice = DATA1;
  cli();                                                                    // deshabilitar interrupciones para acceder a registros
  UBRR0H = 0;                                                               // UBRR = (FXTAL / (16 *  baud)) - 1 si U2X = 0
  UBRR0L = 0x0F;                                                            // Set 62500 baud
  UCSR0A = 0;                                                               // U2X = 0
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << UCSZ02);     // Enable reception (RXEN) transmission (TXEN0) Receive Interrupt (RXCIE = 1)
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);                                   // UCSZ = b111 = 9 bits
  sei();                                                                    // habilitar interrupciones
  //getVersion();                                                           // pide la version del Xpressnet
  //getStatus();                                                            // pide estado de la central
}

void simplexXNET(uint8_t xnetAddr) {
  pinDef[_TX_PIN].type = _XNET;
  pinDef[_RX_PIN].type = _XNET;
  pinDef[_TXRX_PIN].type = _XNET;
  pinMode(_TXRX_PIN, OUTPUT);
  digitalWrite(_TXRX_PIN, LOW);                                             // receive data
  beginXpressNet(xnetAddr);
}


void setAccXnet(uint16_t addr, uint8_t pos) {
  byte  adr, dato;
  addr--;                                                                   // 000000AAAAAAAABB
  adr = (addr >> 2) & 0x00FF;                                               // AAAAAAAA
  dato = ((addr & 0x0003) << 1) | 0x88;                                     // 10001BBx activate
  dato |= pos;

  headerXN (0x52);                                                          // Accessory Decoder operation request    (0x52,AAAAAAAA,10001BBD,XOR)
  dataXN (adr);
  dataXN (dato);
  sendXN();
  waitTime(_XN_ACC_TOUT);
  dato &= 0xF7;                                                             // 10000BBx deactivate
  headerXN (0x52);                                                          // Accessory Decoder operation request    (0x52,AAAAAAAA,10000BBD,XOR)
  dataXN (adr);
  dataXN (dato);
  sendXN();
}


bool isFeedbackActive(uint8_t mod, uint8_t inp) {
  return (bool)(RS[mod - 1] & bit(inp - 1));
}


bool isFeedbackChanged(uint8_t mod) {
  uint8_t module;
  bool change;
  mod--;
  module = mod >> 3;
  cli();
  change = bitRead(changesRS[module], mod & 0x07);
  bitClear(changesRS[module], mod & 0x07);
  sei();
  return change;
}


void toggleAccXnet(uint16_t addr) {
  uint8_t mod, inp;
  uint16_t adr;
  if (addr < 513) {
    adr = addr - 1;
    mod = adr >> 2;
    inp = (adr & 0x03) * 2;
    inp = bitRead(RS[mod], inp) ? 0x01 : 0x00;
    setAccXnet(addr, inp);
  }
}

#endif





//-------------------------------------------------------------------------------------------------

void processSimplexBasic() {
  uint8_t n;
  currentTime = millis();
  newMS = false;
  if (lastTime != (uint8_t)currentTime) {                 // every new ms
    lastTime = (uint8_t)currentTime;
    newMS = true;
    processPins();                                        // pins
  }
  for (n = 0; n <= _MAX_FSM; n++)
    currentStateFSM[n] = nextStateFSM[n];                 // state machine
}

void receiveSimplexDCC() {
#if (USE_DCC == true)
  if (newPacket)                                          // new DCC packet received, decode it
    dccDecode();
#endif
}

void processSimplexDCC() {
#if (USE_DCC == true)
  if (accAddr > 0)                                        // get last received accessory
    accAddr = 0;
  receiveSimplexDCC();
#endif
}

void processSimplex() {
  processSimplexBasic();                                  // pins & FSM
  processTimers();                                        // timers
  processSimplexDCC();                                    // DCC
}


void waitSimplex() {
  processSimplexBasic();                                  // pins & FSM
  receiveSimplexDCC();                                    // receive DCC
}


void waitTime(uint32_t timeout) {
  uint32_t beginTime;
  beginTime = millis();
  while (millis() - beginTime < timeout)
    waitSimplex();
}

void waitServoStop(uint8_t num) {
  while (! isServoStopped(num))
    waitSimplex();
}


void waitReleaseButton(uint8_t num) {
  while (isInputActive(num))
    waitSimplex();
}
