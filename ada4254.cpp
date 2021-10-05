#include "ada4254.h"


ADA4254::ADA4254(SPIClass *spi, int pinCS, bool isFullCalibration)
{
    _pinCS = pinCS;
    _spi = spi;
    _isCalFull = isFullCalibration;

    _gain = {& _adrRegGain, & _regGain};
    _mux = {&_adrRegMux, & _regMux};
    _errAnalog = {& _adrRegAnalogError, &_regAnalog_err};
    _errDigital= {& _adrRegDigitalError, &_regDigital_err};
    _setupCal = {& _adrRegSetupCal, &_regSetupCal};
}

ADA4254::ADA4254(SPIClass *spi, int pinCS, bool isFullCalibration, bool isCRC)
{
    _isCRC = isCRC;
    _pinCS = pinCS;
    _spi = spi;
    _isCalFull = isFullCalibration;

    _gain = {& _adrRegGain, & _regGain};
    _mux = {&_adrRegMux, & _regMux};
    _errAnalog = {& _adrRegAnalogError, &_regAnalog_err};
    _errDigital= {& _adrRegDigitalError, &_regDigital_err};
    _setupCal = {& _adrRegSetupCal, &_regSetupCal};
}

bool ADA4254::connectInputA()
{
    resInputsRegMux();
    _regMux |= _muxInputA;
    writeRegister(_mux);
	return chekRegister(_mux);
	
}

bool ADA4254::connectInputB()
{
    resInputsRegMux();
    _regMux |= _muxInputB;
    writeRegister(_mux);
	return chekRegister(_mux);
}


bool ADA4254::connectInputDVSS()
{
    resInputsRegMux();
    _regMux |= _muxInputC;
    resInputsRegMuxTest();
    _regSetupCal |= _cal_muxDVSS;
    writeRegister(_mux);
    writeRegister(_setupCal);
	return chekRegister(_mux);
}

bool ADA4254::connectInputP20mv()
{
    resInputsRegMux();
    _regMux |= _muxInputC;
    resInputsRegMuxTest();
    _regSetupCal |= _cal_muxP20mV;
    writeRegister(_mux);
    writeRegister(_setupCal);
	return chekRegister(_mux) & chekRegister(_setupCal);
}

bool ADA4254::connectInputM20mv()
{
    resInputsRegMux();
    _regMux |= _muxInputC;
    resInputsRegMuxTest();
    _regSetupCal |= _cal_muxM20mV;
    writeRegister(_mux);
    writeRegister(_setupCal);
	return chekRegister(_mux) & chekRegister(_setupCal);
}

bool ADA4254::disconnectInputs()
{
   resInputsRegMux();
   writeRegister(_mux);
   return chekRegister(_mux);
   
}

bool ADA4254::setGain(GAIN val)
{
    _regGain = (_regGain &(_mask_gain^0xFB)) | (val & _mask_gain);
    writeRegister(_gain);
	return chekRegister(_gain);
}

int ADA4254::available()
{
    if(readRegister(0x2F) == 0x30)
        return 0;
    return 1;        
}

bool ADA4254::init()
{
    writeRegister(_adrRegReset, _valMain_reset);
    if(_isCRC)
    {
        writeRegister(0x0b, 0);
    }
    else
    {
        writeRegister(0x0b, 32);
    }
}


bool ADA4254::isError()
{
    getDigitralError();
    return (bool)(getAnalogError() & _valErrIsError);
}

int ADA4254::getAnalogError()
{
    return readRegister(_errAnalog);
}


int ADA4254::getDigitralError()
{
    return readRegister(_errDigital);
}

bool ADA4254::clearError()
{
    if((_regAnalog_err & _mask__regDigitalErr) > 0)
    {
        _errDigital = _errDigital & _mask__regDigitalErr;
        writeRegister(_errDigital);
    }
    if(_regAnalog_err > 0)
    {
        writeRegister(_errAnalog);   
    }
    return isError();

}

bool ADA4254::isBusy()
{
     if((readRegister(_errDigital) & _valCal_BUSY) != 0)
        return true;
    else
        return false;
}

void ADA4254::calibration()
{
    writeRegister(0x2A, 0x01);
}

void ADA4254::resInputsRegMux()
{
    _regMux &= _mask_mux^_mask__regMux;
}

void ADA4254::resInputsRegMuxTest()
{
    _setupCal &= _mask_test_mux^_mask__regSetupCal;
}


void ADA4254::writeRegister(int adr, int data)
{
    int crc = 0x00;
    if(_isCRC)
        crc = crc8(adr, data);
    digitalWrite(_pinCS, !levelCS1);
    _spi->transfer(adr);
    delay(100); 
    _spi->transfer(data);
    delay(100);
    _spi->transfer(crc);
    digitalWrite(_pinCS, levelCS1);
    delay(300);
}

int ADA4254::readRegister(int adr)
{
    adr |= 0x80; //set read command
    int val = 0;
    int crc = 0;
    digitalWrite(_pinCS, !levelCS1);
    _spi->transfer(adr);
    val = _spi->transfer(0x00);
    crc = _spi->transfer(0x00);
    digitalWrite(_pinCS, levelCS1);
    if(_isCRC)
    {
        if(crc8(adr, val) != crc)
            return -1;
    }
    return val;
}

bool  ADA4254::chekRegister(int adr, int data)
{
	int val = readRegister(adr);
	if(val >= 0)
	{
		if(val == data)
			return true;
	}
	return false;
}

bool  ADA4254::chekRegister(ADA4254::__data data)
{
	int val = readRegister(*data.adress);
	if(val >= 0)
	{
		if(val == data())
			return true;
	}
	return false;
}

void ADA4254::writeRegister(ADA4254::__data data)
{
    writeRegister(*data.adress, data());
}

int ADA4254::readRegister(ADA4254::__data data)
{
    *data.val = readRegister(*data.adress);
    return data();
}

int ADA4254::crc8(int adr, int data)
{
    int val = (adr << 8) | data;
    int crc = val << 8;
    int magic = 0x107<<15; // polinom
    int mask = 0x1<<23;    // sign MSB 1

    for (int i = 0; i < 20; i++)
    {
        if((mask & crc) != 0)
        {
            crc = (crc^magic);
        }
        magic = magic >> 1;
        mask = mask >> 1;
        if(magic < 0xFF)
            break;
    }
    return crc;
}
