#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//wifi credential
const char* ssid = "ssid";
const char* password = "password";

//mqtt server
const char* mqtt_server = "ip_broker";
const int mqtt_port = 1883;
const char* mqttUser = "user";
const char* mqttPassword = "name";

//define sensor pins
const int phSensorPin = 34; // Analog input pin for the pH sensor
const int tdsSensorPin = 35; // Analog input pin for the tds sensor
const int turbidityPin = 36; // Analog input pin for the turbidity sensor

// GPIO where the DS18B20 is connected to
const int oneWireBus = 5;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  client.setServer(mqtt_server, mqtt_port);
  sensors.begin();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  //temperature sensor
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  
  char message[16];
  snprintf(message, sizeof(message), "%.2f", temperatureC);

  client.publish("temperature", message);

  //turbidity sensor

  int turbidityValue = analogRead(turbidityPin);
  float turbidity = map(turbidityValue, 0, 4095, 0, 1023); // Adjust the mapping based on your sensor
  snprintf(message, sizeof(message), "%.2f", turbidity);

  client.publish("turbidity", message);

  //ph sensor

  int phValue = analogRead(phSensorPin);
  float pH = map(phValue, 0, 1023, 0, 14); // Adjust the mapping based on your sensor
  snprintf(message, sizeof(message), "%.1f", phSensorPin);

  client.publish("ph", message);

  //tds sensor

  int tdsValue = analogRead(tdsSensorPin);
  float tds = map(tdsValue, 0, 4095, 0, 1023); // Adjust the mapping based on your sensor

  snprintf(message, sizeof(message), "%.1f", tds);

  client.publish("tds", message);

  delay(2000); // Publish data every 5 seconds (adjust as needed)
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}