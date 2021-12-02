#include "avr/eeprom.h"
#include "Mode.h"
#include "SerialDbgUtils.h"

namespace SmartRelay {
ModeEnum CycleMode(ModeEnum mode) {
    switch (mode) {
    case ModeEnum::serialDriven:
        mode = ModeEnum::alwaysOn;
        break;
    case ModeEnum::alwaysOn:
        mode = ModeEnum::alwaysOff;
        break;
    default:
        mode = ModeEnum::serialDriven;
        break;
    }
    return mode;
}    

bool ValidAsModeEnum(uint8_t rawMode) {
    return rawMode >= static_cast<uint8_t>(ModeEnum::serialDriven) && rawMode <= static_cast<uint8_t>(ModeEnum::alwaysOff);
}
}