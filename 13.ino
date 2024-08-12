#include <Servo.h>

void print(String toPrint="");
void print(long numberToPrint);
String input(String toPrint="");
void blinkTwoLed();

const unsigned int BOUTON = 2;
const unsigned int RED_LED = 12;
const unsigned int GREEN_LED = 13;
unsigned long lastExecutedMillis = 0;

class LevelCrossingGate
{
  private:
    unsigned char position;
    unsigned char minimum;
    unsigned char maximum;
    unsigned char middle;
    Servo servo;

  public:
    void LevelCrossingGate() {
      this->minimum = 2;
      this->maximum = 178;
      this->middle = 90;
      this->servo.attach(5);
    }
    void levelCrossingGateOpen() {
      if (this->position == this->minimum) {
        return;
      }
      this->position = this->minimum;
      this->servo.write(this->position);
    }
    void levelCrossingGateClose() {
      if (this->position == this->minimum) {
        return;
      }
      this->position = this->middle;
      this->servo.write(this->position);
    }
};


class Mode
{
  public:
    virtual void invoke() = 0;
};

class FailureMode : public Mode
{
  public:
    void invoke() {
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

};

class AutomaticMode: public Mode
{
  private:
    Mode modes[2];
  public:
    AutomaticMode() {

    }
    void invoke() {
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
};

class ManualMode : public Mode
{
};

typedef enum {
  failure, automatic, manual
} ModeName;

class App
{
  private:
    Mode modes[ModeName::manual + 1];
    unsigned char currentMode;
    LevelCrossingGate gate();
  public:
    App() {
      print("App");
      FailureMode failureMode();
      AutomaticMode automaticMode();
      ManualMode = manualMode();
      modes[failure] = failureMode;
      modes[automatic] = automaticMode;
      modes[manual] = manualMode;
      currentMode = failure;
      pinMode(BOUTON, INPUT);
      pinMode(RED_LED, OUTPUT);
      pinMode(GREEN_LED, OUTPUT);
      Serial.begin(9600);
      print();
    }
    void update() {
      print("App invoke");
      modes[currentMode].invoke();
    }
    
};

App app();

void setup() {
  print("setup");
} 

void loop() {
  print("loop");
  app.update();
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


String input(String toPrint="") {
  print(toPrint);
  while (Serial.available() == 0) {}
  return Serial.readStringUntil('\n');
}




