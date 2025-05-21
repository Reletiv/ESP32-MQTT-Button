#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi and MQTT credentials
const char* ssid = "xx";
const char* password = "xx";
const char* mqtt_server = "xx";  // Your MQTT broker IP
const char* mqtt_user = "xx";
const char* mqtt_pass = "xx";
const char* mqtt_topic = "xx";

WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi() {
  //Serial.println("Connecting to Wi-Fi...");

  // Fast boot + less flash wear
  WiFi.persistent(false);         
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // Optional: faster connect

  IPAddress local_IP(192, 168, 20, xxx);
  IPAddress gateway(192, 168, 20, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);

  uint8_t bssid[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  WiFi.begin(ssid, password, 0, bssid, true); // BSSID lock + skip scan

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 300) {
    delay(10);
    //Serial.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println("\nWi-Fi connected");
    //Serial.print("IP: ");
    //Serial.println(WiFi.localIP());
  } else {
    //Serial.println("\nWi-Fi failed");
  }
}

void connectToMQTT() {
  client.setServer(mqtt_server, 1883);
  //Serial.println("Connecting to MQTT...");
  int retries = 0;
  while (!client.connected() && retries < 50) {
    if (client.connect("D1_Button", mqtt_user, mqtt_pass)) {
      //Serial.println("MQTT connected");
    } else {
      //Serial.print("MQTT failed, rc=");
      //Serial.println(client.state());
      delay(200);
      retries++;
    }
  }
}

void setup() {
  //Serial.begin(115200);
  //delay(100); // Let the power settle
  //Serial.println("\n--- WeMos D1 Mini MQTT Button Boot ---");

  connectToWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    connectToMQTT();

    if (client.connected()) {
      //Serial.println("Publishing button press...");
      client.publish(mqtt_topic, "pressed");
      delay(300); // Ensure publish completes
      client.publish(mqtt_topic, "released");
      delay(500); // Ensure publish completes
      //Serial.println("Published!");
    } else {
      //Serial.println("MQTT not connected");
    }
  }

  //Serial.println("Going to deep sleep...");
  //Serial.println(WiFi.BSSIDstr());
  ESP.deepSleep(0);  // External wake via RST
}

void loop() {
  // Not used — we sleep after setup
}