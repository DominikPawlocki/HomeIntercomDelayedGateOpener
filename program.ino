#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object

#define PIN_WITH_PRZEKAZNIK_FOR_GATE_OPENING 10

byte gate_open_delay;
byte gate_is_opened_delay;
bool is_gate_opening_triggered;
bool isAliveDisplayed;
unsigned long timer_second;
unsigned long timer_miliSecond;
//char dashStrings[3][MAX_STRING_SIZE]; //Because strings themselves are arrays, this is in actually an example of a two-dimensional array.
char* dashStrings[]={"-  ", " - ", "  -"};
byte dashStringsPos = 0;
char* dotStrings[]={".  ", "  . ", "   ."};
byte dotStringsPos = 0;

void setup() {
  // ------------- uC config ------------------
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(PIN_WITH_PRZEKAZNIK_FOR_GATE_OPENING, OUTPUT); 

  // ---------- LCD display config -----------------
  byte numDigits = 3;

  byte digitPins[] = {4, 8, 9};
  byte segmentPins[] = {5, 7, 2, A2, A3, 6, 3, A1};
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  //byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  byte hardwareConfig = N_TRANSISTORS ; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(99);

  // ---------- gate open timers -----------------
  gate_open_delay = 6;
  timer_second = millis();
  gate_is_opened_delay = 3*10; //multipled by 10 cause the timer runs every 0.1 second

  // -------------------- other variables -------------
 /* strcpy(dashStrings[0], "-  ");
  strcpy(dashStrings[1], " - ");
  strcpy(dashStrings[2], "  -");*/

  is_gate_opening_triggered = false;
  isAliveDisplayed = false;
}

void loop() { 
  // ------------ couting down, delaying gate opening ----------------
  if (gate_open_delay > 0 && millis() >= timer_second ) {
    gate_open_delay--;
    sevseg.setNumber(gate_open_delay);
    timer_second += 1000; //1000 is 1 sec
    timer_miliSecond = millis(); //keeping track with second timer
  }
  
  // ------------ openining a gate procedure ----------------
  if (gate_open_delay == 0 && gate_is_opened_delay > 0 ) {
    // door opening procedure done once :
    if (!is_gate_opening_triggered){
      digitalWrite(PIN_WITH_PRZEKAZNIK_FOR_GATE_OPENING,HIGH);
      is_gate_opening_triggered = true;
    }
    //just to omit delay() func, we want a przekaznik to be opened for about 1 sec, not 5 sec like the gate is being opened.
    if (is_gate_opening_triggered && gate_is_opened_delay <gate_is_opened_delay*3/4){
      digitalWrite(PIN_WITH_PRZEKAZNIK_FOR_GATE_OPENING,LOW);
    }
    // and just some blinking LED display where gate is open
    if (millis() >= timer_miliSecond) {
      sevseg.setChars(dashStrings[dashStringsPos]);
      dashStringsPos++;
      if (dashStringsPos >= 3) dashStringsPos = 0;
      gate_is_opened_delay--;
      timer_miliSecond += 100; //100 is 0.1 sec
      timer_second = millis(); //just keeping track for next function
    }
  }

   // ------------ just standby, and signalling aliveness ----------------
  timer_second += SignalAliveness();
  
  //if (millis() >= timer) {
  //  minutes++; // 100 milliSeconds is equal to 1 deciSecond
  //  timer += 600; //6000 it was 1 minute ! 

    //digitalWrite(18,HIGH);
    //digitalWrite(19,HIGH);
    
   // sevseg.setNumber(minutes, 1);
 // }

  sevseg.refreshDisplay(); // Must run repeatedly
}

unsigned long SignalAliveness(){
    if (gate_open_delay == 0 && gate_is_opened_delay == 0 ) {
    sevseg.setBrightness(1);
    //changes a sign on a LED, every 2 seconds
    if (millis() >= timer_second) {
      if(!isAliveDisplayed){
        sevseg.setChars(dotStrings[dotStringsPos]);
        dotStringsPos++;
        if (dotStringsPos >= 3) dotStringsPos = 0;
        isAliveDisplayed = true;
      }
      else{
        sevseg.blank();
        isAliveDisplayed = false;
        }
      return 2000; //short blink every ten second, for saving energy and not making people mad...
    }
  }
  return 0;
}
