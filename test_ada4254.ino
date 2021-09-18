#include "ada4254.h"

const int slaveSelectPin = 4;

ADA4254 ada(&SPI, 4, 0);

void setup() {

  Serial.begin(9600);

  pinMode(slaveSelectPin, OUTPUT);
  digitalWrite(slaveSelectPin, levelCS1);
  SPI.begin();
  SPI.beginTransaction(SPISettings(14000, MSBFIRST, SPI_MODE0));

  while (ada.available()) {
    ;
  }
}


void loop()
{
    ada.init();
	while (ada.calibration()) {
    ;
    }
    ada.calibration();
    ada.connectInputA();
    ada.setGain(GAIN1);
}
