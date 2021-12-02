#include "Arduino.h"
#include "TypesUtils.h"

namespace NsUtils {
using namespace NsTypesUtils;
    
void MillisDelay(uint delay) {
    auto initialTime = millis();
    while(millis() - initialTime < delay);
}

bool Passed(ulong timeSpan, ulong since) {
    return millis() - since >= timeSpan;
}
}
