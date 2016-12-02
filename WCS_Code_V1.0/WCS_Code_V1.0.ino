#include <elapsedMillis.h>
elapsedMillis timeElapsed;

unsigned int stopflow = 1000;

#define FLOWSENSORPIN 2
int val = 0;
// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!

SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}

void setup() {
  pinMode(5, OUTPUT); //solenoid valve
  pinMode(3, INPUT); //reset button
  pinMode(2, INPUT); //flow meter
  Serial.begin(9600);
  Serial.print("Flow sensor test!");
  lastflowpinstate = digitalRead(FLOWSENSORPIN);
  useInterrupt(true);

}

void loop() {
    Serial.print("LOOP!");
    timeElapsed = 0;
    run();

}
void run(){
  digitalWrite(5, HIGH);
  Serial.print("Main!");
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;
  if(liters >= 0){ //change later
    timeElapsed = 0;
    while(timeElapsed < stopflow){
      digitalWrite(5, HIGH);
      if(liters < 2){
        loop();
      }
      float liters = pulses;
      liters /= 7.5;
      liters /= 60.0;
    
      Serial.print(liters); Serial.println(" Liters");
    }
  }
  stopped();
}

void stopped(){
  Serial.print("Stopped!");
  digitalWrite(5, LOW);
  val = digitalRead(3);
  while(val == LOW){
    val = digitalRead(3);
  }
  loop();
}
