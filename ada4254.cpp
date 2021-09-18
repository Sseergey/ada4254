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

void ADA4254::connectInputA()
{
    resInputsRegMux();
    _regMux |= _muxInputA;
    writeRegister(_mux);
}

void ADA4254::connectInputB()
{
    resInputsRegMux();
    _regMux |= _muxInputB;
    writeRegister(_mux);
}


void ADA4254::connectInputDVSS()
{
    resInputsRegMux();
    _regMux |= _muxInputC;
    resInputsRegMuxTest();
    _regSetupCal |= _cal_muxDVSS;
    writeRegister(_mux);
    writeRegister(_setupCal);
}

void ADA4254::connectInputP20mv()
{
    resInputsRegMux();
    _regMux |= _muxInputC;
    resInputsRegMuxTest();
    _regSetupCal |= _cal_muxP20mV;
    writeRegister(_mux);
    writeRegister(_setupCal);
}

void ADA4254::connectInputM20mv()
{
    resInputsRegMux();
    _regMux |= _muxInputC;
    resInputsRegMuxTest();
    _regSetupCal |= _cal_muxM20mV;
    writeRegister(_mux);
    writeRegister(_setupCal);
}

void ADA4254::disconnectInputs()
{
   resInputsRegMux();
   writeRegister(_mux);
}

void ADA4254::setGain(GAIN val)
{
    _regGain = (_regGain &(_mask_gain^0xFB)) | (val & _mask_gain);
    writeRegister(_gain);
}

int ADA4254::available()
{
    if(readRegister(0x2F) == 0x30)
        return 0;
    return 1;        
}

void ADA4254::init()
{
    writeRegister(_adrRegReset, _valMain_reset);
    if(_isCRC)
        writeRegister(_adrRegDigitalError, _valMain_spi_crc);
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
    digitalWrite(_pinCS, !levelCS1);
    _spi->transfer(adr);
    delay(100); 
    _spi->transfer(data);
    delay(100);
    _spi->transfer(0x00);
    digitalWrite(_pinCS, levelCS1);
    delay(100);
}

int ADA4254::readRegister(int adr)
{
    adr |= 0x80; //set read command
    int val = 0;
    int crc = 0;
    bool isCRC = _isCRC;
    if(_isCRC)
    {
        if((adr == 0x03) || (adr == 0x04) || (adr == 0x05))
        {
            isCRC = false;
        }
    }
    digitalWrite(_pinCS, !levelCS1);
    _spi->transfer(adr);
    val = _spi->transfer(0x00);
    crc = _spi->transfer(0x00);
    digitalWrite(_pinCS, levelCS1);
    return val;
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
