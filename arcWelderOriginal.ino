#include <CMRI.h>

CMRI cmri;
//Arduino Shop Controller
//by K4KFH
//SYSTEM CONFIGURATION
#define ADDRESS 1  //CMRI node address (by default appears as an SMINI node)
#define WELDER_PIN 2 //pin the arc welder LED is connected to
#define LAMPS_PIN 3 //pin that the shop lamps are connected to (no current limiting built in, so make sure you've got a resistor)

unsigned long currentMillis = 0;
unsigned long previousMillisArc = 0;
unsigned long previousMillisFlicker = 0;
int timeArcOff = 3000;
int timeArcOn = 3000;
bool welding = false; //whether the welder is enabled right now. This is set with a CMRI bit.
bool arc = false; //whether there's an arc right now or not
int ledState = LOW;
int timeFlickerOn = 50;
int timeFlickerOff = 170;

void setup() {
  //set address and start CMRI serial connection
  cmri.set_address(ADDRESS);
  Serial.begin(9600, SERIAL_8N2);
  //get the welder and lamps pins set up
  pinMode(WELDER_PIN, OUTPUT);
  pinMode(LAMPS_PIN, OUTPUT);
}

void loop() {
  //ArduinoCMRI crap
  cmri.process();
  //make sure the lamps are in the right state
  digitalWrite(LAMPS_PIN, cmri.get_bit(0));
  //check the CMRI bit to see if the welder is enabled
  welding = cmri.get_bit(1);
  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  if (welding == true) {
    unsigned long currentMillis = millis();

    if (arc == false && currentMillis - previousMillisArc >= timeArcOff) { //if the arc's off, but been off long enough
      arc = true;
      previousMillisArc = currentMillis;
      timeArcOff = random(3000);
    }
    else if (arc == true && currentMillis - previousMillisArc >= timeArcOn) { //if the arc's on, but been on long enough
      arc = false;
      ledState = LOW;
      previousMillisArc = currentMillis;
      timeArcOn = random(3000);
    }

    if ( arc == true && currentMillis - previousMillisFlicker >= timeFlickerOn && ledState == HIGH) {
      previousMillisFlicker = currentMillis;
      timeFlickerOn = random(20);
      ledState = LOW;
    }
    if ( arc == true && currentMillis - previousMillisFlicker >= timeFlickerOff && ledState == LOW) {
      previousMillisFlicker = currentMillis;
      timeFlickerOff = random(100);
      ledState = HIGH;
    }
    digitalWrite(WELDER_PIN, ledState);
  }
  else {
    digitalWrite(WELDER_PIN, LOW);
  }
}
