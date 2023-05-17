
//To connect blynk
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLeeJm5GGZ"
#define BLYNK_DEVICE_NAME "cleaning system RX"
#define BLYNK_AUTH_TOKEN "1EF6OKYPdbXF6IDPLrzG9OcWSPSWZ5yF"


#include <SPI.h> //To communicate with transmitter
#include <LoRa.h> //To receive package
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>



// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN; //authentication

// Your WiFi credentials.
//To connect wifi
// Set password to "" for open networks.
char ssid[] = "Yes4G_LTE_C3F4";
char pass[] = "26018745";


//#define EspSerial Serial1
// or Software Serial on Uno, Nano...

//Declare ESP pins
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX
// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);
//define the pins used by the transceiver module
#define ss 10
#define rst 9
#define dio0 4


String CurrentStr,VoltageStr,PowerStr,TotalPowerStr;
//declare variable

void setup()
{
// Debug console
Serial.begin(115200);
delay(10);
// Set ESP8266 baud rate
EspSerial.begin(ESP8266_BAUD);
delay(10);

Blynk.begin(auth, wifi, ssid, pass); // connect to the blynk
while (!Serial);
Serial.println("LoRa Receiver");
//setup LoRa transceiver module
LoRa.setPins(ss, rst, dio0);
//replace the LoRa.begin(---E-) argument with your location's frequency
//433E6 for Asia
//866E6 for Europe
//915E6 for North America


while (!LoRa.begin(433E6)) {
Serial.println(".");
delay(500);
}

// Change sync word (0xF3) to match the receiver
// The sync word assures you don't get LoRa messages from other LoRa transceivers
// ranges from 0-0xFF mcm password
LoRa.setSyncWord(0xF3);
Serial.println("LoRa Initializing OK!");
}


void loop()
{
Blynk.run();
// try to parse packet
int packetSize = LoRa.parsePacket();
if (packetSize) {
// received a packet
Serial.print("Received packet '");
// read packet
while (LoRa.available()) {
String LoRaData = LoRa.readString();
// LoRaData format: Current/Voltage&Power
// String example: 1/27.43&654
Serial.print(LoRaData);

// Get readingID, temperature and soil moisture 
int pos1 = LoRaData.indexOf('/'); //position
int pos2 = LoRaData.indexOf('&'); 
int pos3 = LoRaData.indexOf('#');
CurrentStr = LoRaData.substring(0, pos1); // read letter based on position (awal sampai /)
VoltageStr = LoRaData.substring(pos1 +1, pos2); // baca sampai &
PowerStr = LoRaData.substring(pos2+1, pos3); // dari & sampai habis
TotalPowerStr = LoRaData.substring(pos3+1, LoRaData.length()); 

// change string to float 
float Current = CurrentStr.toFloat();
float Voltage = VoltageStr.toFloat();
float Power = PowerStr.toFloat();

Serial.println("");
Serial.print("Current:  "); Serial.print(Current); Serial.println(" mA"); 
Serial.println("");
Serial.print("Voltage:  "); Serial.print(Voltage); Serial.println(" V"); 
Serial.println("");
Serial.print("Power:  "); Serial.print(Power); Serial.println(" mW"); 
Serial.println("");
Serial.print("Total Power:  "); Serial.print(TotalPowerStr); Serial.println(" mW"); 
Serial.println("");

//masukkan value dalam blynk app
Blynk.virtualWrite(V0, Current); 
Blynk.virtualWrite(V1, Voltage); 
Blynk.virtualWrite(V2, Power); 
Blynk.virtualWrite(V3, TotalPowerStr);
}
// print RSSI of packet
//Serial.print("' with RSSI ");
//Serial.println(LoRa.packetRssi());
}
}
