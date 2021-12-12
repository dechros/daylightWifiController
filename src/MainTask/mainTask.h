#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Globals/globalVariables.h"

void mainTaskLoop(void *pvParameters);
bool EepromRead();
bool diagnosticScan();
bool normalScan();

#endif