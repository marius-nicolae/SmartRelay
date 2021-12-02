#pragma once
#include "Pins.h"
#include "Mode.h"
#include "ModePresenterTypes.h"
#include "TypesUtils.h"
#include "Utils.h"

namespace NsModePresenter {
using namespace NsPins;
using namespace NsTypesUtils;
using namespace NsUtils;
    
template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, 
uint8_t cyclesNo = 3, uint cycleDuration = 1000>
class LedBlinkModePresenterBase { //led blink mode presenter
public:
    using ModeEnum = typename Mode::ModeEnum;
    using PresenterObserverT = PresenterObserver<Mode, Derived>;

    void Present(const ModeEnum mode, uint pendingTime = 0 /*in milliseconds*/);
    State PresentationState() const;
    bool DifferentModePresented() const;

    TickReturnCode Tick(); //returns 0 - no-op, 1 - in progress presentation, 2 - presentation finished
    void AssociatePresenterObserver(PresenterObserverT* presenterObserver);
private:
    inline const auto& LedBlinkModePhasesRatios() const;
    inline uint CurrentPhaseDuration() const;

    enum class HistoryStatus: int8_t {
        noModeEver = -1,
        sameMode = 0,
        differentMode = 1,
    };
    
    ModeEnum m_presentedMode = ModeEnum::serialDriven;
    HistoryStatus m_historyStatus = HistoryStatus::noModeEver;
    uint8_t m_curPhaseIdx = 0;
    uint8_t m_curCycle = 0;
    State m_state = State::noOp; 
    ulong m_startTime = 0;
    uint m_pendingTime = 0;
    LedToggleableOutPin m_ledPin;
    PresenterObserverT* m_pPresenterObserver = nullptr;
};

//used to store presenter's phases ratios, needed to compute the on/off led's time intervals, for a given mode
template<uint8_t phasesRatiosNo> 
class LedBlinkPhasesRatiosBase {
public:
    template<typename ...Ts>
    LedBlinkPhasesRatiosBase(Ts ...phasesRatios);
    constexpr static uint8_t PhasesNo();
    inline uint8_t operator [](const uint8_t idx) const;
private:
    template<typename T, typename ...Ts>
    void InitPhasesRatios(T firstPhaseRatio, Ts ...restOfPhasesRatios);
    template<typename T>
    void InitPhasesRatios(T firstPhaseRatio);

    uint8_t m_phasesRatios[phasesRatiosNo];
};

//=========================== LedBlinkTimeRatioPresenter =====================
template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
inline void LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::Present(const ModeEnum modeEnum, 
        uint pendingTime) {
    TR1(F("PR "), static_cast<uint8_t>(modeEnum));
    if (m_historyStatus == HistoryStatus::noModeEver) {
        m_historyStatus = HistoryStatus::sameMode;
    } else {
        m_historyStatus = modeEnum != m_presentedMode? HistoryStatus::differentMode: HistoryStatus::sameMode;
    }
    if (pendingTime) {
        m_state = State::presentationPending;
        m_pendingTime = pendingTime;
        m_ledPin.SetLow();
    } else {
        m_curCycle = 0; m_curPhaseIdx = 0; 
        m_state = State::presentationInProgress;
        m_pendingTime = 0;
        m_ledPin.SetHigh();
    }    
    m_presentedMode = modeEnum;
    m_startTime = millis();
}

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
TickReturnCode LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::Tick() {
    switch(m_state) {
    case State::presentationInProgress:
        if (Passed(CurrentPhaseDuration(), m_startTime)) {
            TR(F("LBPS")); //Led Blink Phase Switch
            if (++m_curPhaseIdx >= LedBlinkModePhasesRatios().PhasesNo()) {
                if (++m_curCycle >= cyclesNo) {
                    m_state = State::noOp;
                    TR(F("LBPF"));
                    if (m_pPresenterObserver) {
                        m_pPresenterObserver->OnPresentationFinish(m_presentedMode, DifferentModePresented());
                    }
                    return TickReturnCode::presentationFinished;
                }
                m_curPhaseIdx = 0;
            }
            m_ledPin.Toggle(); m_startTime = millis();
        }
//         TR(F("LBPIP"));
        return TickReturnCode::presentationInProgress;
    case State::presentationPending:
        if (Passed(m_pendingTime, m_startTime)) {
            auto tmp = m_historyStatus;
            Present(m_presentedMode);
            m_historyStatus = tmp; //preserve present status, for correctly determining if a different mode were presented or the same
//             TR(F("LBPIP"));
            return TickReturnCode::presentationInProgress;
        } else {
//             TR(F("LBPPP"));
            return TickReturnCode::presentationPending;
        }
    default:
//         TR(F("LBPNO"));
        return TickReturnCode::noOp;
    };
}

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
inline const auto& LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::
        LedBlinkModePhasesRatios() const {
    return (*static_cast<const Derived*>(this))[m_presentedMode];
}

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
inline uint LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::CurrentPhaseDuration() const {
    auto ledBlinkModePhasesRatios = LedBlinkModePhasesRatios();
    uint sumPhasesRatios = 0;
    for(uint8_t i = 0; i < ledBlinkModePhasesRatios.PhasesNo(); i++) {
        sumPhasesRatios += ledBlinkModePhasesRatios[i];
    }
    return cycleDuration * ledBlinkModePhasesRatios[m_curPhaseIdx] / sumPhasesRatios;
}

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
inline State LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::PresentationState() const {
    return m_state;
}

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
bool LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::DifferentModePresented() const {
    return m_historyStatus == HistoryStatus::differentMode;
}

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, class Derived, uint8_t cyclesNo, uint cycleDuration>
inline void LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, Derived, cyclesNo, cycleDuration>::
        AssociatePresenterObserver(PresenterObserverT* presenterObserver) {
    m_pPresenterObserver = presenterObserver;
}
//=================== LedBlinkModePhasesRatios ================
template<uint8_t phasesRatiosNo>
template<typename ...Ts>
LedBlinkPhasesRatiosBase<phasesRatiosNo>::LedBlinkPhasesRatiosBase(Ts ...phasesRatios) {
    static_assert(sizeof ...(phasesRatios) == phasesRatiosNo, "The arguments number must match the 'phasesRatiosNo' argument");
    InitPhasesRatios(phasesRatios...);
}

template<uint8_t phasesRatiosNo>
template<typename T, typename ...Ts>
void LedBlinkPhasesRatiosBase<phasesRatiosNo>::InitPhasesRatios(T firstPhaseRatio, Ts ...restOfPhasesRatios) {
    m_phasesRatios[phasesRatiosNo - 1 - sizeof ...(restOfPhasesRatios)] = firstPhaseRatio;
   InitPhasesRatios(restOfPhasesRatios...);
}

template<uint8_t phasesRatiosNo>
template<typename T>
void LedBlinkPhasesRatiosBase<phasesRatiosNo>::InitPhasesRatios(T firstPhaseRatio) {
    m_phasesRatios[phasesRatiosNo - 1] = firstPhaseRatio;
}

template<uint8_t phasesRatiosNo>
constexpr uint8_t LedBlinkPhasesRatiosBase<phasesRatiosNo>::PhasesNo() {
    return phasesRatiosNo;
}

template<uint8_t phasesRatiosNo>
inline uint8_t LedBlinkPhasesRatiosBase<phasesRatiosNo>::operator [](const uint8_t idx) const {
    ASSERT(idx < PhasesNo());
    return m_phasesRatios[idx];
}
}
