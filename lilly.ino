bool serialEnabled = true;

bool lastState2;
bool lastState3;

const int pinCount = 6;
boolean currentState[pinCount];
boolean lastState[pinCount];

const int numPairs = 3;
int pairs[][2] = { {0, 1}, {2, 3}, {4, 5} };
int pins[] = { 2, 3, 4, 5, 7, 8 };

int en = 16;
bool enabled = false;
bool lastStateEn = true;
unsigned long lastChangeMs = 0;
unsigned long _start;
unsigned long _stop;

const int pwmPairs = 2;
int pwmCurrent [pwmPairs][3];
int pwmNpn[] = {14, 15};
int currentlyAdjusting = 0;

void setup() {
  if(serialEnabled){
    Serial.begin(115200);
  }
  
  // Enable en pin
  pinMode(en, INPUT_PULLUP);
  
  for(int i = 0; i < pwmPairs; i++){
    pinMode(pwmNpn[i], OUTPUT);
  }

  // Enable rotary pins
  for(int i = 0; i < pinCount; i++){
    pinMode(pins[i], INPUT_PULLUP);
    currentState[i] = digitalRead(pins[i]);
    lastState[i] = digitalRead(pins[i]);
  }
}

  /*
  _start = millis();
  
  _stop = millis();
  int len = _stop - _start;
  Serial.println(String(len));
 */
 
void loop() {
  readRotaries();
  checkPairs();
  checkToggle();
  playPwm();
}

void readRotaries(){
  for(int i = 0; i < pinCount; i++){
    currentState[i] = digitalRead(pins[i]);
  }
}

void checkToggle(){
  // Enable or 
  if(lastStateEn != digitalRead(en) && millis() - lastChangeMs > 150){
    lastChangeMs = millis();
    lastStateEn = digitalRead(en);
    
    debugPrint("State change:" + String(lastStateEn));
    
    if(!lastStateEn){
      enabled = !enabled;
      currentlyAdjusting++;
      
      if(currentlyAdjusting >= pwmPairs){
        currentlyAdjusting = 0;
      }

      debugPrint("Adjusting:" + String(currentlyAdjusting));
    }
  }
}

// Play PWM table
void playPwm(){
  for(int i = 0; i < pwmPairs; i++){
    analogWrite(6, pwmCurrent[i][0]);
    analogWrite(9, pwmCurrent[i][1]);
    analogWrite(10, pwmCurrent[i][2]);
    

    // Show LED
    digitalWrite(pwmNpn[i], true);
    delay(7);
    digitalWrite(pwmNpn[i], false);
  }
}

// Adjust PWM table
void adjustPWM(int pin, int ammount){
  pwmCurrent[currentlyAdjusting][pin] = pwmCurrent[currentlyAdjusting][pin] + ammount;

  if(pwmCurrent[currentlyAdjusting][pin] < 0){
    pwmCurrent[currentlyAdjusting][pin] = 0;
  }

  if(pwmCurrent[currentlyAdjusting][pin] > 255){
    pwmCurrent[currentlyAdjusting][pin] = 255;
  }

  for(int i = 0; i < pwmPairs; i++){
    debugPrint(String(i) + ":" + String(pwmCurrent[i][0]) + "," + String(pwmCurrent[i][1]) + "," + String(pwmCurrent[i][2]));
  }
}

// Check rotary encoders
void checkPairs(){
  for(int i = 0; i < numPairs; i++){
    // If there is a state change
    if(currentState[pairs[i][0]] != lastState[pairs[i][0]] || currentState[pairs[i][1]] != lastState[pairs[i][1]]){
      
      // If both pins are pulled down
      if(currentState[pairs[i][0]] == 0 && currentState[pairs[i][1]] == 0){
        if(lastState[pairs[i][0]] == 1){
          debugPrint("Pair " + String(i) + " cc");

          if(i == 0){
            adjustPWM(0, -5);
          }
          if(i == 1){
            adjustPWM(1, -5);
          }
          if(i == 2){
            adjustPWM(2, -5);
          }
        }
        else{
          debugPrint("Pair " + String(i) + " cw");
          if(i == 0){
            adjustPWM(0, 5);
          }
          if(i == 1){
            adjustPWM(1, 5);
          }
          if(i == 2){
            adjustPWM(2, 5);
          }
        }
      }

      // Update last state
      lastState[pairs[i][0]] = currentState[pairs[i][0]];
      lastState[pairs[i][1]] = currentState[pairs[i][1]];
    }
  }
}

void debugPrint(String data){
  if(serialEnabled){
    Serial.println(data);
  }
}
