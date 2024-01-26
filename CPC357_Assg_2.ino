#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define PIR_PIN A4
#define LED_PIN 40
#define IR_PIN 39
#define SERVO_PIN 38

const char *WIFI_SSID = "cs-mtg-room";
const char *WIFI_PASSWORD = "bilik703";
const char *MQTT_SERVER = "34.142.152.99";
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "smartheater";

WiFiClient espClient;
PubSubClient client(espClient);
Servo servo;

// Helper function to connect WiFi
void setupWifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Setup different modes for the different pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  Serial.begin(9600);

  // Connect to WiFi
  setupWifi();
  // Connect to MQTT server
  client.setServer(MQTT_SERVER, MQTT_PORT);

  // Setting up the servo motor
  servo.attach(38);
  servo.write(0);
}

void reconnect()
{
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT server");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  // Validation to make sure WiFi connection.
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  // Read PIR Sensor
  int pirReading = digitalRead(PIR_PIN);
  // Read IR Sensor
  int irReading = digitalRead(IR_PIN);

  // Boolean for Motor spin
  int motorSpin = 0;

  if (pirReading == HIGH && irReading == LOW) {
    digitalWrite(40, HIGH);
    servo.write(180);
    motorSpin = 1;

    // Delay some time before detecting whether to turn off or not
    delay(2500);
  } else {
    digitalWrite(40, LOW);
    servo.write(0);
    motorSpin = 0;
  }

  // Format data into JSON format to store in database later
  char payload[100];
  sprintf(payload, "{\"PIR_Reading\":%d, \"IR_Reading\":%d,  \"Motor_Spin\":%d}", pirReading, irReading, motorSpin);
  client.publish(MQTT_TOPIC, payload);
  // Print out the payload
  Serial.println(payload);

  // Some buffer time before performing detection again
  delay(500);
}
