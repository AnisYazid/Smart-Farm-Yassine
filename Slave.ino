#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <Adafruit_GFX.h>

#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <esp_now.h>

#include <ESP32Time.h>
#include "DHT.h"
#include <Wire.h>
#define uS_TO_S_FACTOR 500000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 10         /* Time ESP32 will go to sleep (in seconds) */

#define DHTPIN 19 
#define DHTTYPE DHT11 
#define capteur_nv 34
#define moil_sensor 35
#define relay 32
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


uint8_t broadcastAddress[] = {0x34, 0xAB, 0x95, 0x8F, 0x77, 0x78};//adresse mac du receiver

typedef struct struct_message 
{
  float temperature;
  float humidite;
  float niveau_deau;
  float moisture;
  char noeud='2';// numero du noeud,chaque noeud a un numero
} struct_message;

struct_message incomingReadings;  

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println("Going to sleep now"); 
  esp_deep_sleep_start();
}
 
void setup() 
{
  Serial.begin(115200);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +" Seconds");
  
}
 
void loop() {
   delay(1000);
   incomingReadings.niveau_deau=(analogRead( capteur_nv )/255);
   incomingReadings.temperature = dht.readTemperature();
   incomingReadings.humidite = dht.readHumidity();
   incomingReadings.moisture = ((analogRead(moil_sensor)*100)/1000);
   
   esp_now_send(broadcastAddress, (uint8_t *) &incomingReadings, sizeof(incomingReadings));

  

   if(incomingReadings.niveau_deau>750 &&  incomingReadings.moisture<25){
    digitalWrite(relay, HIGH);
    if(incomingReadings.moisture > 75 && incomingReadings.niveau_deau>750 ){
      digitalWrite(relay,LOW);
    }
   }
   else{
    digitalWrite(relay,LOW);
   }

   // clear display
  display.clearDisplay();
  
  // display temperature
  display.setTextSize(0.4);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(0.4);
  display.setCursor(0,8);
  display.print(incomingReadings.temperature);
  display.print(" ");
  display.setTextSize(0.4);
  display.cp437(true);
  display.write(167);
  display.setTextSize(0.4);
  display.print("C");
  
  // display humidity
  display.setTextSize(0.4);
  display.setCursor(0, 15);
  display.print("Humidity: ");
  display.setTextSize(0.4);
  display.setCursor(0, 24);
  display.print(incomingReadings.humidite);
  display.print(" %"); 

  //display water level
  display.setTextSize(0.4);
  display.setCursor(0, 32);
  display.print("water level: ");
  display.setTextSize(0.4);
  display.setCursor(0, 40);
  display.print(incomingReadings.niveau_deau);
  display.print(" %"); 

  //display soil mosture
  display.setTextSize(0.4);
  display.setCursor(0, 48);
  display.print("Soil mosture: ");
  display.setTextSize(0.4);
  display.setCursor(0, 56);
  display.print(incomingReadings.moisture);
  display.print(" %"); 
  
  
  
  display.display(); 
   
}