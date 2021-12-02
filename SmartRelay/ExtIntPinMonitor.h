#pragma once
#include "Pins.h"
#include "TypesUtils.h"
#include "SerialDbgUtils.h"

namespace NsPinsMonitor {
using namespace NsPins;
using namespace NsTypesUtils;


template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode = LOW>
class ExtIntPinMonitorBase //external interrupt pin monitor base
{
public:
    using ExtIntPinMonitor = ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>;
    using PinObserverTriggerT = typename RemoveReference<PinObserverTrigger>::Type;
    using PinObserverT = typename RemoveReference<PinObserver>::Type;

    ExtIntPinMonitorBase(PinObserverTrigger pinObserverTrigger, bool initialStartMonitor = true);
    
    void Tick(bool nestedTick = true); //call m_pinObserverTrigger.Tick()?
    void StopMonitor();
    void StartMonitor();
    PinObserverTriggerT& GetPinObserverTrigger();
    bool PinChanged () const {
        return m_pinChanged;
    }
 protected:
private:
    static void Isr();
    volatile static bool m_pinChanged;
    PinObserverTrigger m_pinObserverTrigger;
    Pin m_pin;
};

template<class Pin, class PinObserver, class PinObserverTrigger>
using ExtIntPinLowMonitorBase = ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, LOW>;

//============================== ExtIntPinMonitorBase class =======================
template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
volatile bool ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::m_pinChanged = false;

template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::ExtIntPinMonitorBase(PinObserverTrigger pinObserverTrigger, 
        bool initialStartMonitor): m_pinObserverTrigger(pinObserverTrigger) {
    m_pinObserverTrigger.AttachPinMonitor(this);
    if (initialStartMonitor) {
        StartMonitor();
    }        
}

template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
void ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::Isr()
{
    m_pinChanged = true;
}

template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
void ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::Tick(bool nestedTick) {
    if (m_pinChanged) {
        TR(F("PC"));
        StopMonitor(); //the observer trigger, observer or some other external entity must resume the monitoring (call StartMonitor), sometime in the future
        m_pinObserverTrigger.Notify(); //notify the pin observer trigger, which will call (in its own terms) the OnPinChange on its embedded pin observer
    }
    if (nestedTick) {
        m_pinObserverTrigger.Tick();
    }        
}

template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
void ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::StopMonitor() {
    TR(F("SM"));
    detachInterrupt(PinInterrupt<Pin>());
    m_pinChanged = false;
}

template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
void ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::StartMonitor() {
    m_pinChanged = false;
    attachInterrupt(PinInterrupt<Pin>(), Isr, extIntMode); //attach the interrupt
}

template<class Pin, class PinObserver, class PinObserverTrigger, uint8_t extIntMode>
typename ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::PinObserverTriggerT& 
ExtIntPinMonitorBase<Pin, PinObserver, PinObserverTrigger, extIntMode>::GetPinObserverTrigger() {
    return m_pinObserverTrigger;
}
}
