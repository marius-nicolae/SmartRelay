#pragma once
#include "Arduino.h"
#include "SerialDbgUtils.h"
#include "Mode.h"
#include "ModePresenterTypes.h"
#include "ModePresenter.h"

namespace SmartRelay {
using namespace SmartRelay;

template<class Mode, class ModePresenter>
class ModePinObserverBase{
public:
    using ModePresenterT = typename RemoveReference<ModePresenter>::Type;
    using ModeT = typename RemoveReference<Mode>::Type;
    using ModeEnum = typename Mode::ModeEnum;

    ModePinObserverBase(Mode mode, ModePresenter modePresenter, bool autoAssociatePresenterObserver = false);
    void OnPinChange();
    ModeT& GetMode();
    ModePresenterT& GetModePresenter();
    void Tick(bool nestedTick = true); //call the m_modePresenter.Tick()?
    void OnPresentationFinish(ModeEnum presentedMode, bool modeChanged);
private:
    Mode m_mode;
    ModeEnum m_presentedMode;
    ModePresenter m_modePresenter;
};

//===================== ModePinObserverBase ====================
template<class Mode, class ModePresenter>
ModePinObserverBase<Mode, ModePresenter>::ModePinObserverBase(Mode mode, ModePresenter modePresenter, bool autoAssociatePresenterObserver): 
        m_mode(mode), m_presentedMode(m_mode.GetModeEnum()), m_modePresenter(modePresenter) {
    if (autoAssociatePresenterObserver) {
        m_modePresenter.AssociatePresenterObserver(this); //for being notified back when a mode presentation ends
    }        
}

template<class Mode, class ModePresenter>
inline void ModePinObserverBase<Mode, ModePresenter>::OnPinChange() {
    TR(F("CP ")); //change pin
    TR1("PS ", static_cast<uint8_t>(m_modePresenter.PresentationState()));
    if (m_modePresenter.PresentationState() == State::noOp) { //present the current mode
        TR(F("PM"));
        m_modePresenter.Present(m_mode.GetModeEnum());
    } else if (m_modePresenter.PresentationState() == State::presentationInProgress || 
               m_modePresenter.PresentationState() == State::presentationPending) {
        TR(F("CM"));
        #ifdef SIMULATION
            constexpr uint pendingTime = 5; //ms
        #else
            constexpr uint pendingTime = 500; //ms
        #endif
        m_modePresenter.Present(m_mode.CycleMode(m_presentedMode), pendingTime);
    }
}

template<class Mode, class ModePresenter>
void ModePinObserverBase<Mode, ModePresenter>::Tick(bool nestedTick) {
    if (nestedTick) {
        m_modePresenter.Tick();
    }
}

template<class Mode, class ModePresenter>
inline typename ModePinObserverBase<Mode, ModePresenter>::ModeT& ModePinObserverBase<Mode, ModePresenter>::GetMode() {
    return m_mode;
}

template<class Mode, class ModePresenter>
inline typename ModePinObserverBase<Mode, ModePresenter>::ModePresenterT& ModePinObserverBase<Mode, ModePresenter>::GetModePresenter()
{
    return m_modePresenter;
}

template<class Mode, class ModePresenter>
void ModePinObserverBase<Mode, ModePresenter>::OnPresentationFinish(ModeEnum presentedMode, bool modeChanged) {
    TR3(F("PF "), static_cast<uint8_t>(presentedMode), " ", static_cast<uint8_t>(modeChanged));
    if (modeChanged) {
        m_mode.SetMode(presentedMode);
    }
}
}
