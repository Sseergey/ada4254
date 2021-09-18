#ifndef ADA4254_H
#define ADA4254_H

#include <SPI.h>

enum GAIN{
    GAIN1div16 = 0x00 << 3,
    GAIN1div8  = 0x01 << 3,
    GAIN1div4  = 0x02 << 3,
    GAIN1div2  = 0x03 << 3,
    GAIN1      = 0x04 << 3,
    GAIN2      = 0x05 << 3,
    GAIN4      = 0x06 << 3,
    GAIN8      = 0x07 << 3,
    GAIN16     = 0x08 << 3,
    GAIN32     = 0x09 << 3,
    GAIN64     = 0x0A << 3,
    GAIN128    = 0x0B << 3
};


class ADA4254
{
public:
    ADA4254(SPIClass *spi, int pinCS, bool isFullCalibration);
    void connectInputA();
    void connectInputB();
    void connectInputDVSS();
    void connectInputP20mv();
    void connectInputM20mv();
    void disconnectInputs();
    void setGain(GAIN val);
    int  available();
    void init();
    bool isError();
    int getAnalogError();
    int getDigitralError();
    bool clearError();
    bool isBusy();

    void calibration();


private:

    int levelCS1 = HIGH;

  
    struct __data
    {
        const int * adress;
        int * val;

        void operator =(int value)
        {
            *val = value;
        }

        void operator |=(int value) const
        {
            *val |= value;
        }

        int & operator ()()
        {
            return *val;
        }

        int  operator &(int value)
        {
            //*val = *val & value;
            return *val & value;
        }
        void  operator &=(int value)
        {
            *val &= value;
        }

    public:
        int *adr() const;
        void setAdr(int *adr);
    };


    void resInputsRegMux();
    void resInputsRegMuxTest();
    void writeRegister(int adr, int data);
    void writeRegister(__data data);
    int  readRegister(int adr);
    int  readRegister(__data data);


    int _pinCS              = 0;
    int _regGain            = 0;
    int _regDigital_err     = 0;
    int _regAnalog_err      = 0;
    int _regMux             = 0;
    int _regSetupCal        = 0;

    SPIClass *_spi;

    __data _gain;
    __data _mux;
    __data _errAnalog;
    __data _errDigital;
    __data _setupCal;

    bool _isCRC             = false;
    bool _isCalFull         = false;

    const int _valMain_reset        = 0x01;
    const int _valMain_spi_crc      = 0x20;

    const int _valErrIsError        = 0x08;

    const int _valCal_FULL          = 0x00; // _adrRegSetupCal
    const int _valC_NoFull          = 0x00; //_adrRegSetupCal
    const int _valCal_muxAVSS       = 0x00;
    const int _valCal_BUSY          = 0x40; //_adrRegDigitalError

    //TEST_MUX
    const int _cal_muxDVSS          = 0x05;
    const int _cal_muxP20mV         = 0x0A;
    const int _cal_muxM20mV         = 0x0F;
    const int _cal_exec             = 0x01;

    //MUX
    const int _muxInputA            = 0x60;
    const int _muxInputB            = 0x18;
    const int _muxInputC            = 0x06;
    const int _muxD                 = 0x01;

    //ADRESS REGISTER
    const int _adrRegGain           = 0x00;
    const int _adrRegReset          = 0x01;
    const int _adrRegDigitalError   = 0x03;
    const int _adrRegAnalogError    = 0x04;
    const int _adrRegMux            = 0x06;
    const int _adrRegSetupCal       = 0x0E;
    const int _adrRegExeCal         = 0x2A;

    //MASK
    const int _mask_mux             = 0x7F; //_adrRegMux
    const int _mask_gain            = 0x78; //_adrRegGain
    const int _mask_test_mux        = 0x0F; //_adrRegSetupCal

    const int _mask__regSetupCal    = 0xFF;
    const int _mask__regMux         = 0x7F;
    const int _mask__regGain        = 0xFB;
    const int _mask__regDigitalErr  = 0x3B;


};

#endif // ADA4254_H
