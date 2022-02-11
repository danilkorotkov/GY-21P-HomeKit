uint32_t activeTimeout = 5*60*1000 ; //5мин
struct DigooData{
  uint32_t updated    = 0;
  float   temperature = 0;
  double humidity     = 100;
  bool    isNew[2]    = {true, true};
  float pressure      = 760.00;
  
};

struct THP : Service::TemperatureSensor {         // First we create a derived class from the HomeSpan 
  
  SpanCharacteristic *CurrentTemperature;              // here we create a generic pointer to a SpanCharacteristic 
  SpanCharacteristic *StatusActive;
  DigooData*          Data;
    
  THP(DigooData* Data) : Service::TemperatureSensor(){
 
    LOG1("Constructing digoo…\n");
    this->Data = Data;
    CurrentTemperature=new Characteristic::CurrentTemperature(25);// this is where we create the On Characterstic we had previously defined in setup().  Save this in the pointer created above, for use below
    CurrentTemperature->setRange(-50, 100, 0.1);
    StatusActive=new Characteristic::StatusActive(true);
      
    LOG1(this->Data->humidity);         
    LOG1("Constructing Digoo successful!\n");

    new THP_h(this->Data);
    
  } // end constructor


  void loop(){                                     // loop() method
    if ( (millis() - Data->updated) > activeTimeout ){
      LOG1("sensor timeout\n");
      if (StatusActive -> getVal()) {StatusActive->setVal(false);}
      return;
    }
    
    if (Data->isNew[0]){
      LOG1("sensor T update\n");
      Data->isNew[0] = false;
      
      if (CurrentTemperature->getVal() != Data->temperature) {CurrentTemperature->setVal(Data->temperature);}
      //CurrentTemperature->setVal(Data->temperature);
      
      if ( !(StatusActive -> getVal()) ) {StatusActive->setVal(true);}
    }
  } // loop 


  struct THP_h : Service::HumiditySensor {         // First we create a derived class from the HomeSpan 
    
    SpanCharacteristic *CurrentRelativeHumidity;              // here we create a generic pointer to a SpanCharacteristic 
    SpanCharacteristic *StatusActive;
    DigooData* DataH;
    
    THP_h(DigooData* Data) : Service::HumiditySensor(){
   
      LOG1("Constructing digooH…\n");
      this->DataH = Data;
      CurrentRelativeHumidity=new Characteristic::CurrentRelativeHumidity(0);// this is where we create the On Characterstic we had previously defined in setup().  Save this in the pointer created above, for use below
      StatusActive=new Characteristic::StatusActive(true);
     
      LOG1(this->DataH->humidity);         
      LOG1("Constructing Digoo successful!\n");
    } // end constructor
  
  
    void loop(){                                     // loop() method
      if ( (millis() - DataH->updated) > activeTimeout ){
        if (StatusActive -> getVal()) {StatusActive->setVal(false);}
        return;
      }
      
      if (DataH->isNew[1]){
        LOG1("sensor H update\n");
        DataH->isNew[1] = false;

        if (CurrentRelativeHumidity->getVal() != DataH->humidity) {CurrentRelativeHumidity->setVal(DataH->humidity);}
        //CurrentRelativeHumidity->setVal(DataH->humidity);
        
        if ( !(StatusActive -> getVal()) ) {StatusActive->setVal(true);}
      }
    } // loop 
  };
};
