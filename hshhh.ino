#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
int weird =4;

void setup() {
  Serial.begin(115200);
  Serial.println("why");
  SerialBT.begin("ESP32_Test"); // Bluetooth device name
  Serial.println("Bluetooth Started. Ready to pair.");
}

void loop() {
  if (SerialBT.available()) {
    // String received = SerialBT.readString();
    // Serial.println("Received: " + received);
    if(weird==0)
      weird=4;
    else
      weird=0;
    SerialBT.print(0+weird);
    SerialBT.print(",");
    SerialBT.print(1+weird);
    SerialBT.print(",");
    SerialBT.print(2+weird);
    SerialBT.print(",");
    SerialBT.print(3+weird);
    SerialBT.print(",");

    SerialBT.println(); // Send back data
  }
}