#ifndef MPSWITCH_H
#define MPSWITCH_H

#include <Arduino.h>

/*
    This multipurpose switch library can distinguish
    a short button-like on/off signal from a regular toggle.

    It replaces the need to physically add a button,
    if there's a switch connected to an input pin.
*/

class MPSwitch
{
public:
    MPSwitch(byte pin);
    bool poll(unsigned long millis = 0); // true on change
    bool shortPress();
    bool toggleOn();
    bool toggleOff();
    bool isOn();

private:
    const unsigned long _debounceDelay = 50;
    const unsigned long _toggleDelay = 300;

    byte _pin;
    bool _debouncing;
    bool _newRead;
    bool _lastRead;
    unsigned long _lastEdge;

    bool _shortPress;
    bool _newToggle;
    bool _lastToggle;
};

#endif
