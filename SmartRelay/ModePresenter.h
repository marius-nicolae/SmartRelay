#pragma once
#include "LedBlinkModePresenter.h"

namespace SmartRelay {
using namespace NsModePresenter;
    
template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver,
uint8_t cyclesNo = 3, uint cycleDuration = 1000>
class ModePresenterBase: public LedBlinkModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver,
    ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>, cyclesNo, cycleDuration> {
public:
    using LedBlinkPhasesRatiosT = LedBlinkPhasesRatiosBase<2>;
    
    const LedBlinkPhasesRatiosT& operator [](const ModeEnum mode) const;
private:
    static LedBlinkPhasesRatiosT m_serialDrivenLedBlinkPhasesRatios;
    static LedBlinkPhasesRatiosT m_alwaysOnLedBlinkPhasesRatios;
    static LedBlinkPhasesRatiosT m_alwaysOffLedBlinkPhasesRatios;
};

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, uint8_t cyclesNo, uint cycleDuration>
typename ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::LedBlinkPhasesRatiosT 
    ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::m_serialDrivenLedBlinkPhasesRatios = {1, 1};
    
template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, uint8_t cyclesNo, uint cycleDuration>
typename ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::LedBlinkPhasesRatiosT 
    ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::m_alwaysOnLedBlinkPhasesRatios = {4, 1};
    
template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, uint8_t cyclesNo, uint cycleDuration>
typename ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::LedBlinkPhasesRatiosT 
    ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::m_alwaysOffLedBlinkPhasesRatios = {1, 4};

template<class LedToggleableOutPin, class Mode, template <class, class> class PresenterObserver, uint8_t cyclesNo, uint cycleDuration>
const typename ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::LedBlinkPhasesRatiosT& 
    ModePresenterBase<LedToggleableOutPin, Mode, PresenterObserver, cyclesNo, cycleDuration>::operator [](const ModeEnum mode) const
{
    switch(mode) {
    case ModeEnum::serialDriven:
        return m_serialDrivenLedBlinkPhasesRatios;
    case ModeEnum::alwaysOn:
        return m_alwaysOnLedBlinkPhasesRatios;
    case ModeEnum::alwaysOff:
        return m_alwaysOffLedBlinkPhasesRatios;
    default:
        ASSERT(false); //should never happen
    }
}
}
