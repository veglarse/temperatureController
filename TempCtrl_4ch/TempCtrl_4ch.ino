/*
  Analog input, analog output, serial output

 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.

 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground

 created 29 Dec. 2008
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */
// Libraries
#include <TimerOne.h>

// These constants won't change.  They're used to give names
// to the pins used:
const int num_channels = 4;

// Some global variables
volatile int counter = 1;

//Temperature sensors (NTC)
int temp_sensor_pin[num_channels] = {A3,A2,A1,A0};
int heater_pin[num_channels] = {3,5,6,9};

int sensorValue[num_channels];        // value read from the pot
volatile int outputValue[num_channels];        // value output to the PWM (analog out)


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  // Init pins
  int channel;
  for (channel = 0; channel < num_channels; channel = channel +1) {
    pinMode(temp_sensor_pin[channel], INPUT);
    pinMode(heater_pin[channel], OUTPUT);
  }

  Timer1.initialize(1000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here

}

void loop() {
  int channel;
  for (channel = 0; channel < num_channels; channel = channel +1) {
    // read the analog in value:
    sensorValue[channel] = analogRead(temp_sensor_pin[channel]);
    // map it to the range of the analog out:
    if (sensorValue[channel] < 1022) {
      outputValue[channel] = map(sensorValue[channel], 950, 970, 0, 255);
    }
    else {
      outputValue[channel] = 0;
    }
    // change the analog out value:
    //analogWrite(heater_pin[channel], outputValue[channel]);
  
    // print the results to the serial monitor:

    Serial.print("\t T = " );
    Serial.print(sensorValue[channel]);
    Serial.print("\t H = ");
    Serial.print(outputValue[channel]);
  }
  Serial.println();
  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);
}


/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    // Count time
    counter = counter + 1;
    if (counter > 1024) {
      counter = 1;
    }

    // Set LED state
    int channel;
    for (channel = 0; channel < num_channels; channel = channel +1) {
      int offset = 256*channel;
      if ((counter > offset) && (counter <= (outputValue[channel]+offset))) {
        digitalWrite( heater_pin[channel], 1);
      }
      else {
        digitalWrite( heater_pin[channel], 0);
      }
    }
}
