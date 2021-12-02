#pragma once
#include "SerialDbgUtils.h"
#include "TypesUtils.h"

namespace NsPinObserver {
using namespace NsTypesUtils;
    
//Todo: remove the class, if not used
template<class Pin>
class PinObserverBase
{
public:
    using PinT = typename RemoveReference<Pin>::Type;
    
    PinObserverBase(Pin& pin);
    PinT& GetPin();
private:
    Pin m_pin;
};

//===================== PinObserverBase ====================
template<class Pin, class Derived>
PinObserverBase<Pin, Derived>::PinObserverBase(Pin pin):  m_pin(pin) {
}

template<class Pin>
inline Pin& PinObserverBase<Pin>::GetPin() {
    return m_pin;
}
};