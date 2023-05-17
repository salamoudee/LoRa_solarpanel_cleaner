#include <SPI.h>     
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

//Define all Pins that are used

#define actpin1 4 //Actuator Pin positive
#define actpin2 5 //Actuator Pin negative
#define waterpumpPin1 6 //Waterpump pin +ve
#define waterpumpPin2 7 //Waterpump pin -ve

Adafruit_INA219 ina219; //Initialize INA219 module. (Current and Voltage Sensor)


int counter = 0; // Just to count 
void setup() 
{
 //Initialize all Pins 
 pinMode (actpin1, OUTPUT);
 pinMode (actpin2, OUTPUT);
 pinMode (waterpumpPin1, OUTPUT);
 pinMode (waterpumpPin2, OUTPUT);

 //Insert values for all pins, to ensure all devices are turned off
 digitalWrite(actpin1, LOW);
 digitalWrite(actpin2, LOW);
 digitalWrite(waterpumpPin1, LOW);
 digitalWrite(waterpumpPin2, LOW);

  Serial.begin(9600); //To display outputs in Serial monitor(Frequency)
  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) //To initialize location of Lora, 433E6 = Asia
  {
    Serial.println("Starting LoRa failed!"); //If LoRA fail to start, nnti dia loop, until it can be started
    while(1);
  }
  LoRa.setSyncWord(0xF3); //Initialize LoRA password, to ensure same as receiver
  
    
  if (! ina219.begin()) { //If INA219 cant be started
    Serial.println("Failed to find INA219 chip"); //Loops until INA219 starts
    while (1) { delay(10); }
  }
  
  ina219.setCalibration_16V_400mA(); //Set INA219 calibration. , The bigger the range, lower the accuracy
  
  Serial.println("Measuring voltage and current with INA219 ...");
  
}


void loop() 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float Current = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  float power = 0;


  //TO GET VALUES OF LOAD
  shuntvoltage = ina219.getShuntVoltage_mV(); //call function to find shunt voltage
  busvoltage = ina219.getBusVoltage_V(); //call function to find bus voltage
  Current = ina219.getCurrent_mA(); //call function to find current
  power_mW = ina219.getPower_mW(); //call function to find power
  loadvoltage = busvoltage + (shuntvoltage / 1000); //Voltage equation for the real voltage of the load. 

  power = loadvoltage * Current; // To validate the power obtained by INA219
  
  if (Current < 0){ //To ensure current is positive, kadang2 INA219 send negative value
  Current = 0;
  }
  
  //TO PRINT OBTAINED VALUES IN SERIAL MONITOR
  Serial.println(Current);
  Serial.println(loadvoltage);
  Serial.println(power);

  //String is sentence which can contain numericals and letters combined
  //To declare a variable (String) containing all readings of load.
  String msg = String(Current) + "/" + String(loadvoltage) + "&" + String(power) ;
  
  //To notify Lora send message
  Serial.println("Sending message ");
  Serial.println(msg);

  //To send the string
  LoRa.beginPacket(); //masuk kotak
  LoRa.print(msg); //Ensure barang betul dalam kotak 
  LoRa.endPacket(); //Ensure barang sampai kat rumah customer

  // FIRST TIME DETECTION
  if (Current < 20){ //when current less than 20mA
        Serial.println("Current less than 20mA detected"); 
        delay(1000);
        Serial.println("5");
        delay(1000);
        Serial.println("4");
        delay(1000);
        Serial.println("3");
        delay(1000);
        Serial.println("2");
        delay(1000);

        // Get 2nd reading, to ensure current is really2 below 20mA
        shuntvoltage = ina219.getShuntVoltage_mV();
        busvoltage = ina219.getBusVoltage_V();
        Current = ina219.getCurrent_mA();
        power_mW = ina219.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        power = loadvoltage * Current; //p=iv
  
        if (Current < 20){ //Cleaning start
        
        Serial.println("Cleaning");

        //To turn on water pumps
        analogWrite(waterpumpPin1, 0); //0 until 255, value of PWM
        analogWrite(waterpumpPin2, 200); //PWM to control power
        
        delay(10000); //delay to control water pump ON time

        Serial.println("Water pump off");
        digitalWrite(waterpumpPin1, LOW);
        digitalWrite(waterpumpPin2, LOW);
        delay(500);
        
        Serial.println("Actuator ON");
        digitalWrite(actpin1, HIGH);
        digitalWrite(actpin2, LOW);
        delay(20000);        
        Serial.println("Actuator reverse");
        digitalWrite(actpin1, LOW);
        digitalWrite(actpin2, HIGH);
        delay(20000);

        Serial.println("Actuator off");
        digitalWrite(actpin1, LOW);
        digitalWrite(actpin2, LOW);

        //Turn on balik waterpump 2nd time
        analogWrite(waterpumpPin1, 0);
        analogWrite(waterpumpPin2, 200);
        
        delay(10000); //delay to control water pump ON time

        Serial.println("Water pump off");
        digitalWrite(waterpumpPin1, LOW);
        digitalWrite(waterpumpPin2, LOW);
        delay(500);

        Serial.println("Actuator ON");
        digitalWrite(actpin1, HIGH);
        digitalWrite(actpin2, LOW);
        delay(20000);
        
        Serial.println("Actuator reverse");
        digitalWrite(actpin1, LOW);
        digitalWrite(actpin2, HIGH);
        delay(20000);

        Serial.println("Actuator off");
        digitalWrite(actpin1, LOW);
        digitalWrite(actpin2, LOW);
        
    
    }}
  
  delay(1000);
}
