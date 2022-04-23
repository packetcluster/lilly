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

void setup() {
  Serial.begin(115200);

  // Enable en pin
  pinMode(en, INPUT_PULLUP);
  pinMode(14, OUTPUT);

  // Enable rotary pins
  for(int i = 0; i < pinCount; i++){
    pinMode(pins[i], INPUT_PULLUP);
    currentState[i] = digitalRead(pins[i]);
    lastState[i] = digitalRead(pins[i]);
  }
}

void loop() {
  // Read current state
  for(int i = 0; i < pinCount; i++){
    currentState[i] = digitalRead(pins[i]);
  }

  // Enable or 
  if(lastStateEn != digitalRead(en) && millis() - lastChangeMs > 150){
    lastChangeMs = millis();
    lastStateEn = digitalRead(en);
    
    Serial.println("State change:" + String(lastStateEn));
    
    if(!lastStateEn){
      enabled = !enabled;
      digitalWrite(14, enabled);
    }
  }
  
  checkPairs();
}
int pwm[16];

void adjustPWM(int pin, int ammount){
  pwm[pin] = pwm[pin] + ammount;

  if(pwm[pin] < 0){
    pwm[pin] = 0;
  }

  if(pwm[pin] > 255){
    pwm[pin] = 255;
  }

  Serial.println(String(pwm[pin]));
  analogWrite(pin, pwm[pin]);
}

void checkPairs(){
  for(int i = 0; i < numPairs; i++){
    // If there is a state change
    if(currentState[pairs[i][0]] != lastState[pairs[i][0]] || currentState[pairs[i][1]] != lastState[pairs[i][1]]){
      
      // If both pins are pulled down
      if(currentState[pairs[i][0]] == 0 && currentState[pairs[i][1]] == 0){
        if(lastState[pairs[i][0]] == 1){
          Serial.println("Pair " + String(i) + " cc");

          if(i == 0){
            adjustPWM(6, -5);
          }
          if(i == 1){
            adjustPWM(9, -5);
          }
          if(i == 2){
            adjustPWM(10, -5);
          }
        }
        else{
          Serial.println("Pair " + String(i) + " cw");
          if(i == 0){
            adjustPWM(6, 5);
          }
          if(i == 1){
            adjustPWM(9, 5);
          }
          if(i == 2){
            adjustPWM(10, 5);
          }
        }
      }

      // Update last state
      lastState[pairs[i][0]] = currentState[pairs[i][0]];
      lastState[pairs[i][1]] = currentState[pairs[i][1]];
    }
  }
}
