#include <Servo.h>
#include "connectivity.h"



const int oven_off = 70; // oven off servo position
const int oven_on = 110;
const int servoPin = 5;
const int lamp = 12; // led indicator
bool oven_working = true;
int oven_time = 0; // counts how long does the oven have to work (*6s)
long now = millis();
long lastMeasure = 0;

Servo myservo; // creating servo object

void set_servo(int pos)
{
  myservo.attach(servoPin);
  myservo.write(pos);
  delay(2000);
  myservo.detach();
}


// executes when someone publishes a message in subscribed topic
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(topic=="DOM/piec"){
     Serial.print("Received command:  ");
     if(messageTemp == "on"){
       oven_time = 600;  // turns the oven on for 1 hour
       Serial.print("On");
     }
     else if(messageTemp == "off"){
       oven_time = 0;
       Serial.print("Off");
     }
     else if(messageTemp != NULL) // in case of turning the oven on for a specific time period
     {
       oven_time = messageTemp.toInt();
     }
   }
  Serial.println();
}



// GPIO, serial and wifi setup
void setup() {
  pinMode(lamp, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  set_servo(oven_off);
}


void loop() {
  //check_wifi();              // checks
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (now - lastMeasure > 6000) {
    lastMeasure = now;
    if(oven_time > 0)
      {
        if(!oven_working){
          digitalWrite(lamp, HIGH);
          set_servo(oven_on);
          client.publish("DOM/feedback", "ovenOn");
          oven_working = true;
        }
        
        oven_time--;
      }
     else
     {
      if(oven_working){
          digitalWrite(lamp, LOW);
          set_servo(oven_off);
          client.publish("DOM/feedback", "ovenOff");
          oven_working = false;
        }
     }
  }
} 
