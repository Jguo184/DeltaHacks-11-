/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <esp_now.h>
#include <WiFi.h>

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

const uint8_t   OLED_pin_scl_sck        = 18;
const uint8_t   OLED_pin_sda_mosi       = 23;
const uint8_t   OLED_pin_cs_ss          = 17;
const uint8_t   OLED_pin_res_rst        = 5;
const uint8_t   OLED_pin_dc_rs          = 16;

#define	BLACK           0x0000
#define WHITE           0xFFFF
#define	RED             0xF800

Adafruit_SSD1331 oled =
    Adafruit_SSD1331(
        OLED_pin_cs_ss,
        OLED_pin_dc_rs,
        OLED_pin_sda_mosi,
        OLED_pin_scl_sck,
        OLED_pin_res_rst
     );

int buzzer = 4;
int count =0;


int co2threshold = 500;
int tvocthreshold = 2000;
int moithreshold = 60;
int tempthreshold = 28;

const int lengththreshold = 3;



//array

int co2Data[lengththreshold] = {0};
int tvocData[lengththreshold] = {0};

double co2avg=0;

double tvocavg=0;
//Structure example to receive data
//Must match the sender structure
typedef struct test_struct {
  int x;
  int y;
  int z;
  int f;

} test_struct;

//Create a struct_message called myData 
test_struct myData;

void displayData(){

   oled.setTextColor(RED);
   oled.setTextSize(1);
   oled.setCursor(6, 15);
   oled.print("CO2:");
   oled.fillRect(0, 25, 90, 20, WHITE);
   oled.setTextColor(BLACK);

   oled.setCursor(36, 25);
   oled.setTextSize(2);
   oled.print(co2avg);
  

}
//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len != sizeof(myData)) {
    Serial.println("Invalid data size");
    return;
  }
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.print("z: ");
  Serial.println(myData.z);
  Serial.print("f: ");
  Serial.println(myData.f);
  Serial.println();

  if (myData.z >= tempthreshold|| myData.f>= moithreshold) {
      analogWrite(buzzer, 60);
      delay(100); // Allow buzzer to sound briefly
    }
    analogWrite(buzzer, 0);

 if (count < lengththreshold) {
    co2Data[count] = myData.x;
    tvocData[count] = myData.y;
    count++;
  }

  if (count == lengththreshold) {
    co2avg = 0;
    tvocavg = 0;

    for (int j = 0; j < lengththreshold; j++) {
      co2avg += co2Data[j];
      tvocavg += tvocData[j];
    }

    co2avg /= lengththreshold;
    tvocavg /= lengththreshold;

    Serial.printf("CO2 Avg: %.2f\nTVOC Avg: %.2f\n", co2avg, tvocavg);

    displayData();

    if (tvocavg >= tvocthreshold || co2avg >= co2threshold) {
      analogWrite(buzzer, 60);
      delay(100); // Allow buzzer to sound briefly
    }
    analogWrite(buzzer, 0);

    // Reset for next batch
    memset(co2Data, 0, sizeof(co2Data));
    memset(tvocData, 0, sizeof(tvocData));
    count = 0;
  }
}
 


void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  pinMode (buzzer, OUTPUT);
    oled.begin();
    //oled.fillScreen(WHITE);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }


  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));




}
 
void loop() {
}