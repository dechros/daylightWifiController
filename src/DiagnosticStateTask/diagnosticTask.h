#ifndef DIAGNOSTIC_TASK_H
#define DIAGNOSTIC_TASK_H

#include <Arduino.h>
#include "Globals/globalVariables.h"

void diagnosticTaskLoop(void *pvParameters);
bool connectWithMachineID();
bool connectWithoutMachineID();

#endif