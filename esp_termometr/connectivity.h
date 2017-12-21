#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "UPCFD833CA"; // wifi ssid TP-LINK_6162
const char* password = "vkjmfTmhppy2"; // wifi password sonet1001
const char* mqtt_server = "192.168.0.106"; // raspberry IP

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);


// connect to WiFi network
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void check_wifi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi disconnected");
    setup_wifi();
  }
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp_temperature")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      client.subscribe("DOM/termometr");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 seconds");
      delay(10000);
    }
  }
}
