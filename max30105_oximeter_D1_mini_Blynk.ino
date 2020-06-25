// OOOOOOKKK   FUNZIONA BENE !!!!!
//This sketch reads data form MAX30102 sensor via I2C and posts to Vitual pins in Blynk applications

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 
MAX30105 particleSensor;
 
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;
uint32_t tsLastReport = 0;

//Here you have to put your Blynk Auth string and access point credentials.
char auth[] = "0xOqmGrQmfDc2hP9yUGR5X7Z4ZdMe6hq";
char ssid[] = "Vodafone-34162499";
char pass[] = "exvwmfckpc5bkav";

 
void setup()
{
Blynk.begin(auth, ssid, pass);
Serial.begin(115200);
Serial.println("Initializing...");
 
// Initialize sensor
if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
{
Serial.println("MAX30105 was not found. Please check wiring/power. ");
while (1);
}
Serial.println("Place your index finger on the sensor with steady pressure.");
 
particleSensor.setup(); //Configure sensor with default settings
particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}
 
void loop()
{
long irValue = particleSensor.getIR(); //   / 1000;  //divido per 1000 il valore
 
if (checkForBeat(irValue) == true)
{
//We sensed a beat!
long delta = millis() - lastBeat;
lastBeat = millis();
beatsPerMinute = 60 / (delta / 1000.0);
if (beatsPerMinute < 255 && beatsPerMinute > 20)
{
rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
rateSpot %= RATE_SIZE; //Wrap variable
 
//Take average of readings
beatAvg = 0;
for (byte x = 0 ; x < RATE_SIZE ; x++)
beatAvg += rates[x];
beatAvg /= RATE_SIZE;
}
}
 
Serial.print("IR=");
Serial.print(irValue);
Serial.print(", BPM=");
Serial.print(beatsPerMinute);
Serial.print(", Avg BPM=");
Serial.print(beatAvg); 
 
if (irValue < 50000)
Serial.print(" No finger?");
Serial.println(); 
Blynk.run();
Blynk.virtualWrite(V1, irValue);
Blynk.virtualWrite(V2, beatsPerMinute);
Blynk.virtualWrite(V3, beatAvg);
}
