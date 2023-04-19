/*
 * Project Dyanmic Color Range Device
 * Description: Takes two pictures and sends them to Adafruit on two seperate feeds
 * Author: Caden Gamache
 * Date: 04/14/2023 - 04/19/2023
`*/
#include <WiFi.h>

#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// These are used for image publication to Adafruit dashboard
#include "credentials.h"
#include <PubSubClient.h>
#include <base64.h>

/******************************** Declare Variables ********************************/
// Wifi
const char* ssid = "DDCIOT";
const char* password = "ddcIOT2020";

// Adafruit feeds for publishing
const char* mqttTopicSnap = "Mr_Caden/feeds/image";
const char* mqttTopicSnap2 = "Mr_Caden/feeds/image2";
const char* mqttTopicStream = "Mr_Caden/feeds/currentroom";
const int MAX_PUBLISH = 50 * 1024;    // Adafruit's limit is 100 kB

// Image storage
char strOut[1024];  // Image 1 to Adafruit storage (1kB limit when Dashboard control history is on)
char strOut2[1024];  // Image 2 to Adafruit storage(1kB limit when Dashboard control history is on)
String buffer, buffer2;      // Adafruit image publication buffers (100kB limit when Dashboard control history is off)

/******************************** Create Objects ********************************/
WiFiClient espClient;
PubSubClient client(espClient);

/******************************** Create Prototypes ********************************/
void mqtt_setup();
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(9600); 
  pinMode(14,INPUT);

  Serial.setDebugOutput(true);
  Serial.println("Serial is up!");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_VGA;          // limited for 100 kB Adafruit limit for publishing
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_DRAM;
  config.jpeg_quality = 10;                   // (0-63) higher numbers are lower quality
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  sensor_t * s = esp_camera_sensor_get();
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop() {
  mqtt_setup();     // Refresh MQTT signal to keep alive
  static camera_fb_t *fb = NULL;
  // Snaps the first picture
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    client.publish(mqttTopicStream, "Camera capture failed for image 1\0");
    return;
  }

  // Encodes incoming images to 64 bit
  buffer = base64::encode((uint8_t *)fb->buf, fb->len);
  sprintf(strOut, "Got frame: %d x %d (%d/%d)\0", fb->width, fb->height, fb->len, buffer.length());
  client.publish(mqttTopicStream, strOut);
  Serial.printf("%s\n", strOut);
  // Checks if the first picture outputted values and that the size is within the maximum limit
  if (buffer.length() < MAX_PUBLISH) {
    if (client.publish(mqttTopicSnap, buffer.c_str()))
    {
      Serial.print("Published Image 1 to ");
      Serial.print(mqttTopicSnap);
      Serial.printf("\n");
      client.publish(mqttTopicStream, "Published image 1!\0");
      Serial.printf("Published %d bytes (from %d)\n", buffer.length(), fb->len);
    }
    else
    {
      Serial.println("Error Publishing Image");
      client.publish(mqttTopicStream, "Error publishing image 1...\0");
    }
  } else {
    client.publish(mqttTopicStream, "Over limit on image 1 - We'll try to publish the next pic\0");
  }
  // Resets buffer 1
  esp_camera_fb_return(fb);
  buffer.clear();

  delay(2000);

  // Snaps the scond picture
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed for image 2");
    client.publish(mqttTopicStream, "Camera capture of image 2 failed\0");
    return;
  }
  // Checks if the second picture outputted values and that the size is within the maximum limit
  buffer2 = base64::encode((uint8_t *)fb->buf, fb->len);
  sprintf(strOut2, "Got frame2: %d x %d (%d/%d)\0", fb->width, fb->height, fb->len, buffer2.length());
  client.publish(mqttTopicStream, strOut2);
  Serial.printf("%s\n", strOut2);
  if (buffer2.length() < MAX_PUBLISH) {
    if (client.publish(mqttTopicSnap2, buffer2.c_str())) {
      Serial.print("Published Image 2 to ");
      Serial.print(mqttTopicSnap2);
      Serial.printf("\n");
      client.publish(mqttTopicStream, "Published image 2!\0");
      Serial.printf("Published2 %d bytes (from %d)\n", buffer2.length(), fb->len);
    }
    else {
      Serial.println("Error Publishing Image2");
      client.publish(mqttTopicStream, "Error publishing image 2...\0");
    }
  } 
  else {
    client.publish(mqttTopicStream, "Over limit on image 2 - We'll try to publish the next pic\0");
  }
  // Resets buffer 2
  esp_camera_fb_return(fb);
  buffer2.clear();

  // Traps the loop here until a a high signal is read on pin 14
  while(digitalRead(14) == LOW) {
    digitalRead(14);
    // Continuously updates Image 1, otherwise exposure and other settings get erased giving poor results
    static camera_fb_t *fb = NULL;
    fb = esp_camera_fb_get();
    buffer = base64::encode((uint8_t *)fb->buf, fb->len);
    esp_camera_fb_return(fb);

    // Continuously updates Image 2, otherwise exposure and other settings get erased giving poor results
    fb = esp_camera_fb_get();
    buffer2 = base64::encode((uint8_t *)fb->buf, fb->len);

    delay(10);

    // Resets both image buffers
    esp_camera_fb_return(fb);
    buffer.clear();
    buffer2.clear();
  }
  delay(2000);
}

/******************************** Define Functions *********************************/

void mqtt_setup() {
  static int MQTT_BUFFERSIZE = 50 * 1024;
  // Buffersize is set lower based on successful publish history
  client.setBufferSize((uint16_t)MQTT_BUFFERSIZE);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  Serial.println("Connecting to MQTT…");

  while (!client.connected()) {        
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
      Serial.println("connected");
    } 
    else {
      Serial.print("failed with state  ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

    Serial.print("Message arrived in topic: ");
    Serial.println(topic);

    String byteRead = "";
    Serial.print("Message: ");
    for (int i = 0; i < length; i++) {
      byteRead += (char)payload[i];
    }    
    Serial.println(byteRead);
}
