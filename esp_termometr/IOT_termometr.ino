#include <SimpleDHT.h>
#include "connectivity.h"

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;

void callback(String topic, byte* message, unsigned int length) {
  Serial.println("Got a message");
}


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect("ESP8266Client");

  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) 
  {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  Serial.println(temperature);
  Serial.println("LLLLLLLLLLLLLLL");
  
  
  Serial.println("wysylam wiadomosc do serwera");
  client.publish("DOM/temperature", "21");
  delay(10000);
  
}
