#include <Servo.h>

void invokeFailureMode();
void invokeAutomaticMode();
void invokeManualMode();
void print(String toPrint="");
void print(long numberToPrint);
String input(String toPrint="");
void blinkTwoLed();

const unsigned int BOUTON = 2;
const unsigned int RED_LED = 12;
const unsigned int GREEN_LED = 13;
unsigned long lastExecutedMillis = 0;

typedef struct LevelCrossingGate {
  unsigned char position;
  unsigned char minimum;
  unsigned char maximum;
  unsigned char middle;
  Servo servo;
} LevelCrossingGate;

enum {
  failure, automatic, manual
};

typedef struct Mode {
  void (*invoke)(void);
} Mode;


LevelCrossingGate gate;
Mode failureMode;
Mode automaticMode;
Mode manualMode;
Mode modes[3];
unsigned char currentMode;

void setup() {
  print("setup");
  LevelCrossingGateInit(&gate);
  failureMode.invoke = invokeFailureMode;
  automaticMode.invoke = invokeAutomaticMode;
  manualMode.invoke = invokeManualMode;
  modes[0] = failureMode;
  modes[1] = automaticMode;
  modes[2] = manualMode;
  currentMode = failure;

  pinMode(BOUTON, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  Serial.begin(9600);
  print();
} 

void loop() {
  print("loop");
  modes[currentMode].invoke();
  
} 

void LevelCrossingGateInit(LevelCrossingGate *gate) {
  gate->minimum = 2;
  gate->maximum = 178;
  gate->middle = 90;
  gate->servo.attach(5);
}

void allumeLed(bool etatBouton) {
  if (etatBouton) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  }
  else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  }
}

void print(String toPrint="") {
  Serial.println(toPrint);
}

void print(long numberToPrint) {
  Serial.println(String(numberToPrint));
}

void invokeFailureMode() {
  print("invokeFailureMode");
  blinkTwoLed();
  levelCrossingGateOpen(&gate);

  if (Serial.available() == 0) {
    return;
  }
  String answer = Serial.readStringUntil('\n');
  if (answer == "a") {
    currentMode = automatic;
    return;
  }
  print("Signal invalide");
}

void invokeAutomaticMode() {
  print("invokeAutomaticMode");
  invokeAutomaticRedMode();
  String answer = input("vert ou manuel ?");
  if (answer == "vert") {
    // TODO refactory that because call stack
    invokeAutomaticGreenMode();
    return;
  }
  if (answer == "manuel") {
    currentMode = manual;
    return;
  }
  print("Données erronées");
  currentMode = failure;
  
}

void invokeAutomaticRedMode() {
  print("Mode rouge");
  digitalWrite(GREEN_LED, false);
  digitalWrite(RED_LED, true);
  levelCrossingGateClose(&gate);
}

void invokeAutomaticGreenMode() {
  print("Mode vert");
  digitalWrite(RED_LED, false);
  digitalWrite(GREEN_LED, true);
  levelCrossingGateOpen(&gate);
  // TODO refactory that because call stack
  String answer = input("rouge ou manuel ?");
  if (answer == "rouge") {
    invokeAutomaticRedMode();
    return;
  }
  if (answer == "manuel") {
    currentMode = manual;
    return;
  }
  print("Données erronées");
  currentMode = failure;
}

void invokeManualMode() {
  print("invokeManualMode");
}

void blinkTwoLed() {
  unsigned long currentMillis = millis();
  unsigned char interval = 255;
  if (currentMillis - lastExecutedMillis >= interval) {
    lastExecutedMillis = currentMillis;
    digitalWrite(RED_LED, true);
    digitalWrite(GREEN_LED, true);
  }
  else {
    digitalWrite(RED_LED, false);
    digitalWrite(GREEN_LED, false);
  }
}

void levelCrossingGateOpen(LevelCrossingGate *gate) {
  if (gate->position == gate->minimum) {
    return;
  }
  gate->position = gate->minimum;
  gate->servo.write(gate->position);
}

void levelCrossingGateClose(LevelCrossingGate *gate) {
  if (gate->position == gate->minimum) {
    return;
  }
  gate->position = gate->middle;
  gate->servo.write(gate->position);
}

String input(String toPrint="") {
  print(toPrint);
  while (Serial.available() == 0) {}
  return Serial.readStringUntil('\n');
}




