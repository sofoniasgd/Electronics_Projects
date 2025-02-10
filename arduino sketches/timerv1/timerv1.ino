/*
    Relay Timer

    Turns a relay on and off at a set interval, 
    the interval will be set by a potentiometer while holding down a push button

    The circuit:
    * potentiometer connected to pin A0
    * push button connected to pin 
    * Relay connected to pins 13
    * three digit common anode display connected to pins  0 - 9
    * led indicators connected to pin

    Created 5 Feb 2025
    By Sofonias Gashaw
    

    http://harmonyz.dev

*/

      // OUTPUT PINS
// Segment pins (Common Anode: LOW turns a segment ON)
const int segmentPins[7] = {0, 1, 2, 3, 4, 9, 10}; 
const int digitPins[3] = {11, 12, 13}; // Common anode pins for each digit

const int relay = 8; // relay pins
// led out pins
const int LED1 = 6;

      // INPUT PINS
int push_button = 7;
int potentiometer = A0; 
int sensorValue = 0;  // variable to store the value coming from the sensor
int pot_value = 0;

//timer value in milliseconds
int timeDifference = 0;
int timeLeft = 0;
int newTimeDifference = 0;
//relay status
bool relay_status = 0;
int setup_mode = 0;
int newValue = 0;

// Digit patterns for numbers 0-9 (A-G, common anode: LOW is ON)
const byte numbers[10] = {
  0b11000000, // 0
  0b11111001, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011001, // 4
  0b10010010, // 5
  0b10000010, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000  // 9
};
const byte letters[5] = {
  0b10000110, // E 0
  0b10101111, // r 1
  0b10001100, // p 2
  0b10100011, // o 3
  0b10000111  // t 4
};


void displayNumber(int number) {
  int digits[3] = {number / 100, (number / 10) % 10, number % 10}; // Extract digits
  
  for (int d = 0; d < 3; d++) {
    digitalWrite(digitPins[d], HIGH); // Turn on current digit
    
    for (int s = 0; s < 7; s++) {
      digitalWrite(segmentPins[s], bitRead(numbers[digits[d]], s) ? HIGH : LOW);
    }
    
    delay(5); // Small delay for multiplexing
    digitalWrite(digitPins[d], LOW); // Turn off the digit before moving to the next
  }
}

void displayError() {
  // define words to display
  int word_type[3] = {0, 1, 1}; // 'err' 
  int word_state[3] = {2, 3, 4}; // 'pot'
  for (int i = 0; i < 3; i++) { //print first word
    digitalWrite(digitPins[i], HIGH); // Turn on current digit
    for (int j = 0; j < 7; j++) {
      digitalWrite(segmentPins[j], bitRead(letters[word_type[i]], j) ? HIGH : LOW);
    }
    delay(5); // Small delay for multiplexing
    digitalWrite(digitPins[i], LOW); // Turn off the digit before moving to the next
  }
  delay(2000);
  for (int i = 0; i < 3; i++) { //print second word
    digitalWrite(digitPins[i], HIGH); // Turn on current digit
    for (int j = 0; j < 7; j++) {
      digitalWrite(segmentPins[j], bitRead(letters[word_state[i]], j) ? HIGH : LOW);
    }
    delay(5); // Small delay for multiplexing
    digitalWrite(digitPins[i], LOW); // Turn off the digit before moving to the next
  }
  delay(2000);
}
void setup() {
  // setup pins
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  for (int i = 0; i < 3; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); // Start with all digits OFF
  }

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(push_button, INPUT);
}

  /* two modes of operation
    1. setup mode: while holding down the push button, turning the knob will set the timer(also displayed on the segment display)
    2. releasing the push button will start the timer,
      whenever the timer reaches zero, the relay will activate(or deactivate if already activated) 
      and the timer will reset
  */
void loop() {
// first condition
setup_mode = digitalRead(push_button);
if (setup_mode == HIGH) {
  // read data
  // check if analog singnal is good
  newValue = analogRead(potentiometer);
  delay(2);
  newTimeDifference = map(analogRead(potentiometer), 0, 1023, 0, 120);//map value to 0-120 seconds
  timeDifference = newTimeDifference * 1000;//store in milliseconds
  timeLeft = 0;
  // display data
  displayNumber(newTimeDifference);
}
else {
  // check if time left is zero
  if (timeLeft <= 0) {
    //if timer is zero continue loop
    if (timeDifference <=0 ) {
      return;
    }
    //reset timer
    timeLeft = timeDifference;
    // timer is zero so flip relay status
    relay_status = relay_status == HIGH? LOW : HIGH;
    digitalWrite(LED_BUILTIN, relay_status);
  }
  //time not run out yet
  displayNumber((timeLeft / 1000) + 1);
  timeLeft -= 16;
}
}
