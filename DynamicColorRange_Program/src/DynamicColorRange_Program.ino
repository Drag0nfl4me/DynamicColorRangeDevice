/*
 * Project DynamicColorRange_Program
 * Description: Sends a signal to an ESP32 and flashes a colored LED which outputs two different pictures. 
 * Author: Caden Gamache
 * Date: 04/02/2023 - 04/19/2023
 */
SYSTEM_MODE(SEMI_AUTOMATIC);

/***************************** Declare Variables *****************************/
// General for loop variables
int i, j;

// Pins
const int REDPIN = A5, GREENPIN = A4, BLUEPIN = A3, BUTTONPIN = A2, CAMERAPIN = A0, ARGONLEDPIN = D7;

// Color declaration and array storage
const int BLACK = 0, RED = 1, ORANGE = 2, YELLOW = 3, GREEN = 4, CYAN = 5, BLUE = 6, PURPLE = 7, WHITE = 8;
const int COLOR[] {BLACK, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE};
int currentColor = 1;

// Brightness (Percent)
float BRIGHTNESS = 100.0;

// Button setup
bool buttonPressed, snapPicture, repeatBlock;

/****************************** Create Prototypes *****************************/
// Takes in a chosen color (8 total) and brightness and sends the proper voltage to the three pins of a 3W LED to get the desired color
void turnLed(int color, float brightnessPercent, int redPin, int greenPin, int bluePin);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(REDPIN,OUTPUT);
  pinMode(GREENPIN,OUTPUT);
  pinMode(BLUEPIN,OUTPUT);
  pinMode(BUTTONPIN,INPUT);
  pinMode(ARGONLEDPIN, OUTPUT);
  pinMode(CAMERAPIN, OUTPUT);
}

void loop() {
  buttonPressed = digitalRead(A2);
  // Button toggle logic
  if(buttonPressed != snapPicture) {
    if(snapPicture) {
      snapPicture = !snapPicture;
    }
    snapPicture = buttonPressed;
  }
  if (snapPicture) {
    if (snapPicture != repeatBlock) { // This repeatBlock, if statement ensures the code won't repeated with every loop
      // First Picture
      turnLed(BLACK, BRIGHTNESS, REDPIN, GREENPIN, BLUEPIN);
      digitalWrite(ARGONLEDPIN,HIGH);
      digitalWrite(CAMERAPIN,HIGH);  // Sends a high output to ESP32 which it will read and take two pictures

      delay(1500);
      digitalWrite(D7,LOW);
      digitalWrite(CAMERAPIN,LOW);   // Stops sending the "take picture" signal

      // Second Picture (Just visual, timed with ESP32's second picture code)
      turnLed(COLOR[currentColor], BRIGHTNESS, REDPIN, GREENPIN, BLUEPIN);
      delay(1500);
      digitalWrite(ARGONLEDPIN,HIGH);

      delay(1500);
      digitalWrite(ARGONLEDPIN,LOW);
      turnLed(BLACK, BRIGHTNESS, REDPIN, GREENPIN, BLUEPIN);

      repeatBlock = 1;  // Sets repeatBlock to true
      // Cycles through a color array with ever call
      currentColor++;
      if(currentColor>8) {
        currentColor = 1;
      }
    }
  }
  // Sets everything to an idle state
  if (snapPicture == 0) {
    if (snapPicture != repeatBlock) {
      digitalWrite(ARGONLEDPIN,LOW);
      digitalWrite(CAMERAPIN,LOW);

      repeatBlock = 0;
    }
  }
}

void turnLed(int color, float brightnessPercent, int redPin, int greenPin, int bluePin) {
  static int black = 0, red = 1, orange = 2, yellow = 3, green = 4, cyan = 5, blue = 6, purple = 7, white = 8;
  // Quick reset, otherwise colors mix ever so often
  analogWrite(redPin, LOW);
  analogWrite(greenPin, LOW);
  analogWrite(bluePin, LOW);

  if(color == black) {
    analogWrite(redPin, LOW);
    analogWrite(greenPin, LOW);
    analogWrite(bluePin, LOW);
    //Serial.printf("Current LED color = black     \r");
  }
  if(color == red) {
    analogWrite(redPin, 255.0/(101.0-brightnessPercent));
    analogWrite(greenPin, LOW);
    analogWrite(bluePin, LOW);
    //Serial.printf("Current LED color = red       \r");
  }
  if(color == orange) {
    analogWrite(redPin, 235.0/(101.0-brightnessPercent));
    analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
    analogWrite(bluePin, LOW);
    //Serial.printf("Current LED color = orange     \r");
  }
  if(color == yellow) {
    analogWrite(redPin, 200.0/(101.0-brightnessPercent));
    analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
    analogWrite(bluePin, LOW);
    //Serial.printf("Current LED color = yellow     \r");
  }
  if(color == green) {
    analogWrite(redPin, LOW);
    analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
    analogWrite(bluePin, LOW);
    //Serial.printf("Current LED color = green     \r");
  }
  if(color == cyan) {
    analogWrite(redPin, LOW);
    analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
    analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
    //Serial.printf("Current LED color = cyan     \r");
  }
  if(color == blue) {
    analogWrite(redPin, LOW);
    analogWrite(greenPin, LOW);
    analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
    //Serial.printf("Current LED color = blue     \r");
  }
  if(color == purple) {
    analogWrite(redPin, 15.0/(101-brightnessPercent));
    analogWrite(greenPin, LOW);
    analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
    //Serial.printf("Current LED color = purple     \r");
  }
  if(color == white) {
    analogWrite(redPin, 145.0/(101.0-brightnessPercent));
    analogWrite(greenPin, 255.0/(101.0-brightnessPercent));
    analogWrite(bluePin, 255.0/(101.0-brightnessPercent));
    //Serial.printf("Current LED color = white     \r");
  }
}
