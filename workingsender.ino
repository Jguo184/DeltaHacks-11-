#include <DHT.h>

/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>
#include "sensirion_common.h"
#include "sgp30.h"
#define DHTPIN 33
#define DHTTYPE DHT11
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0xCC, 0xDB, 0xA7, 0x30, 0xB1, 0xCC};

DHT dht(DHTPIN, DHTTYPE);
typedef struct test_struct {
  int x;
  int y;
  int z;
  int f;
} test_struct;

test_struct test;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void co2(){

 s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;
    Serial.begin(115200);
    Serial.println("serial start!!");

    /*For wio link!*/
    #if defined(ESP8266)
    pinMode(15, OUTPUT);
    digitalWrite(15, 1);
    Serial.println("Set wio link power!");
    delay(500);
    #endif
    /*  Init module,Reset all baseline,The initialization takes up to around 15 seconds, during which
        all APIs measuring IAQ(Indoor air quality ) output will not change.Default value is 400(ppm) for co2,0(ppb) for tvoc*/
    while (sgp_probe() != STATUS_OK) {
        Serial.println("SGP failed");
        while (1);
    }
    /*Read H2 and Ethanol signal in the way of blocking*/
    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                            &scaled_h2_signal);
    if (err == STATUS_OK) {
        Serial.println("get ram signal!");
    } else {
        Serial.println("error reading signals");
    }
    err = sgp_iaq_init();

}
 
void setup() {

  co2();
  pinMode(25,OUTPUT);
  dht.begin();
  digitalWrite(25, HIGH);
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}
 
void loop() {

    s16 err = 0;
    u16 tvoc_ppb, co2_eq_ppm;
    err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
    if (err == STATUS_OK) {
      
      test.x = tvoc_ppb;
      test.y = co2_eq_ppm;
      test.z = dht.readTemperature();
      test.f = dht.readHumidity();

    } else {
        Serial.println("error reading IAQ values\n");
    }

  esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  Serial.println(test.x);
    Serial.println(test.y);
    Serial.println(test.z);
    Serial.println(test.f);


  delay(2000);
}