#ifndef DIAGNOSTIC_TASK_H
#define DIAGNOSTIC_TASK_H

#include <Arduino.h>
#include <EEPROM.h>


#include "Globals/globalVariables.h"
#include "Globals/definitions.h"

void diagnosticTaskLoop(void *pvParameters);
bool connectWithMachineID();
bool connectWithoutMachineID();

/**
 * @brief Reads TCP stream data.
 * 
 * @return String parsed TCP stream data?
 */
String readMachineIDdata();

/**
 * @brief Saves machine ID to EEPROM memory.
 * 
 * @param TCPmachineID Received machine ID from TCP stream
 * @return true Succesfull
 * @return false Fail
 */
bool saveMachineIDtoEEPROM(String TCPmachineID);

#endif