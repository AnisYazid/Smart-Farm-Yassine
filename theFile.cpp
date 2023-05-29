#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>



char auth[] = "";
char ssid[] = "";///Enter your wifi name
char pass[] = "";// Enter wifi password


//pin number
const int soil_pin = 14;
const int relay_pin = ;
const int dht_pin = ;
const int POWER_PIN = ;
const int SIGNAL_PIN = ;

//sensor variables
int soil = 0 ;
float temp = 0; //temperature
float humidity = 0;
int Water = 0;

//oled
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//dht
#define DHTTYPE DHT11
DHT dht(dht_pin, DHTTYPE);

//blynk
#define BLYNK_TEMPLATE_ID "TMPLdPFtvgES"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
BlynkTimer timer;

void readDHT()
{
  temp = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
  humidity = dht.readHumidity();

  if ( isnan(t) || isnan(humidity)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }else{
    Blynk.virtualWrite(V0, temp); 
    Blynk.virtualWrite(V3, humidity); 
  }
}

void readSoil(){ //humidity
  soil= analogRead(soil_pin);
  soil = map(value,4095,0,0,100);
  Blynk.virtualWrite(V1, soil); 
}

void readWater() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  Wat = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF
  Serial.print("The water sensor value: ");
  Serial.println(Wat);
  Blynk.virtualWrite(V2, Wat);
}

//turning relay on or off
void onRelay(){
    digitalWrite(relay_pin, HIGH);
}
void offRelay(){
    digitalWrite(relay_pin, LOW);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);     
  dht.begin();
  //oled 
    
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  //    

  timer.setInterval(1000L, sendSensor);
  //insert pin mode in or out here pls
  pinMode(soil_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  pinMode(dht_pin, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(SIGNAL_PIN, OUTPUT);


}

void loop()
{
  //read sensors
  readDHT();
  readWater();
  readSoil();
  


  Blynk.run();
  timer.run();
  
  //oled display
  display.clearDisplay();
  
  // display temperature
  display.setTextSize(0.4);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(0.4);
  display.setCursor(0,8);
  display.print(incomingReadings.temp);
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
  display.print(incomingReadings.humidity);
  display.print(" %"); 

  //display water level
  display.setTextSize(0.4);
  display.setCursor(0, 32);
  display.print("water level: ");
  display.setTextSize(0.4);
  display.setCursor(0, 40);
  display.print(incomingReadings.Wat);
  display.print(" %"); 

  //display soil mosture
  display.setTextSize(0.4);
  display.setCursor(0, 48);
  display.print("Soil mosture: ");
  display.setTextSize(0.4);
  display.setCursor(0, 56);
  display.print(incomingReadings.soil);
  display.print(" %"); 
  display.display();  
}
