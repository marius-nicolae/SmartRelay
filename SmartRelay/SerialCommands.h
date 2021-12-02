#pragma once
#include "TypesUtils.h"
#include "WString.h"

namespace SmartRelay {
using namespace NsTypesUtils;

template<class Serial, class Mode>
class SerialCommandsBase {
public:
    using SerialT = typename RemoveReference<Serial>::Type;
    using ModeT = typename RemoveReference<Mode>::Type;
    using SerialCommands = SerialCommandsBase<Serial, Mode>;
    
    SerialCommandsBase(SerialT& serial, ModeT& mode);
    void Tick();
private:
    bool HandleGetModeCommand();
    bool HandleSetModeCommand();
    bool HandleGetRelayStateCommand();
    bool HandleSetRelayStateCommand();
    void Reset();
    char Read();
    void ParseCmd();
    bool ConsumeUpToChar(char chr = m_eol);
    bool ConsumeUpToEol(); //also set eolReached bit
    bool GetSoleArgChr(); //get the first char as argument and ignore the rest, up to eol
    
    SerialT& m_serial;
    ModeT& m_mode;
    constexpr static uint8_t m_argBufLen = 1;
    char m_argBuf[m_argBufLen];
    
    enum class ParseState:uint8_t {
        cmd = 0,
        args = 1,
        invalid = 2,
    };
    ParseState m_parseState = ParseState::cmd;
    uint8_t m_cmdParseIdx = 0;
    
    struct CmdParseHints {
        uint8_t set: 1, //0 for get and 1 for set cmds
                postArg : 1, //0: search for the first arg char 1: consume post argument characters
                eolReached : 1;
    };
    CmdParseHints m_cmdParseHints = {0, 0, 0};
    bool (SerialCommandsBase::*m_pHndMethod)() = nullptr;
    char m_argChr = '\0';
    constexpr static char m_eol = '\r';
};

//============================ SerialCommands =================
template<class Serial, class Mode>
SerialCommandsBase<Serial, Mode>::SerialCommandsBase(SerialT& serial, ModeT& mode): m_serial(serial), m_mode(mode) {
}

template<class Serial, class Mode>
void SerialCommandsBase<Serial, Mode>::Tick() {
    switch (m_parseState) {
    case ParseState::cmd:
        ParseCmd();
    	break;
    case ParseState::args:
        if ((this->*m_pHndMethod)()) {
            Reset();
        }            
    	break;
    case ParseState::invalid:
        if (m_cmdParseHints.eolReached || ConsumeUpToChar(m_eol)) {
            m_serial.println("Err");
            Reset();
        }
        break;
    }
}

template<class Serial, class Mode>
void SerialCommandsBase<Serial, Mode>::ParseCmd() {
    while(m_serial.available()) {
        char chr = Read();
        TR3(F("PCMD "), chr, " ", m_cmdParseIdx);
        switch(m_cmdParseIdx) {
        case 0: //first cmd character
            switch (chr) {
            case 's':
                m_cmdParseHints.set = 1;
        	    break;
            case 'g':
                break;
            case m_eol://empty command
            case ' ':
                continue;
            default:
                m_parseState = ParseState::invalid;
                return;
            }
            break;
        case 1: //second cmd character
            switch (chr)
            {
            case 'm':
                m_parseState = ParseState::args;
                m_pHndMethod = m_cmdParseHints.set? &SerialCommands::HandleSetModeCommand : &SerialCommands::HandleGetModeCommand;
                return;
            case 'r':
                break;
            default:
                m_parseState = ParseState::invalid;
                if (chr == m_eol) {
                    m_cmdParseHints.eolReached = 1;
                }
                return;
            }
            break;
        case 2: //third cmd character
            if (chr != 's') {
                m_parseState = ParseState::invalid;
                if (chr == m_eol) {
                    m_cmdParseHints.eolReached = 1;
                }
                return;
            }
            m_parseState = ParseState::args;
            m_pHndMethod = m_cmdParseHints.set? &SerialCommands::HandleSetRelayStateCommand : &SerialCommands::HandleGetRelayStateCommand;
            return;
        }
        m_cmdParseIdx++;
    }        
}

template<class Serial, class Mode>
char SerialCommandsBase<Serial, Mode>::Read() {
    return m_serial.read() & 0x7f; //sometimes the most significant bit is wrongly set
}

template<class Serial, class Mode>
void SerialCommandsBase<Serial, Mode>::Reset() {
    TR(F("SCRST"));
    m_parseState = ParseState::cmd;
    m_cmdParseIdx = 0;
    m_cmdParseHints = {0, 0, 0};
    m_argChr = '\0';
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::HandleGetModeCommand() {
//     TR(F("SC GM"));
    if (ConsumeUpToChar(m_eol)) { //ignore all the rest of chars, up to eol
        m_serial.println(static_cast<uint8_t>(m_mode.GetModeEnum()));
        return true;
    } else {
        return false;
    }
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::HandleSetModeCommand() {
//     TR1(F("SC SM "), static_cast<uint8_t>(m_parseState));
    if (GetSoleArgChr() && m_parseState == ParseState::args) {
        TR1(F("SC SM1 "), m_argChr);
        switch (m_argChr) {
        case '0':
            m_mode.SetMode(ModeEnum::serialDriven);
            break;
        case '1':
            m_mode.SetMode(ModeEnum::alwaysOn);
            break;
        case '2':
            m_mode.SetMode(ModeEnum::alwaysOff);
            break;
        default:
            m_parseState = ParseState::invalid;
//             TR3(F("SC SM2 "), static_cast<uint8_t>(m_cmdParseHints.eolReached), " ", static_cast<uint8_t>(m_parseState));
            return false;
        }
        m_serial.println("OK");
        return true;        
    } else {
        return false;
    }        
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::HandleGetRelayStateCommand() {
//     TR(F("SC GR"));
    if (ConsumeUpToChar(m_eol)) {
        m_serial.println(m_mode.GetOutPin().IsHigh()? '1' : '0');
        return true;
    } else {
        return false;
    }
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::HandleSetRelayStateCommand() {
    if (GetSoleArgChr() && m_parseState == ParseState::args) {
        TR1(F("SC SR "), m_argChr);
        if (m_mode.GetModeEnum() == ModeEnum::serialDriven) { //set relay mode make sense only on serialDriven mode
            switch (m_argChr) {
            case '0':
               m_mode.SetPinLow();
                break;
            case '1':
                m_mode.SetPinHigh();
                break;
            default:
                m_parseState = ParseState::invalid;
                return false;
            }
            m_serial.println("OK");
            return true;
        } else {
            m_parseState = ParseState::invalid;
            return false;
        }            
    } else {
    return false;
    }
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::GetSoleArgChr() {
    if (!m_cmdParseHints.postArg) {
        while(m_serial.available()) {
            char chr = Read();
            TR1(F("SC GAC "), chr);
            switch (chr) {
            case ' ':
                continue;
            case m_eol:
                m_parseState = ParseState::invalid;
                m_cmdParseHints.eolReached = 1;
                return false;
            default:
                m_argChr = chr;
                m_cmdParseHints.postArg = 1; //for consuming all the chars, up to eol
                return false;
            }         
        }
    } else {
        return ConsumeUpToEol();
    }
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::ConsumeUpToChar(char chr) {
    while(m_serial.available()) {
        char readChr = Read();
        TR3(F("SC CC "), static_cast<uint8_t>(readChr), " ", static_cast<uint8_t>(chr));
        if (readChr == chr) {
            return true;
        }
    }
    return false;
}

template<class Serial, class Mode>
bool SerialCommandsBase<Serial, Mode>::ConsumeUpToEol() {
    if (ConsumeUpToChar(m_eol)) {
        m_cmdParseHints.eolReached = 1;
        TR(F("CSEOL"));
        return true;
    } else {
        return false;
    }
}
}