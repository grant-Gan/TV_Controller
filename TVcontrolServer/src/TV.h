#pragma once
#include <IRsend.h>
#include <Arduino.h>
#include <ESP8266HTTPClient.h>

class TV 
{
private:
    bool powerStatus = false;            //tv power status : on or off
    bool netStatus = false;
    const int voltageThreshold = 100;
    const unsigned int IRLED = D2;     //IRremote LED pin : D2
    const unsigned int powerCheckPin = A0;
public:
    TV();
    void turnOffTv();
    bool setNetStatus( bool setStatus );
    bool getTvPowerState(int checkCount);
    int getMaxValue( int sample[] );
    int getMinValue( int sample[] );
    bool getPowerStatus();
    bool netPowerStatus();


};