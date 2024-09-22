// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// For ph Sensor
#include <Adafruit_GFX.h>
#include <SimpleTimer.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// For DHT11
#include <DHT.h>
#define DHT11_PIN 14

// For Blynk
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3vKnxcOpB"
#define BLYNK_TEMPLATE_NAME "Water Monitoring"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "vwowyuvHMJlwqbuu_kELOKVDZmTVT05H";//Paste auth token you copied

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "WaterMonitoring";///Enter your wifi name
char pass[] = "water123456789";// Enter wifi password


DHT dht11(DHT11_PIN, DHT11);
float tempC;
float tempCC;
float humi;

float calibration_value = 20.24 - 0.7; //21.34 - 0.7
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;

float ph_act;


void setup(void)
{
  Wire.begin();
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");
  // Start up the library
  sensors.begin();
  dht11.begin(); // for dht11
  Blynk.begin(auth, ssid, pass); // For Blynk Cloud
}

void loop(void){ 
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature of the Water is: ");
    Serial.println(tempC);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }

  // ph Sensor
  for(int i=0;i<10;i++) 
      { 
          buffer_arr[i]=analogRead(35);
          delay(30);
      }
  for(int i=0;i<9;i++)
      {
        for(int j=i+1;j<10;j++)
            {
               if(buffer_arr[i]>buffer_arr[j])
                    {
                        temp=buffer_arr[i];
                        buffer_arr[i]=buffer_arr[j];
                        buffer_arr[j]=temp;
                    }
            }
       }
    avgval=0;
    for(int i=2;i<8;i++)
      avgval+=buffer_arr[i];
    float volt=(float)avgval*3.3/4096.0/6;  
    ph_act = -5.70 * volt + calibration_value;

    Serial.print("pH Value : ");
    Serial.println(ph_act);

    // for DHT11
  humi = dht11.readHumidity();
  tempCC = dht11.readTemperature();
  //float tempF = dht11.readTemperature(true);
 
  if (isnan(tempCC) || isnan(humi)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(tempCC);
    Serial.println("°C  ~  ");
  }
  delay(1000);
  Blynk.run();
  Blynk.virtualWrite(V0, tempC);
  Blynk.virtualWrite(V1, ph_act);
  Blynk.virtualWrite(V2, tempCC);
  Blynk.virtualWrite(V3, humi);

}