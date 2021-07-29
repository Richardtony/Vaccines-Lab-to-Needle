#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"

WiFiClient  client;
#define DHTPIN 4     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT22  // DHT 22
unsigned long myChannelNumber = 1391230;  // Replace the 0 with your channel number
const char * myWriteAPIKey = "M9U8OGBJFHEIX9EY";    // Paste your ThingSpeak Write API Key between the quotes
const char * myReadAPIKey = "J0M0YVXACNDCXIO3";    // Paste your ThingSpeak Read API Key between the quotes

DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  WiFi.begin("tony", "NNFHTP7TE3");
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected, IP Address: ");
  Serial.print(WiFi.localIP());
  ThingSpeak.begin(client);
  
  dht.begin();

}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  ThingSpeak.setField(1, h);
  ThingSpeak.setField(2, t);
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  float y = ThingSpeak.readLongField(myChannelNumber, 1, myReadAPIKey); // Reads the humidity value from the cloud.
  float z = ThingSpeak.readLongField(myChannelNumber, 2, myReadAPIKey); // Reads the temperature value from the cloud.

  Serial.print(y); // Prints the humidity
  Serial.println("%");
  Serial.print(z); // Prints the temperature
  Serial.println("°C "); 

  // Check the return code
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(15000);// Wait a few seconds between measurements.
}
