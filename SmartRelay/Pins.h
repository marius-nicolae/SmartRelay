#pragma once
#include "Arduino.h"

namespace NsPins {
template<uint8_t pinNo>
class Pin {
public:
    constexpr static uint8_t m_pinNo = pinNo;
    static bool IsHigh();
    static bool IsLow();
    static uint8_t State();
};

template<uint8_t pinNo, uint8_t flavor = INPUT_PULLUP>
class InputDigitalPin: public Pin<pinNo> {
public:
    InputDigitalPin();
};

template<uint8_t pinNo>
class OutputDigitalPin: public Pin<pinNo> {
public:
    OutputDigitalPin(uint8_t initialState = LOW);
    static void SetHigh();
    static void SetLow();
};

template<uint8_t pinNo>
class ToggleOutputDigitalPin: public OutputDigitalPin<pinNo> {
public:
    ToggleOutputDigitalPin(uint8_t initialState = LOW);
    using Pin<pinNo>::IsHigh;
    using OutputDigitalPin<pinNo>::SetLow;
    using OutputDigitalPin<pinNo>::SetHigh;
    void static Toggle();
};

template<class Pin>
inline uint8_t PinInterrupt() {
    return digitalPinToInterrupt(Pin::m_pinNo);
}
//===================== Pin ===================
template<uint8_t pinNo>
inline bool Pin<pinNo>::IsHigh() {
    return State() == HIGH;
}

template<uint8_t pinNo>
inline bool Pin<pinNo>::IsLow() {
    return State() == LOW;
}

template<uint8_t pinNo>
inline uint8_t Pin<pinNo>::State() {
    return digitalRead(pinNo);
}
//===================== InputDigitalPin ===================
template<uint8_t pinNo, uint8_t flavor>
InputDigitalPin<pinNo, flavor>::InputDigitalPin() {
    pinMode(pinNo, flavor);
}

//===================== OutputDigitalPin ===================
template<uint8_t pinNo>
OutputDigitalPin<pinNo>::OutputDigitalPin(uint8_t initialState) {
    pinMode(pinNo, OUTPUT); digitalWrite(pinNo, initialState);
}

template<uint8_t pinNo>
inline void OutputDigitalPin<pinNo>::SetHigh() {
    digitalWrite(pinNo, HIGH);
}

template<uint8_t pinNo>
inline void OutputDigitalPin<pinNo>::SetLow() {
    digitalWrite(pinNo, LOW);
}

//===================== ToggleOutputDigitalPin ===================
template<uint8_t pinNo>
ToggleOutputDigitalPin<pinNo>::ToggleOutputDigitalPin(uint8_t initialState): OutputDigitalPin<pinNo>(initialState) {    
}

template<uint8_t pinNo>
void ToggleOutputDigitalPin<pinNo>::Toggle() {
    if (IsHigh()) {
        SetLow();
    } else {
        SetHigh();
    }
}
}
