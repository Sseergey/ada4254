#include "ada4254.h"

const int slaveSPISelectPin = 4;

//turn on crc 
ADA4254 ada(&SPI, slaveSPISelectPin, 0, 1);

void setup() {

  Serial.begin(9600);
  pinMode(slaveSPISelectPin, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(14000, MSBFIRST, SPI_MODE0));

  while (ada.available()) {
    ;
  }
}


void loop()
{
    ada.init();
    ada.calibration();
    ada.connectInputA();
    ada.setGain(GAIN1);
}
