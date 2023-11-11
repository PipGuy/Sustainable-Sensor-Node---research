#include <PubSubClient.h>
#include <ESP8266WiFi.h>


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
const char* ssid = "HUAWEI-U3uy";        // your network SSID (name)
const char* pass = "Limaes01";    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
PubSubClient client(wifiClient);

const char* mqtt_server = "test.mosquitto.org";
//int        port     = 1883;
//const char topic[]  = "testardugw";
//int        count = 1;


//set interval for sending messages (milliseconds)
const long interval = 10000;
unsigned long previousMillis = 0;

int counter = 0;
bool txstatus;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("ESP8266Client", "", "")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.println(client.state());
      delay(1000);
    }
  }
}


void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  //mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
int Rvalue = counter;
  

    Serial.print("Sending message to topic: ");
    Serial.println("testardugw");
    Serial.println(Rvalue); 
 

    // send message, the Print interface can be used to set the message contents
    txstatus = client.publish("testardugw", "test_hello");
    char buffer[40];
    sprintf(buffer, "Publish status %i ", txstatus); 
    Serial.println(buffer);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(50);                    
    //mqttClient.endMessage();
    
  delay(5000); // Wait for 10 seconds
   
  
 

    Serial.println();
   counter = counter + 1;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages
}


