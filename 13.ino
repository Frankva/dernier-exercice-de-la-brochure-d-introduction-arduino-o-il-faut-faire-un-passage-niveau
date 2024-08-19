#include <Servo.h>

// helper.h
void print(String toPrint="");
void print(long numberToPrint);
String input(String toPrint="");
void blinkTwoLed();
// helper.h end

// main.h
const unsigned int BOUTON = 2;
const unsigned int RED_LED = 12;
const unsigned int GREEN_LED = 13;
// main.h end

// LevelCrossingGate.h
class LevelCrossingGate
{
  private:
    unsigned char position;
    unsigned char minimum;
    unsigned char maximum;
    unsigned char middle;
    Servo *servo;

  public:
    LevelCrossingGate();
    void levelCrossingGateOpen();
    void close();
};
// LevelCrossingGate.h end

// Mode.h
class Mode
{
  public:
    virtual void invoke() = 0;
    virtual void update() = 0;
};
// Mode.h end

// App.h
typedef enum {
  failure, automatic, manual
} ModeName;

class App
{
  private:
    Mode *modes[ModeName::manual + 1];
    ModeName currentMode;
    LevelCrossingGate *gate;
  public:
    App();
    void update();
    LevelCrossingGate *getGate();
    void setCurrentMode(ModeName mode);
};

// App.h end

// FailureMode.h
class FailureMode : public Mode
{
  private:
    App *app;
    unsigned long lastExecutedMillis;
  public:
    FailureMode(App *app);
    void invoke(); 
    void update(); 
    void blinkTwoLed();
};
// FailureMode.h end

// AutomaticMode.h

typedef enum {
  redLight, greenLight
} AutomaticModeName;

class AutomaticMode: public Mode
{
  private:
    App *app;
    Mode *modes[AutomaticModeName::greenLight + 1];
    AutomaticModeName currentMode:
  public:
    AutomaticMode(App *app);
    void invoke();
    void update();
    void invokeAutomaticRedMode();
    void invokeAutomaticGreenMode();
};
// AutomaticMode.h end

// ManualMode.h
class ManualMode : public Mode
{
  private:
    App *app;
  public:
    ManualMode(App *app);
    void invoke();
    void update();
};
// ManualMode.h end

// LevelCrossingGate.cpp
LevelCrossingGate::LevelCrossingGate() {
  this->minimum = 2;
  this->maximum = 178;
  this->middle = 90;
  this->servo = new Servo();
  this->servo->attach(5);
}
void LevelCrossingGate::levelCrossingGateOpen() {
  if (this->position == this->minimum) {
    return;
  }
  this->position = this->minimum;
  this->servo->write(this->position);
}
void LevelCrossingGate::close() {
  if (this->position == this->minimum) {
    return;
  }
  this->position = this->middle;
  this->servo->write(this->position);
}
// LevelCrossingGate.cpp end

// FailureMode.cpp
FailureMode::FailureMode(App *app)
{
  this->app = app;
  lastExecutedMillis = 0;
}

void FailureMode::invoke() {
  print("invokeFailureMode");
  LevelCrossingGate *gate = this->app->getGate();
  gate->levelCrossingGateOpen();
}

void FailureMode::update() {
  this->blinkTwoLed();
  if (Serial.available() == 0) {
    return;
  }
  String answer = Serial.readStringUntil('\n');
  if (answer == "a") {
    this->app->setCurrentMode(automatic);
    return;
  }
  print("Signal invalide");
}

void FailureMode::blinkTwoLed() {
  unsigned long currentMillis = millis();
  unsigned char interval = 255;
  if (currentMillis - this->lastExecutedMillis >= interval) {
    this->lastExecutedMillis = currentMillis;
    digitalWrite(RED_LED, true);
    digitalWrite(GREEN_LED, true);
  }
  else {
    digitalWrite(RED_LED, false);
    digitalWrite(GREEN_LED, false);
  }
}

// FailureMode.cpp end

// AutomaticMode.cpp

AutomaticMode::AutomaticMode(App *app)
{
  this->app = app;
}

void AutomaticMode::invoke() {
  print("invokeAutomaticMode");
  this->invokeAutomaticRedMode();
  String answer = input("vert ou manuel ?");
  if (answer == "vert") {
    // TODO refactory that because call stack
    invokeAutomaticGreenMode();
    return;
  }
  if (answer == "manuel") {
    this->app->setCurrentMode(manual);
    return;
  }
  print("Données erronées");
  this->app->setCurrentMode(failure);
}
void AutomaticMode::invokeAutomaticRedMode() {
  print("Mode rouge");
  digitalWrite(GREEN_LED, false);
  digitalWrite(RED_LED, true);
  this->app->getGate()->close();
}

void AutomaticMode::update() {
}

void AutomaticMode::invokeAutomaticGreenMode() {
  print("Mode vert");
  digitalWrite(RED_LED, false);
  digitalWrite(GREEN_LED, true);
  this->app->getGate()->close();
  // TODO refactory that because call stack
  String answer = input("rouge ou manuel ?");
  if (answer == "rouge") {
    invokeAutomaticRedMode();
    return;
  }
  if (answer == "manuel") {
    this->app->setCurrentMode(manual);
    return;
  }
  print("Données erronées");
  this->app->setCurrentMode(failure);
}
// AutomaticMode.cpp end

// ManualMode.cpp

ManualMode::ManualMode(App *app) {
  this->app = app;
}

void ManualMode::invoke() {
}

void ManualMode::update() {
}

// ManualMode.cpp end

// App.cpp
App::App() {
  print("App");
  gate = new LevelCrossingGate();
  // FailureMode failureMode(this);
  // AutomaticMode automaticMode();
  // ManualMode manualMode(this);
  modes[failure] = new FailureMode(this);
  modes[automatic] = new AutomaticMode(this);
  modes[manual] = new ManualMode(this);
  currentMode = failure;
  pinMode(BOUTON, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  Serial.begin(9600);
  print();
}
void App::update() {
  print("App invoke");
  modes[currentMode]->update();
}
LevelCrossingGate *App::getGate() {
  return this->gate;
}
void App::setCurrentMode(ModeName mode) {
  this->currentMode = mode;
  modes[currentMode]->invoke();
}
// App.cpp end

// main.cpp begin

App *app; 

void setup() {
  print("setup");
  app = new App;
  print("setup end");
} 

void loop() {
  print("loop");
  app->update();
  print("loop end");
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
// main.cpp end

// helper.cpp

void print(String toPrint="") {
  Serial.println(toPrint);
}

void print(long numberToPrint) {
  Serial.println(String(numberToPrint));
}


String input(String toPrint="") {
  print(toPrint);
  while (Serial.available() == 0) {}
  return Serial.readStringUntil('\n');
}

// helper.cpp end
