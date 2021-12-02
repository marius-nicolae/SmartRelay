#pragma once
#include "Arduino.h"
#include "Utils.h"
#include "SerialDbgUtils.h"

namespace NsPinObserverTrigger {
using namespace NsUtils;

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, class Derived>
class PinObserverTriggerBase
{
public:
    using PinObserverT = typename RemoveReference<PinObserver>::Type;
    using PinObserverTrigger = PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>;
    using PinMonitorT = PinMonitor<MonitoredPin, PinObserver, Derived>;
    PinObserverTriggerBase(PinObserver pinObserver);
    void Tick(bool nestedTick = true); //call m_pinObserver.Tick()?
    void Notify();
    PinObserverT& GetPinObserver();
    void AttachPinMonitor(PinMonitorT* pPinMonitor);
protected:
    PinObserver m_pinObserver;
    PinMonitorT* m_pPinMonitor;
private:
};

//trigger observer if the pin is in the "pinMode" mode for the "debounceTime" milliseconds
template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, uint debounceTime = 50 /*in milliseconds*/, 
    uint8_t pinMode = HIGH>
class PinObserverTriggerDebouncedBase: public PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, 
        PinObserverTriggerDebouncedBase<PinObserver, MonitoredPin, PinMonitor, debounceTime, pinMode>>
{
public:
    using PinObserverTriggerDebounced = PinObserverTriggerDebouncedBase<PinObserver, MonitoredPin, PinMonitor, debounceTime, pinMode>;
    using PinObserverTrigger = PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, PinObserverTriggerDebounced>;

    PinObserverTriggerDebouncedBase(PinObserver pinObserver);
    void Tick(bool nestedTick = true);
    void Notify();
protected:
    bool m_pendingNotification;
    ulong m_debounceStartTime;
    using PinObserverTrigger::m_pinObserver;
    using PinObserverTrigger::m_pPinMonitor;
// private:
};

// template<class PinObserver, PinMonitor>
// using PinObserverTriggerDebounced = PinObserverTriggerDebouncedBase<PinObserver, PinMonitor>;
//=========================== PinChangeObserverTrigger(Immediate)Base ==========================
template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, class Derived>
PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>::PinObserverTriggerBase(PinObserver pinObserver): 
        m_pinObserver(pinObserver), m_pPinMonitor(nullptr) {
}

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, class Derived>
inline void PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>::Tick(bool nestedTick) { //the real action takes place directly in Notify method
    if (nestedTick) {
        m_pinObserver.Tick();
    }
}

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, class Derived>
inline void PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>::Notify() {
    m_pinObserver.OnPinChange();
    ASSERT(m_pPinMonitor);
    m_pPinMonitor->StartMonitor();
}

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, class Derived>
inline typename PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>::PinObserverT& 
PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>::GetPinObserver() {
    return m_pinObserver;
}

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, class Derived>
inline void PinObserverTriggerBase<PinObserver, MonitoredPin, PinMonitor, Derived>::AttachPinMonitor(PinMonitorT* pPinMonitor) {
    m_pPinMonitor = pPinMonitor;
}
//=============================== PinChangeObserverTriggerDebounced ========================
template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, uint debounceTime, uint8_t pinMode>
PinObserverTriggerDebouncedBase<PinObserver, MonitoredPin, PinMonitor, debounceTime, pinMode>::
        PinObserverTriggerDebouncedBase(PinObserver pinObserver): PinObserverTrigger(pinObserver), m_pendingNotification(false), 
        m_debounceStartTime(0) {
}

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, uint debounceTime, uint8_t pinMode>
void PinObserverTriggerDebouncedBase<PinObserver, MonitoredPin, PinMonitor, debounceTime, pinMode>::Notify() { //just mark the observer notification time
    m_pendingNotification = true;
    m_debounceStartTime = millis();
}

template<class PinObserver, class MonitoredPin, template <class, class, class> class PinMonitor, uint debounceTime, uint8_t pinMode>
void PinObserverTriggerDebouncedBase<PinObserver, MonitoredPin, PinMonitor, debounceTime, pinMode>::Tick(bool nestedTick) {
    if (m_pendingNotification) {
        ulong currentTime = millis();
        if (MonitoredPin::State() == pinMode) {
            if (Passed(debounceTime, m_debounceStartTime)) {
                PinObserverTrigger::Notify(); //throughly notify observer
                m_pendingNotification = false;
                ASSERT(m_pPinMonitor);
                m_pPinMonitor->StartMonitor();
            }
        } else {
            m_debounceStartTime = currentTime;
        }
    }
    PinObserverTrigger::Tick(nestedTick);
}
}
