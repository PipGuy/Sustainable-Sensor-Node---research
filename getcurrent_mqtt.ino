#include <Wire.h>
#include <Adafruit_INA219.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoMqttClient.h>

Adafruit_INA219 ina219_L;
Adafruit_INA219 ina219_EH(0x41);

float BVL = 0;
float BVEH = 0;
float c_mA_L = 0;
float c_mA_EH = 0;
float power_mW_L = 0;
float power_mW_EH = 0;

char ssid[] = "HUAWEI-U3uy";        // your network SSID (name)
char pass[] = "Limaes01";    // your network password (use for WPA, or use as key for WEP)
const long utcOffsetInSeconds = 7*3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic[] = "ehmeasure";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
 
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  WiFi.mode(WIFI_STA);
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
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  digitalWrite(LED_BUILTIN, HIGH);

  //ina219.setCalibration_32V_1A();
  if (!ina219_L.begin()||!ina219_EH.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
 

  timeClient.begin(); 
  Serial.print("Timestamp,");
  Serial.print("Voltage L,");
  Serial.print("Current L,");
  Serial.println("Power L,");  
  Serial.print("Voltage EH,");
  Serial.print("Current EH,");
  Serial.println("Power EH");
}

void loop() {

  mqttClient.poll();
  BVL = ina219_L.getBusVoltage_V();
  BVEH = ina219_EH.getBusVoltage_V();
  c_mA_L = ina219_L.getCurrent_mA();
  c_mA_EH = ina219_EH.getCurrent_mA();
  power_mW_L = ina219_L.getPower_mW();
  power_mW_EH = ina219_EH.getPower_mW();
  //loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(" ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.print(timeClient.getSeconds());
  Serial.print(",");
  Serial.print(BVL);
  Serial.print(";");
  Serial.print(BVEH);
  Serial.print(";");
  Serial.print(c_mA_L);
  Serial.print(";");
  Serial.print(c_mA_EH);
  Serial.print(";");
  Serial.print(power_mW_L);
  Serial.print(";");
  Serial.println(power_mW_EH);
String payload = String(daysOfTheWeek[timeClient.getDay()]) + ", " + timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds() + ";"+ String(BVL) + ";" 
                  + String(BVEH) + ";" + String(c_mA_L) + ";" + String(c_mA_EH) + ";" + String(power_mW_L) + ";" + String(power_mW_EH);
   
  
  mqttClient.beginMessage(topic);
   mqttClient.print(payload);
  mqttClient.endMessage();
  timeClient.update();

 

  delay(1000);
}