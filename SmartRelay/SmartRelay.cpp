#include "avr/sleep.h"
#include "SoftwareSerial.h"
#include "SerialDbgUtils.h"
#include "ExtIntPinMonitor.h"
#include "ModePinObserver.h"
#include "PinObserverTrigger.h"
#include "Mode.h"
#include "ModePresenter.h"
#include "SerialCommands.h"

using namespace NsPinsMonitor;
using namespace SmartRelay;
using namespace NsPins;
using namespace NsPinObserverTrigger;

/************************************************************************/
/* Used pins                                                            */
/* PIN0[IN]: Serial Rx (for using analog external interrupt)            */
/* PIN1[OUT]: led pin (Digispark dev board)                             */
/* PIN2[IN_PULLUP]: mode change push button pin (sole ext. int. pin)    */
/* PIN3[OUT]: Serial Tx                                                 */
/* PIN4[OUT]: output relay drive (drives the actual load)               */
/* PIN5[IN]: unused - reserver for reset/debugWire                      */
/************************************************************************/

using ModePin = InputDigitalPin<PIN_PB2, INPUT_PULLUP>;
using LedBlinkPin = ToggleOutputDigitalPin<PIN_PB1>;
using RelayDrivePin = OutputDigitalPin<PIN_PB4>;

using Mode = ModePersistentBase<RelayDrivePin>;

//reduced durations for quicker debugging in SIMULATOR
#ifdef SIMULATION 
    using ModePresenter = ModePresenterBase<LedBlinkPin, Mode, ModePinObserverBase, 3, 10>;
    using ModePinObserver = ModePinObserverBase<Mode, ModePresenter>;
    using PinObserverTrigger = PinObserverTriggerDebouncedBase<ModePinObserver, ModePin, ExtIntPinLowMonitorBase, 5>;
#else
    using ModePresenter = ModePresenterBase<LedBlinkPin, Mode, ModePinObserverBase>;
    using ModePinObserver = ModePinObserverBase<Mode, ModePresenter>;
    using PinObserverTrigger = PinObserverTriggerDebouncedBase<ModePinObserver, ModePin, ExtIntPinLowMonitorBase>;
#endif

using ModePinMonitor = ExtIntPinLowMonitorBase<ModePin, ModePinObserver, PinObserverTrigger>;

ModePinMonitor g_modePinMonitor = {PinObserverTrigger(ModePinObserver(Mode(), ModePresenter()))};

SoftwareSerial Serial = {PIN_PB0, PIN_PB3};

SerialCommandsBase<SoftwareSerial, Mode> g_serialCommand = {Serial, g_modePinMonitor.GetPinObserverTrigger().GetPinObserver().GetMode()};

void setup() {
    {//init rest pin as INPUT_PULLUP
        InputDigitalPin<PB5> resetPin;
    }
    Serial.begin(115200); //tx - PIN_PB0, rx - PIN_PB1
    ModePinObserver& pinObserver = g_modePinMonitor.GetPinObserverTrigger().GetPinObserver();
    pinObserver.GetModePresenter().AssociatePresenterObserver(&pinObserver);
    TR1(F("S "), static_cast<uint8_t>(g_modePinMonitor.GetPinObserverTrigger().GetPinObserver().GetMode().GetModeEnum()));
    TR1(F("PCS "), static_cast<uint8_t>(g_modePinMonitor.PinChanged()));
    TR1(F("MPS "), digitalRead(PIN_PB2));
    
    //setup the power management
    noInterrupts();
    //Power saving
    // disable ADC. Analog pins will be unavailable
    ADCSRA = 0;
    //set the power down sleep mode. The INT1, INT0 and Pin Change interrupts will still wake the CPU
    //According to http://www.gammon.com.au/power the power consumption should drop to 0.36 mA
//     set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    set_sleep_mode (SLEEP_MODE_IDLE); //the timers/counters works only in normal or power idle mode. For the rest (ADC and Power Down) watchdog timers
                                      //have to be used
    sleep_enable (); // enables the sleep bit in the mcucr register
    interrupts();
    sleep_cpu();
}

// the loop function runs over and over again, forever
void loop() {
//     TR(".");
    g_modePinMonitor.Tick();
    g_serialCommand.Tick();
    sleep_cpu();
}
