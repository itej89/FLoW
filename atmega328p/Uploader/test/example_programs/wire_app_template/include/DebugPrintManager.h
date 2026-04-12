#pragma once

#include <Logger.h>

using namespace debug;

extern Logger* DbgPrint;

void initializeDebugPrint();
void printFirmwareInfo(Logger& dbg);