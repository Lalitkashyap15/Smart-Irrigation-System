ARDUINO IDE CODE 
#define BLYNK_TEMPLATE_ID "TMPL3hDJiQRvO" 
#define BLYNK_TEMPLATE_NAME "GreenPulse" 
#define BLYNK_AUTH_TOKEN "Wwaot3OreS1rXbh3m2TYhBDaEeKEiOjb" 
#include <WiFi.h> 
#include <WiFiClient.h> 
#include <BlynkSimpleEsp32.h> 
#include <DHT.h> 
// ----- Pin Configuration ----- 
#define DHTPIN 4 
#define DHTTYPE DHT11 
#define SOIL_PIN 34 
#define RELAY_PIN 27 
 
// ----- Calibration & Thresholds ----- 
const int VAL_AIR = 4095; 
const int VAL_LOW = 2620; 
const int VAL_MED = 1770; 
const int VAL_FULL = 1600; 
const int MOISTURE_THRESHOLD = 50; // Pump turns on below 50% 
const int HUMIDITY_LIMIT = 80; // "Too high" humidity threshold 
// ----- State Variables ----- 
float smoothMoisture = 0; 
float filterWeight = 0.15; 
bool autoMode = false;    
// Controlled by V4 
bool humiditySkipActive = false; // Controlled by V5 
int pumpState = 0; 
char ssid[] = "Redmi"; 
char pass[] = "avm30406"; 
// Current relay status 
char auth[] = BLYNK_AUTH_TOKEN; 
DHT dht(DHTPIN, DHTTYPE); 
BlynkTimer timer; 
// ----- Custom Mapping Function ----- 
int calculateMoisture(int raw) { 
if (raw >= VAL_AIR) return 0; 
if (raw >= VAL_LOW) return map(raw, VAL_AIR, VAL_LOW, 0, 10); 
if (raw >= VAL_MED) return map(raw, VAL_LOW, VAL_MED, 10, 50); 

return constrain(map(raw, VAL_MED, VAL_FULL, 50, 100), 0, 100); 
} 
// ----- Logic Controller ----- 
void runSmartLogic(float currentTemp, float currentHum, float currentMoist) { 
if (autoMode) { 
bool moistureLow = (currentMoist < MOISTURE_THRESHOLD); 
bool humidityTooHigh = (humiditySkipActive && currentHum > HUMIDITY_LIMIT); 
// Decision Engine 
if (moistureLow && !humidityTooHigh) { 
if (pumpState == 0) { 
digitalWrite(RELAY_PIN, HIGH); 
pumpState = 1; 
Blynk.virtualWrite(V3, 1); // Sync the manual button in app 
Serial.println("Auto: Pump ON"); 
}} else { 
if (pumpState == 1) { 
digitalWrite(RELAY_PIN, LOW); 
pumpState = 0; 
Blynk.virtualWrite(V3, 0); // Sync the manual button in app 
Serial.println(humidityTooHigh ? "Auto: Humidity Skip Active - Pump OFF" : "Auto: 
Moisture OK - Pump OFF"); 
} 
} 
} 
}