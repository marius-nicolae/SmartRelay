#pragma once
#include "Arduino.h"
#include "SerialDbgUtils.h"
#include "avr/eeprom.h"
#include "SerialDbgUtils.h"

namespace SmartRelay {
enum class ModeEnum:uint8_t {
    serialDriven = 0,
    alwaysOn = 1,
    alwaysOff = 2        
};

ModeEnum CycleMode(ModeEnum mode);
bool ValidAsModeEnum(uint8_t rawMode);

template<class OutPin>
class ModeBase {
public:
    using ModeEnum = SmartRelay::ModeEnum;
    ModeBase(ModeEnum modeEnum = ModeEnum::serialDriven);
    void SetMode(ModeEnum modeEnum);
    ModeEnum GetModeEnum() const;
    ModeEnum NextMode() const;
    static ModeEnum CycleMode(ModeEnum& curModeEnum);
    ModeEnum Cycle();
    static bool ValidAsModeEnum(uint8_t rawMode);
    OutPin& GetOutPin();
protected:
    OutPin m_outPin;
    ModeEnum m_modeEnum;
};

template<class OutPin>
class ModePersistentBase: public ModeBase<OutPin> {
public:
    using ModeBaseT = ModeBase<OutPin>;
    
    ModePersistentBase(ModeEnum modeEnum = ModeEnum::serialDriven);
    void SetMode(ModeEnum modeEnum);
    bool SetPinHigh();
    bool SetPinLow();
protected:
    using ModeBase<OutPin>::m_outPin;
    using ModeBase<OutPin>::m_modeEnum;
private:
    struct EepromModeFormat {
        uint8_t mode : 7,
                relayState : 1;
    };
    constexpr static uint8_t* m_eepromAddr = 0x0;
    ModeEnum EepromLoad(bool *relayStateForSerialDrivenMode = 0);
    void EepromSave();
};

//============================ SmartRelayModeBase =========================
template<class OutPin>
ModeBase<OutPin>::ModeBase(ModeEnum modeEnum): m_modeEnum(modeEnum) {
    SetMode(modeEnum);
}

template<class OutPin>
inline void ModeBase<OutPin>::SetMode(ModeEnum modeEnum) {
    m_modeEnum = modeEnum;
    if (modeEnum == ModeEnum::alwaysOn) {
        m_outPin.SetHigh();
        } else if (modeEnum == ModeEnum::alwaysOff) {
        m_outPin.SetLow();
    }
}

template<class OutPin>
inline ModeEnum ModeBase<OutPin>::GetModeEnum() const {
    return m_modeEnum;
}

template<class OutPin>
inline ModeEnum ModeBase<OutPin>::NextMode() const {
    return SmartRelay::CycleMode(m_modeEnum);
}

template<class OutPin>
inline ModeEnum ModeBase<OutPin>::CycleMode(ModeEnum& curModeEnum) {
    curModeEnum = SmartRelay::CycleMode(curModeEnum);
    return curModeEnum;
}

template<class OutPin>
ModeEnum ModeBase<OutPin>::Cycle() {
    SetMode(SmartRelay::CycleMode(m_modeEnum));
}

template<class OutPin>
inline bool ModeBase<OutPin>::ValidAsModeEnum(uint8_t rawMode) {
    return SmartRelay::ValidAsModeEnum(rawMode);
}

template<class OutPin>
inline OutPin& ModeBase<OutPin>::GetOutPin() {
    return m_outPin;
}
//============================ SmartRelayModePersistentBase ==================
template<class OutPin>
ModePersistentBase<OutPin>::ModePersistentBase(ModeEnum modeEnum): ModeBase<OutPin>(modeEnum) {
    bool relayState;
    ModeBaseT::SetMode(EepromLoad(&relayState));
    if (m_modeEnum == ModeEnum::serialDriven) {
        if (relayState) {
            m_outPin.SetHigh();
        } else {
            m_outPin.SetLow();
        }
    }
}

template<class OutPin>
void ModePersistentBase<OutPin>::SetMode(ModeEnum modeEnum) {
    ModeBaseT::SetMode(modeEnum);
    EepromSave();
}

template<class OutPin>
ModeEnum ModePersistentBase<OutPin>::EepromLoad(bool *relayStateForSerialDrivenMode) {
    uint8_t eepromReadByte = eeprom_read_byte(m_eepromAddr);
    uint8_t rawMode = eepromReadByte & 0x7f; //most significant byte is reserved for relay state, for serialDriven mode
    ModeEnum modeEnum = ModeEnum::serialDriven;
    if (ValidAsModeEnum(rawMode)) {
        modeEnum = static_cast<ModeEnum>(rawMode);
        if (relayStateForSerialDrivenMode) {
            *relayStateForSerialDrivenMode = (modeEnum == ModeEnum::serialDriven) && (eepromReadByte & 0x80);
        }           
    } else if (relayStateForSerialDrivenMode) {
        *relayStateForSerialDrivenMode = false; //assign a safe value, in case the value read from eeprom is invalid
    } 
//     TR1(F("LM "), static_cast<uint8_t>(res)); //not working. Called from a constructor, before "Serial" gets initialized in setup function
    return modeEnum;
}

template<class OutPin>
void ModePersistentBase<OutPin>::EepromSave() {
    uint8_t modeByte = static_cast<uint8_t>(this->GetModeEnum());
    if(m_modeEnum == ModeEnum::serialDriven && m_outPin.IsHigh()) {
        modeByte |= 0x80; //store also the relay state for "serialDriven" mode
    }
    TR1(F("SvM "), modeByte);
    eeprom_write_byte(m_eepromAddr, modeByte);
}

template<class OutPin>
bool ModePersistentBase<OutPin>::SetPinHigh() {
    if (m_modeEnum == ModeEnum::serialDriven) {
        m_outPin.SetHigh();
        EepromSave();
        return true;
    } else {
        return false;
    }
}

template<class OutPin>
bool ModePersistentBase<OutPin>::SetPinLow() {
    if (m_modeEnum == ModeEnum::serialDriven) {
        m_outPin.SetLow();
        EepromSave();
        return true;
    } else {
        return false;
    }
}
}