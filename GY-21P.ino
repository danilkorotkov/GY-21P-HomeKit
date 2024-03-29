/*
  on Sep 21, 2020
  by MohammedDamirchi
  Home
*/
#define SDA  23
#define SCL  22
#include <Wire.h>
//TwoWire I2C_0 = TwoWire(0);*/
#include "HomeSpan.h" 
#include "THP.h" 
DigooData gy21p;
unsigned long TempTimeout  = 30000;   // чтение термодатчков
unsigned long TempTime     = 0;   // 

#include <Adafruit_Sensor.h>
#include "Adafruit_Si7021.h"
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bme; // I2C
Adafruit_Si7021 sensor = Adafruit_Si7021();

void HomeKit(){
  homeSpan.setApSSID("Sensor-AP");
  homeSpan.setApPassword("");
  homeSpan.setControlPin(0);
  homeSpan.setStatusPin(2);
  homeSpan.setLogLevel(1);

  homeSpan.setSketchVersion("2.1.1");
  homeSpan.enableOTA();
  
  homeSpan.begin(Category::Sensors,"HTP");
  
  new SpanAccessory(); 
  
    new Service::AccessoryInformation(); 
      new Characteristic::Name("Sensor"); 
      new Characteristic::Manufacturer("Danil"); 
      new Characteristic::SerialNumber("0000001"); 
      new Characteristic::Model("beta"); 
      new Characteristic::FirmwareRevision("0.1"); 
      new Characteristic::Identify();            
      
    new Service::HAPProtocolInformation();      
      new Characteristic::Version("1.1.0"); 
  
    new THP(&gy21p);
}

void poll_gy21p(){
  float bmeTemp   = bme.readTemperature();
  float bmePress  = bme.readPressure()*0.00750062;
  float SiHum     = sensor.readHumidity();
  float SiTemp    = sensor.readTemperature();
  
  Serial.print("Temperature(bme): ");
  Serial.print(bmeTemp);
  Serial.print(" °C\t");

  Serial.print("Pressure: ");
  Serial.print(bmePress);
  Serial.print(" mmHg\t");

  Serial.print("Humidity: ");
  Serial.print(SiHum, 2);
  Serial.print("\tTemperature(Si7021): ");
  Serial.println(SiTemp, 2);

  if (bmePress != NAN){
    gy21p.pressure = bmePress;
  }

  if (!isnan(bmeTemp) && !isnan(SiTemp)){
    gy21p.temperature = (bmeTemp + SiTemp)/2;
    gy21p.updated     = millis();
    gy21p.isNew[0]    = true;
  }

  if (!isnan(bmeTemp) && isnan(SiTemp)){
    gy21p.temperature = bmeTemp;
    gy21p.updated     = millis();
    gy21p.isNew[0]    = true;
  }  
  
  if (isnan(bmeTemp) && !isnan(SiTemp)){
    gy21p.temperature = SiTemp;
    gy21p.updated     = millis();
    gy21p.isNew[0]    = true;
  }

  if (!isnan(SiHum)){
    gy21p.humidity    = SiHum;
    gy21p.updated     = millis();
    gy21p.isNew[1]    = true;
  }
  
}


void setup() {
  Serial.begin(115200);
  //I2C_0.begin(SDA , SCL , uint32_t (700000) );
  Wire.begin(SDA, SCL, uint32_t (400000));
  bme.begin(BMP280_ADDRESS_ALT);
  sensor.begin();
  poll_gy21p();
  HomeKit();



}

void loop() {
  if ( (millis() - TempTime) > TempTimeout ){
    poll_gy21p();
    TempTime = millis();
  }
  
  homeSpan.poll();
    
}

/*
#include <Wire.h>
 
void setup(){
    Wire.begin();    
 
    Serial.begin(115200);
    while (!Serial);
    Serial.println("\nI2C Scanner");
} 
 
void loop(){
    byte error, address;
    int nDevices;
 
    Serial.println("Scanning...");
 
    nDevices = 0;
    for(address = 8; address < 127; address++ ){
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
 
        if (error == 0){
            Serial.print("I2C device found at address 0x");
            if (address<16)
                Serial.print("0");
            Serial.print(address,HEX);
            Serial.println(" !");
 
            nDevices++;
        }
        else if (error==4) {
            Serial.print("Unknow error at address 0x");
            if (address<16)
                Serial.print("0");
            Serial.println(address,HEX);
        } 
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
 
    delay(5000);
}
*/

/*
 CustomCharacteristic.AtmosphericPressureLevel = function() {
    Characteristic.call(this, 'Air Pressure', CustomCharacteristic.AtmosphericPressureLevel.UUID);
    this.setProps({
      format: Characteristic.Formats.UINT8,
      unit: "mbar",
      minValue: 800,
      maxValue: 1200,
      minStep: 1,
      perms: [Characteristic.Perms.READ, Characteristic.Perms.NOTIFY]
    });
    this.value = this.getDefaultValue();
  };
  CustomCharacteristic.AtmosphericPressureLevel.UUID = 'E863F10F-079E-48FF-8F27-9C2605A29F52';
  inherits(CustomCharacteristic.AtmosphericPressureLevel, Characteristic);

  // courtesy of https://github.com/robi-van-kinobi/homebridge-cubesensors
  CustomCharacteristic.AtmosphericPressureSensor = function(displayName, subtype) {
    Service.call(this, displayName, CustomCharacteristic.AtmosphericPressureSensor.UUID, subtype);

    // Required Characteristics
    this.addCharacteristic(CustomCharacteristic.AtmosphericPressureLevel);

    // Optional Characteristics
    this.addOptionalCharacteristic(Characteristic.StatusActive);
    this.addOptionalCharacteristic(Characteristic.StatusFault);
    this.addOptionalCharacteristic(Characteristic.StatusLowBattery);
    this.addOptionalCharacteristic(Characteristic.StatusTampered);
    this.addOptionalCharacteristic(Characteristic.Name);
  };
  CustomCharacteristic.AtmosphericPressureSensor.UUID = 'B77831FD-D66A-46A4-B66D-FD7EE8DFE3CE';
  inherits(CustomCharacteristic.AtmosphericPressureSensor, Service);


 */
//#define CUSTOM_CHAR(NAME,UUID,PERMISISONS,FORMAT,TYPE,DEFVAL,MINVAL,MAXVAL,STATIC_RANGE)
