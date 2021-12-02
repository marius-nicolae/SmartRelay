#pragma once
#ifdef _TRACE
#include "Arduino.h"

template<typename T>
inline void _Trace(T t) {
    Serial.println(t);
}

template<typename T, typename... Ts>
inline void _Trace(T t, Ts... ts) {
    Serial.print(t);
    _Trace(ts...);
}

//be aware not to call any TR[<no>] in global objects constructor or its calees (functions or methods) since the Serial object is not initialized yet
#define TR(p0) _Trace(p0)
#define TR1(p0, p1) _Trace(p0, p1)
#define TR2(p0, p1, p2) _Trace(p0, p1, p2)
#define TR3(p0, p1, p2, p3) _Trace(p0, p1, p2, p3)
#define TR4(p0, p1, p2, p3, p4) _Trace(p0, p1, p2, p3, p4)
#define TR5(p0, p1, p2, p3, p4, p5) _Trace(p0, p1, p2, p3, p4, p5)
#define TR6(p0, p1, p2, p3, p4, p5, p6) _Trace(p0, p1, p2, p3, p4, p5, p6)
#define TR7(p0, p1, p2, p3, p4, p5, p6, p7) _Trace(p0, p1, p2, p3, p4, p5, p6, p7)
#define TR8(p0, p1, p2, p3, p4, p5, p6, p7, p8) _Trace(p0, p1, p2, p3, p4, p5, p6, p7, p8)
#define TR9(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) _Trace(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define TR_NUM(num, base) Serial.println(num, base)

//be aware not to call any ASSERT in global objects constructor or its calees (functions or methods) since the Serial object is not initialized yet
#define ASSERT(cond) if (!(cond)) { _Trace(F("AS F "), __func__, "@", __FILE__, ":", __LINE__); abort(); }
#define ASSERT_MSG(cond, msg) if (!(cond)) { _Trace(F("AS F "), __func__, "@", __FILE__, ":", __LINE__, ": ", msg); abort(); }
#define ASSERT_MINIMAL(cond, msg) if (!(cond)) { _Trace(F("AS F "), msg); abort(); }
    
    
//WARNING: serial printing from ISRs can cause unexpected behavior such as sudden reset in the middle of print or immediately re-calling the ISR with crazy variables values
//Activate the _ISR_TRACE and use the ISR_TR<no> with caution, only shortly to debug ISRs although they should be simple enough
#ifdef _ISR_TRACE
#define ISR_TR(traceStr) TR(traceStr)
#define ISR_TR1(traceStr, p1) TR1(traceStr, p1)
#define ISR_TR2(traceStr, p1, p2) TR2(traceStr, p1, p2)
#define ISR_TR3(traceStr, p1, p2, p3) TR3(traceStr, p1, p2, p3)
#define ISR_TR4(traceStr, p1, p2, p3) TR4(traceStr, p1, p2, p3)
#define ISR_TR5(traceStr, p1, p2, p3, p4, p5) TR5(traceStr, p1, p2, p3, p4, p5)
#define ISR_TR6(traceStr, p1, p2, p3, p4, p5, p6) TR6(traceStr, p1, p2, p3, p4, p5, p6)
#define ISR_TR7(traceStr, p1, p2, p3, p4, p5, p6, p7) TR7(traceStr, p1, p2, p3, p4, p5, p6, p7)
#define ISR_TR8(traceStr, p1, p2, p3, p4, p5, p6, p7, p8) TR8(traceStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ISR_TR9(traceStr, p1, p2, p3, p4, p5, p6, p7, p8, p9) TR9(traceStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#else
#define ISR_TR(traceStr)
#define ISR_TR1(traceStr, p1)
#define ISR_TR2(traceStr, p1, p2)
#define ISR_TR3(traceStr, p1, p2, p3)
#define ISR_TR4(traceStr, p1, p2, p3)
#define ISR_TR5(traceStr, p1, p2, p3, p4, p5)
#define ISR_TR6(traceStr, p1, p2, p3, p4, p5, p6)
#define ISR_TR7(traceStr, p1, p2, p3, p4, p5, p6, p7)
#define ISR_TR8(traceStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ISR_TR9(traceStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#endif

#else //_TRACE
#define  TR(p0)
#define TR1(p0, p1)
#define TR2(p0, p1, p2)
#define TR3(p0, p1, p2, p3)
#define TR4(p0, p1, p2, p3, p4)
#define TR5(p0, p1, p2, p3, p4, p5)
#define TR6(p0, p1, p2, p3, p4, p5, p6)
#define TR7(p0, p1, p2, p3, p4, p5, p6, p7)
#define TR8(p0, p1, p2, p3, p4, p5, p6, p7, p8)
#define TR9(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define TR_NUM(num, base)

#define ASSERT(cond)
#define ASSERT_MSG(cond, msg)
#define ASSERT_MINIMAL(cond, msg)

#define  ISR_TR(p0)
#define ISR_TR1(p0, p1)
#define ISR_TR2(p0, p1, p2)
#define ISR_TR3(p0, p1, p2, p3)
#define ISR_TR4(p0, p1, p2, p3)
#define ISR_TR5(p0, p1, p2, p3, p4, p5)
#define ISR_TR6(p0, p1, p2, p3, p4, p5, p6)
#define ISR_TR7(p0, p1, p2, p3, p4, p5, p6, p7)
#define ISR_TR8(p0, p1, p2, p3, p4, p5, p6, p7, p8)
#define ISR_TR9(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#endif //_TRACE
