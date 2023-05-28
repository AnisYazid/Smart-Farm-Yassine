#include <Arduino.h>
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
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyAVu1lwgL9iJ_l9c2AhjRE6GGIbT6vjCEE "
#define DATABASE_URL "https://green-iot-73ad5-default-rtdb.firebaseio.com/"



#include <esp_now.h>

#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>

const char* ssid = "Galaxy M11c02c";
const char* password = "rqpk3385";
FirebaseData fbdo;
 
FirebaseAuth auth;
FirebaseConfig config;
  unsigned long sendDataPrevMillis = 0;
  int count = 0;
  bool signupOK = false;

typedef struct struct_message {
  float temperature;
  float humidite;
  float niveau_deau;
  float moisture;
  char noeud;// numero du noeud,chaque noeud a un numero

} struct_message;

struct_message incomingReadings;

JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  board["noeud"] = incomingReadings.noeud;
  board["temperature"] = incomingReadings.temperature;
  board["humidite"] = incomingReadings.humidite;
  board["niveau_deau"] = incomingReadings.niveau_deau;
  board["moisture"] = incomingReadings.moisture;
 
  
  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("Board ID %u: %u bytes\n", incomingReadings.noeud, len);
  Serial.printf("t value: %0.2f \n", incomingReadings.temperature);
  Serial.printf("h value: %0.2f \n", incomingReadings.humidite);
  Serial.printf("nv eau value: %0.2f \n", incomingReadings.niveau_deau);
  Serial.printf("moisture value: %0.2f \n", incomingReadings.moisture);
  Serial.println();

  
}



void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial.println(WiFi.localIP());
  
  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());


  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

 
   
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
   config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
 
void loop() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
     

  if (Firebase.ready() == false) {
    Serial.println("Firebase not ready!");
    return;
  }

  if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/temperature", incomingReadings.temperature)) {
    Serial.print("Temperature: ");
    Serial.println(incomingReadings.temperature);
  } else {
    Serial.println("Error pushing temperature to Firebase");
  }

  if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/humidity", incomingReadings.humidite)) {
    Serial.print("Humidity: ");
    Serial.println(incomingReadings.humidite);
  } else {
    Serial.println("Error pushing humidity to Firebase");
  }

  if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/Waterlevel", incomingReadings.niveau_deau)) {
    Serial.print("Water level: ");
    Serial.println(incomingReadings.niveau_deau);
    delay(100);
  } else {
    Serial.println("Error pushing temperature to Firebase");
  }
delay(2000);

  if (Firebase.RTDB.setFloat(&fbdo, "/sensorData/moisture", incomingReadings.moisture)) {
    Serial.print("Moisture: ");
    Serial.println(incomingReadings.moisture);
    delay(100);
  } else {
    Serial.println("Error pushing Moisture to Firebase");
  }
  }
  }
  
}