#include "mpSwitch.h"

MPSwitch::MPSwitch(byte pin) : _pin(pin)
{
    pinMode(_pin, INPUT_PULLUP);

    _newRead = _lastRead = digitalRead(_pin);
    _lastEdge = millis();
    _shortPress = false;
    _newToggle = _lastToggle = false;
}

bool MPSwitch::poll(unsigned long mil)
{
    if (mil == 0)
        mil = millis();
    _lastRead = _newRead;
    _lastToggle = _newToggle;

    // debounce edge
    if (!_debouncing || _lastEdge + _debounceDelay < mil)
    {
        _debouncing = false;
        _newRead = digitalRead(_pin);
    }

    _shortPress = _newRead && !_lastRead && _lastEdge + _toggleDelay > mil;
    _newToggle = !_lastRead && _lastEdge + _toggleDelay < mil;

    if (_newRead != _lastRead)
    {
        _lastEdge = mil;
        _debouncing = true;
    }

    return shortPress() || toggleOn() || toggleOff();
}

bool MPSwitch::shortPress()
{
    return _shortPress;
}

bool MPSwitch::toggleOn()
{
    return _newToggle && !_lastToggle;
}

bool MPSwitch::toggleOff()
{
    return !_newToggle && _lastToggle;
}

bool MPSwitch::isOn()
{
    return _newToggle;
}
