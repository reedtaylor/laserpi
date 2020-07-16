#include <iostream>
#include <wiringPi.h>


#define BUSY_DELAY 5

/*** PIN ASSIGNMENTS ***/
#define P_MAN_FIRE_IN 0
#define P_GERBIL_FIRE_IN 2
#define P_FIRE_OUT 3
#define P_SWITCH_MAN_IN 4
#define P_ARM_OUT 5

/*** LOGIC VALUES ***/
#define FIRE_ACTIVE 0
#define MANUAL_CONTROL_ACTIVE 0
#define ARMED_ACTIVE 0


void initializePins() {
  // Input from the "FIRE" button.
  // Goal = multiplex this with the "FIRE" from the GERBIL
  // depending on the state of MANUAL signal.  Also maybe
  // prevent firing as a (non-primary) safety interlock layer
  pinMode(P_MAN_FIRE_IN, INPUT);
  if (FIRE_ACTIVE == 0) {
    // Active Low
    pullUpDnControl(P_MAN_FIRE_IN, PUD_UP);
  } else {
    // Active High
    pullUpDnControl(P_MAN_FIRE_IN, PUD_DOWN);
  }
    

  // Input from the gerbil "FIRE" output, would be wired to "LO" on the LPSU.  Active low.
  // Goal = multiplex this with the "FIRE" from the panel button
  // depending on the state of MANUAL signal.  Also maybe
  // prevent firing as a (non-primary) safety interlock layer
  pinMode(P_GERBIL_FIRE_IN, INPUT);
  if (FIRE_ACTIVE == 0) {
    // Active Low
    pullUpDnControl(P_GERBIL_FIRE_IN, PUD_UP);
  } else {
    // Active High
    pullUpDnControl(P_GERBIL_FIRE_IN, PUD_DOWN);
  }

  // Output to fire the laser via the "LO" input of the LPSU.  Active low.
  // See "..._FIRE_IN" pins
  pinMode(P_FIRE_OUT, OUTPUT);
  digitalWrite(P_FIRE_OUT, (!FIRE_ACTIVE)); // assert inactive ASAP

  // Input determines whether the panel is in "MANUAL" vs "PWM" aka gerbil-controlled
  // mode. Non-manual / PWM as is considered the "safer" configuration.
  pinMode(P_SWITCH_MAN_IN, INPUT);
  if (MANUAL_CONTROL_ACTIVE == 0) {
    // Low means manual control; default to PWM
    pullUpDnControl(P_SWITCH_MAN_IN, PUD_UP);

  } else {
    // Hogh means manual control; default to PWM
    pullUpDnControl(P_SWITCH_MAN_IN, PUD_DOWN);
  }
  
  // Output for arming the laser via the "P" input, active low.
  // Note that this is knot conneected directly to the LPSU but rather runs through the
  // SPST "ARM" panel switch as well as any other mechanical interlocks before connection
  // to the LPSU.   Goal is to make the active-low arm signal (un)available based on
  // other inputs to this logic.
  // TODO: confirm that not-low is 3.3v.
  // TODO: external pullup for safety when this system is inpoerative
  pinMode(P_ARM_OUT, OUTPUT);
  digitalWrite(P_ARM_OUT, (!ARMED_ACTIVE)); // assert inactive ASAP
}

int main (void)
{
  wiringPiSetup();
  initializePins();

  while (true) {

    /**** DEFAULT STATE ****/
    // goal is to have all of these in their "safest" configs
    bool isManual = false;
    bool isFiring = false;
    bool isReady = false;
    
    /**** READ INPUTS ****/
    int switchFireIn = digitalRead(P_MAN_FIRE_IN);
    int gerbilFireIn = digitalRead(P_GERBIL_FIRE_IN);
    int switchManIn = digitalRead(P_SWITCH_MAN_IN); 


    /**** DECODE INPUTS and UPDATE STATE ****/
    if (switchManIn == MANUAL_CONTROL_ACTIVE) {
      isManual = true;
    } else {
      isManual = false;
    }

    if (true) {  // TODO: add logic for safety interlocks e.g. water flow / temp
      isReady = true;
    } else {
      // TODO: think about how to display the fault state
      isReady = false;
    }


    /**** ASSERT OUTPUTS ****/
    if (isReady) {
      digitalWrite(P_ARM_OUT, ARMED_ACTIVE);

      if (isManual) {
	digitalWrite(P_FIRE_OUT, switchFireIn);
      } else {
	digitalWrite(P_FIRE_OUT, gerbilFireIn);
      }
    } else {
      digitalWrite(P_ARM_OUT, !(ARMED_ACTIVE));
      digitalWrite(P_FIRE_OUT, !(FIRE_ACTIVE));
    }
    
    // to avoid swamping the CPU completely... should probably do something smarter like
    // interrupts or timers as this blocking delay currently places a ceiling on the reaction time with
    // possible safety consequences
    delay(BUSY_DELAY);


  }


  return 0 ;
}


