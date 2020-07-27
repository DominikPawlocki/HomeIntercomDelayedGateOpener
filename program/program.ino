#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object

#define PIN_WITH_RELAY_FOR_GATE_OPENING 10
#define GATE_OPENING_DELAY 6
#define SECONDS_THAT_GATE_OPENING_LASTS 3

byte seconds_left_to_open_a_gate;
byte seconds_left_to_stop_opening_gate;
bool was_gate_opening_signal_triggered;
bool is_aliveness_displayed;
unsigned long timer_second;
unsigned long timer_miliSecond;
//char dashStrings[3][MAX_STRING_SIZE]; //Because strings themselves are arrays, this is in actually an example of a two-dimensional array.
char *dashStrings[] = {"-  ", " - ", "  -"};
byte dashStringsPos = 0;
char *dotStrings[] = {".  ", "  . ", "   ."}; //had to be like that to LED display with 3 segments displayed it correctly.
byte dotStringsPos = 0;

void setup()
{
  // ------------- uC config ------------------
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(PIN_WITH_RELAY_FOR_GATE_OPENING, OUTPUT);

  // ---------- LCD display config -----------------
  byte numDigits = 3;
  byte digitPins[] = {4, 8, 9};
  byte segmentPins[] = {5, 7, 2, A2, A3, 6, 3, A1};
  bool resistorsOnSegments = true; // 'false' means resistors are on digit pins
  //byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  byte hardwareConfig = N_TRANSISTORS; // See README.md for options
  bool updateWithDelays = false;       // Default 'false' is Recommended
  bool leadingZeros = false;           // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false;        // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(99);

  // ---------- gate open timers -----------------
  seconds_left_to_open_a_gate = GATE_OPENING_DELAY;
  timer_second = millis();
  seconds_left_to_stop_opening_gate = SECONDS_THAT_GATE_OPENING_LASTS * 10; //multipled by 10 cause the timer runs every 0.1 second

  // -------------------- other variables -------------
  was_gate_opening_signal_triggered = false;
  is_aliveness_displayed = false;
}

void loop()
{
  // ------------ couting down, delaying gate opening ----------------
  if (AreStillSecondsLeftForOpeningAGate())
  {
    ShowSecondsRemainingToGateOpening();
  }
  // ------------ openining a gate procedure ----------------
  if (IsGateOpeningNow())
  {
    SetGateOpeningSignalOnOrOff();
    // and just some signalling on LED display where gate is open
    ShowGateIsOpening();
  }

  // ------------ just standby, and signalling aliveness ----------------
  if (WasGateOpenAlreadyAndJustOnStandByNow())
  {
    timer_second += ShowAliveness();
  }

  sevseg.refreshDisplay(); // Must run repeatedly
}

void ShowSecondsRemainingToGateOpening()
{
  seconds_left_to_open_a_gate--;
  sevseg.setNumber(seconds_left_to_open_a_gate);
  timer_second += 1000;        //1000 is 1 sec
  timer_miliSecond = millis(); //keeping track with second timer
}

void SetGateOpeningSignalOnOrOff()
{
  // door opening procedure done once :
  if (!was_gate_opening_signal_triggered)
  {
    digitalWrite(PIN_WITH_RELAY_FOR_GATE_OPENING, HIGH);
    was_gate_opening_signal_triggered = true;
  }
  //just to omit delay() func, we want a relay to be opened for about 1 sec, not 5 sec like the gate is being opened.
  if (was_gate_opening_signal_triggered && seconds_left_to_stop_opening_gate < seconds_left_to_stop_opening_gate * 3 / 4)
  {
    digitalWrite(PIN_WITH_RELAY_FOR_GATE_OPENING, LOW);
  }
}

void ShowGateIsOpening()
{
  if (millis() >= timer_miliSecond)
  {
    sevseg.setChars(dashStrings[dashStringsPos]);
    dashStringsPos++;
    if (dashStringsPos >= 3)
      dashStringsPos = 0;
    seconds_left_to_stop_opening_gate--;
    timer_miliSecond += 100; //100 is 0.1 sec
    timer_second = millis(); //just keeping track for next function
  }
}

bool AreStillSecondsLeftForOpeningAGate()
{
  if (seconds_left_to_open_a_gate > 0 && millis() >= timer_second)
    return true;
  return false;
}

bool IsGateOpeningNow()
{
  if (seconds_left_to_open_a_gate == 0 && seconds_left_to_stop_opening_gate > 0)
    return true;
  return false;
}

bool WasGateOpenAlreadyAndJustOnStandByNow()
{
  if (seconds_left_to_open_a_gate == 0 && seconds_left_to_stop_opening_gate == 0)
    return true;
  return false;
}

unsigned long ShowAliveness()
{
  sevseg.setBrightness(1);
  //changes a sign on a LED, every 2 seconds
  if (millis() >= timer_second)
  {
    if (!is_aliveness_displayed)
    {
      sevseg.setChars(dotStrings[dotStringsPos]);
      dotStringsPos++;
      if (dotStringsPos >= 3)
        dotStringsPos = 0;
      is_aliveness_displayed = true;
    }
    else
    {
      sevseg.blank();
      is_aliveness_displayed = false;
    }
    return 2000; //short blink every two seconds, for saving energy and not making people mad...
  }
  return 0;
}
