#include <Servo.h>
#include "connectivity.h"


const int oven_off = 70; // oven off servo position /water heating on
const int oven_on = 110;
const int oven_perm_off = 90; // power off servo position
const int servoPin = 5;
const int led_G = 12; // green led
const int led_R = 15; // red led
const int led_B = 13; // blue led
bool oven_working = true;
bool need_restart = false;
int oven_time = 0; // counts how long does the oven have to work (*6s)
long now = millis();
long lastMeasure = 0;
long lastTelSend = 0;
long lastHeartbeat = 0;
const int telSendInterval = 12000; // in ms
const int heartbeatInterval = 3000;


void set_servo(Servo serv, int pos)
{
  serv.attach(servoPin);
  serv.write(pos);
  delay(2000);
  serv.detach();
}

void blink_led(int n){
  for(int i=0; i<n; i++){
        digitalWrite(led_G, HIGH);
        delay(50);
        digitalWrite(led_G, LOW);
        delay(100);
       }
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
     else if(messageTemp == "reset"){
       need_restart = true;
       Serial.print("restarting");
     }
     else if(messageTemp != NULL) // in case of turning the oven on for a specific time period
     {
       oven_time = messageTemp.toInt();
       Serial.println(oven_time);
     }
   }
  Serial.println();
}

Servo myservo; // creating servo object

// GPIO, serial and wifi setup
void setup() {
  pinMode(led_G, OUTPUT);
  pinMode(led_R, OUTPUT);
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  set_servo(myservo, oven_off);
}

char * toArray(int number)
    {
        int n = log10(number) + 1;
        int i;
      char numberArray[10]; 
        for ( i = 0; i < n; ++i, number /= 10 )
        {
            numberArray[i] = number % 10;
        }
        return numberArray;
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
    if(need_restart == true){
      need_restart = false;
      blink_led(5);
      set_servo(myservo, oven_perm_off);
      set_servo(myservo, oven_off);
      oven_working = false;
      client.publish("DOM/feedback", "ovenRestarted");
    }
    if(oven_time > 0)
      {
        if(!oven_working){
          digitalWrite(led_G, HIGH);
          set_servo(myservo, oven_on);
          client.publish("DOM/feedback", "ovenOn");
          oven_working = true;
        }
        oven_time--;
      }
    else
    {
     if(oven_working){
         digitalWrite(led_G, LOW);
         set_servo(myservo, oven_off);
         client.publish("DOM/feedback", "ovenOff");
         oven_working = false;
       }
    }
    if(now-lastTelSend > telSendInterval){
      //char msg[30] = {oven_time, oven_working};
      char msg[30];
      itoa(oven_time, msg, 10);

      client.publish("DOM/status", msg);
      lastTelSend = now;
    }
  }
  if(now-lastHeartbeat > heartbeatInterval){
    int i=0;
    // add logic to activate led_R when connection is down
    while(i < 255){
      analogWrite(led_B, i);
      i += 1+i/20;
      delay(30);
    }
    while(i > 0){
      analogWrite(led_B, i);
      i -= 1+i/20;
      delay(30);
    }
    analogWrite(led_B, 0);
    lastHeartbeat = millis();
  }
 /* for(int i=0; i<205; i++){
    analogWrite(led_R, i);
    delay(20);
  }
  for(int i=205; i>0; i--){
    analogWrite(led_R, i);
    delay(20);
  }*/ 
}

