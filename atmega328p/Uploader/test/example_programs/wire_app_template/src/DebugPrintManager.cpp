#include <DebugPrintManager.h>

#include <BuildInfo.h>
#include <Logger.h>
using namespace debug;

extern FirmwareMetadata buildInfo;

Logger* DbgPrint;

void initializeDebugPrint() {
  Serial.begin(115200);
  DbgPrint = &Logger::getInstance(Serial);
}

void printFirmwareInfo(Logger& dbg) {
  DbgPrint->printf("Firmware Version: %s\n", buildInfo.version);
  DbgPrint->printf("Build Date: %s\n", buildInfo.buildDate);
  DbgPrint->printf("Build Time: %s\n", buildInfo.buildTime);
}