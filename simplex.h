/*    simplex.h -- F. Cañada 2025 --  https://usuaris.tinet.cat/fmco/
                                      https://github.com/fmcopaco/simplex

      v0.1  13sep25   Initial version. Basic and DCC commands.
      v0.2  21sep25   Added PWM effect. Added Xpressnet commads.
*/

#ifndef SIMPLEX_H
#define SIMPLEX_H

////////////////////////////////////////////////////////////
// ***** USER OPTIONS *****
////////////////////////////////////////////////////////////

// Set next definitions to to false to save memory
#define USE_DCC       true                                      // Use DCC decoding
#define USE_XNET      true                                      // Use Xpressnet decoding

#define _SPD_SERVO    100                                       // Default speed of servo
#define _COIL_TOUT    200                                       // Default pulse time (COIL)
#define _INPUT_TOUT   20                                        // Default time for debouncing (INPUT)
#define _SENSOR_TOUT  500                                       // Default time for debouncing (SENSOR)
#define _FLASH_TOUT   500                                       // Default time for flashing (OUT/EFFECT)
#define _XN_ACC_TOUT  150                                       // Time for activate Xpressnet accesories
#define _MAX_TIMERS   4                                         // Max. defined timers
#define _MAX_REPEAT   4                                         // Max. nested repeats
#define _MAX_FSM      10                                        // Max. FSM

////////////////////////////////////////////////////////////
// ***** END OF USER OPTIONS *****                              // Don't change anything below here
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// ***** SIMPLEX HARDWARE DEPENDENT *****
////////////////////////////////////////////////////////////

#define _MAX_PINS     20                                        // Max. pins of the device (Arduino UNO/Nano: 20)
#define _MAX_SERVO    12                                        // Max. servo available (library Servo.h: 12)
#define _DCC_PIN      2                                         // DCC input pin. Must use interrupts
#define _TX_PIN       0                                         // Serial TX pin for Xpressnet (MAX485)
#define _RX_PIN       1                                         // Serial RX pin for Xpressnet (MAX485)
#define _TXRX_PIN     A3                                        // Direction pin for Xpressnet (MAX485)

////////////////////////////////////////////////////////////
// ***** SIMPLEX BASIC DEFINED FUNCTIONS *****
////////////////////////////////////////////////////////////

#define RUN_SIMPLEX     processSimplex();                       // main simplex loop

#define SET_NAME(n,p)   const uint8_t n = p;                    // generic definition of a constant. Used to define names
#define VAR_NAME(n)     int n;                                  // generic definition of variable
#define SET_VAR(n,p)    n = p;                                  // set new value of a variable

#define PIN_OUT(p)      setType(p, _OUT);                       // set a pin for fixed output (ex. relay, lights,...)
#define PIN_COIL(p)     setType(p, _COIL);                      // set a pin for pulse output (ex. coil)
#define PIN_EFFECT(p)   setType(p, _EFFECT);                    // set a pin for light effect
#define PIN_SERVO(p)    setType(p, _SERVO);                     // set a pin for servo (max. 12: _MAX_SERVO, from 0º to 180º)
#define PIN_INPUT(p)    setType(p, _INPUT);                     // set a pin as digital input
#define PIN_BUTTON(p)   setType(p, _BUTTON);                    // set a pin as debouncing input (ex. pushutton)
#define PIN_SENSOR(p)   setType(p, _SENSOR);                    // set a pin as sensor

#define PIN_MODE(p,n)   setModePin(p,n);                        // set the pin mode (type OUT/EFFECT)
#define PIN_TIME(p,n)   setTimePin(p,n);                        // set pin timeout/servo speed

#define SET(p)          setPin(p, true);                        // activate output pin (type OUT/EFFECT/COIL)
#define RESET(p)        setPin(p, false);                       // deactivate output pin (type OUT/EFFECT/COIL)
#define SERVO(p,n)      setServoPos(p,n);                       // move servo to new position (0..180)
#define ARRIVED(p)      if (isServoStopped(p))                  // check if servo arrived

#define ACTIVE(p)       if (isInputActive(p))                   // check if input active
#define FREE(p)         if (!isInputActive(p))                  // check if input
#define PRESSED(p)      if (isInputActive(p))                   // check if button pressed

#define FSM_NEW(n)      const uint8_t n = newFSM();             // FSM, add a new FSM
#define FSM_USE(n)      useFSM(n);                              // FSM, use FSM
#define FSM_NAME(n)     const uint8_t n = newState();           // FSM, name definition of a state
#define FSM_STATE(n)    if (getStateFSM() == n)                 // FSM, check for a state (0..255)
#define FSM_GO(n)       setStateFSM(n);                         // FSM, transition to new state (0..255)

#define SET_TIMER(p,n)  setTimer(p,n);                          // start timer (max. 4: _MAX_TIMERS)
#define TIMEOUT(p)      if(isTimeout(p))                        // check if timer expired (max. 4: _MAX_TIMERS)

#define WAIT(n)         waitTime(n);                            // Wait time running simplex loop
#define WAIT_SERVO(p)   waitServoStop(p);                       // Wait servo to arrive new position running simplex loop
#define WAIT_RELEASE(p) waitReleaseButton(p);                   // Wait to release button  

#define FUNCTION(n)     void n()                                // Define a function
#define CALL(n)         n();                                    // Call a function
#define REPEAT(n)       newRepeat();while(repeatSimplex(n))     // repeat commands (2..65535 times, max. 4 nested repeats: _MAX_REPEAT)

#define AND(x,y)        if ((x) && (y))                         // check if both condition are true
#define OR(x,y)         if ((x) || (y))                         // check if at least one condition is true
#define NOT(x)          (!x)                                    // logical negation
#define EQUAL(x,y)      if ((x) == (y))                         // check if both conditions are equal


#if (USE_DCC == true)
////////////////////////////////////////////////////////////
// ***** SIMPLEX DCC DEFINED FUNCTIONS *****
////////////////////////////////////////////////////////////

#define PIN_DCC()           simplexDCC();                       // initialize DCC input pin (_DCC_PIN)
#define DCC_ACC_RED(n)      if (isAccessoryDCC(n, 0x00))        // check if received accessory number to red
#define DCC_ACC_GREEN(n)    if (isAccessoryDCC(n, 0x01))        // check if received accessory number to green

#endif


#if (USE_XNET == true)
////////////////////////////////////////////////////////////
// ***** SIMPLEX XNET DEFINED FUNCTIONS *****
////////////////////////////////////////////////////////////

#define XNET_ADDR(n)        simplexXNET(n);                     // initialize Xpressnet interface (MAX485). Valid address: 1..31
#define XNET_RED(n)         setAccXnet(n, 0x00);                // Set accesory number to red
#define XNET_GREEN(n)       setAccXnet(n, 0x01);                // Set accesory number to green
#define XNET_TOGGLE(n)      toggleAccXnet(n);                   // Toggle accessory position
#define XNET_ACTIVE(n,p)    if (isFeedbackActive(n,p))          // check if RS input occupied
#define XNET_FREE(n,p)      if (!isFeedbackActive(n,p))         // check if RS input free
#endif


//------------------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
// ***** SIMPLEX BASIC LIBRARY *****
////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Servo.h>


typedef enum {_UNDEF, _OUT, _EFFECT, _SERVO, _COIL, _BUTTON, _INPUT, _SENSOR, _DCC, _XNET, _LNET} pinType;

typedef enum {NORMAL, INVERT, FLASH, DIMMER, CANDLE, FIRE, FLUORESCENT, WELDING, PWM} modeType;

typedef struct {
  pinType   type;                                               // pin type
  bool      active;                                             // current status
  bool      invert;                                             // invert status
  bool      flash;                                              // Flash type
  bool      state;                                              // current state
  uint8_t   index;                                              // index to servo array or effect
  uint8_t   pos;                                                // servo current position or effect phase/start/stop
  uint8_t   target;                                             // servo target position
  uint8_t   pwm;                                                // current PWM
  uint8_t   counter;                                            // counter
  uint32_t  timer;                                              // timer for output
  uint32_t  timeout;                                            // timeout
} pinData;


typedef struct {
  bool      active;                                             // current status
  bool      expired;
  uint32_t  timer;                                              // timer for output
  uint32_t  timeout;                                            // timeout
} delayData;


void setupSimplex();
void processSimplex();
void setType(uint8_t num, pinType type);
void setPin(uint8_t num, bool state);
void setModePin(uint8_t num, modeType mode);
void setTimePin (uint8_t num, uint32_t timeout);
void setServoPos(uint8_t num, uint8_t pos);
void setTimer(uint8_t tmr, uint32_t timeout);
bool isTimeout(uint8_t tmr);
void setStateFSM(uint8_t state);
uint8_t getStateFSM();
void waitTime(uint32_t timeout);
bool isServoStopped(uint8_t num);
bool isInputActive(uint8_t num);
void waitServoStop(uint8_t num);
void waitReleaseButton(uint8_t num);
void newRepeat();
uint8_t newState();
uint8_t newFSM();
uint8_t useFSM(uint8_t num);
bool repeatSimplex(uint16_t num);


#if (USE_DCC == true)
////////////////////////////////////////////////////////////
// ***** SIMPLEX DCC LIBRARY *****
////////////////////////////////////////////////////////////

void dccISR();
void simplexDCC();
bool isAccessoryDCC(uint16_t adr, uint8_t state);

#endif

#if (USE_XNET == true)
////////////////////////////////////////////////////////////
// ***** SIMPLEX XPRESSNET LIBRARY *****
////////////////////////////////////////////////////////////

void simplexXNET(uint8_t xnetAddr);
void setAccXnet(uint16_t addr, uint8_t pos);
void toggleAccXnet(uint16_t addr);
bool isFeedbackActive(uint8_t mod, uint8_t inp);

#endif



#endif
