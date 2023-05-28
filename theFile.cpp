#define BLYNK_TEMPLATE_ID "TMPLdPFtvgES"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


char auth[] = "";

char ssid[] = "";///Enter your wifi name
char pass[] = "";// Enter wifi password

#define DHTPIN 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//pin number
const int sensor_pin = 14;
const int relay_pin = ;
int value ;

int Water = 0;

BlynkTimer timer;

void sendSensorDHT()
{
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if ( isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }else{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  Blynk.virtualWrite(V0, t); // select your virtual pins accordingly
    }
}

void Soil()
{
  value= analogRead(sensor_pin);
  value = map(value,4095,0,0,100);

 Blynk.virtualWrite(V1, Value); 
}

void Water() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  Wat = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("The water sensor value: ");
  Serial.println(Wat);

   Blynk.virtualWrite(V2, Wat);
}

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
  //    
    timer.setInterval(1000L, sendSensor);
    //insert pin mode in or out here pls
}

void loop()
{
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