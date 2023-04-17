/*
 * Project DynamicColorRange_Program
 * Description: A new device that takes two pictures and compares the color change between every pixel. It then creates a new image with the data.
 * Author: Caden Gamache
 * Date: 04/02/2023 - 
 */
#include "credentials.h"

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"

/****************** Global State ******************/ 
TCPClient TheClient;
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/********************* Feeds *********************/  
Adafruit_MQTT_Subscribe sub_image1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/image"); 
//Adafruit_MQTT_Subscribe sub_image2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/image2"); 


/***************************** Declare Variables *****************************/
int i, x, j;

const int REDPIN = A5, GREENPIN = A4, BLUEPIN = A3, BUTTONPIN = A2;
const int BLACK = 0, RED = 1, ORANGE = 2, YELLOW = 3, GREEN = 4, CYAN = 5, BLUE = 6, PURPLE = 7, WHITE = 8;
const int COLOR[] {BLACK, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, WHITE};
float BRIGHTNESS = 10.0;
int currentColor;

int timer;
bool buttonPressed, snapPicture, test;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(REDPIN,OUTPUT);
  pinMode(GREENPIN,OUTPUT);
  pinMode(BLUEPIN,OUTPUT);
  pinMode(BUTTONPIN,INPUT);
  pinMode(D7, OUTPUT);
  pinMode(A0, OUTPUT);

  timer = millis();
}

void loop() {
  buttonPressed = digitalRead(A2);
  if(buttonPressed != snapPicture) {
      if(snapPicture) {
        snapPicture = !snapPicture;
      }
      snapPicture = buttonPressed;
  }
  if (snapPicture) {
    if (snapPicture != test) {
      // First Picture
      digitalWrite(D7,HIGH);
      digitalWrite(A0,HIGH);

      delay(1500);
      digitalWrite(D7,LOW);
      digitalWrite(A0,LOW);

      // Second Picture
      delay(1500);
      digitalWrite(D7,HIGH);
      digitalWrite(A0,HIGH);

      delay(1500);
      digitalWrite(D7,LOW);
      digitalWrite(A0,LOW);

      test = 1;
    }
    //String parse0 = Serial1.readStringUntil('%X',0xFFD9);
    //Serial.printf("%s",parse0.c_str());
  }
  if (snapPicture == 0) {
    if (snapPicture != test) {
      digitalWrite(D7,LOW);
      digitalWrite(A0,LOW);

      test = 0;
    }
  }

  /*********************** Subscriptions ***********************/
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &sub_image1) {
      //image1 = atof((char *)sub_image1.lastread);
    }
  }


  if((millis()-timer) > 5000) {
    timer = millis();
    currentColor++;
    turnLed(COLOR[currentColor], BRIGHTNESS, REDPIN, GREENPIN, BLUEPIN);
    if (currentColor>8) {
      currentColor = 0;
    }
  }
}

void turnLed(int color, float brightnessPercent, int redPin, int greenPin, int bluePin) {
    static int black = 0, red = 1, orange = 2, yellow = 3, green = 4, cyan = 5, blue = 6, purple = 7, white = 8;
    // quick reset, otherwise colors mix ever so often
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

// void colorChange(int *array[][]) {

// }

// void colorChangeRGB(int *array[][]) {

// }