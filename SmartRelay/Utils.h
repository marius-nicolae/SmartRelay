#pragma once
#include "TypesUtils.h"

namespace NsUtils {
using namespace NsTypesUtils;

void MillisDelay(uint delay);
bool Passed(ulong timeSpan, ulong since); //return true if "timeSpan" milliseconds have passed since "since"
}