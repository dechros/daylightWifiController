#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <Arduino.h>
#include <WiFi.h>

#include "enums.h"

extern char *ssid;
extern char *password;
extern String machineID;
extern WiFiServer server;
extern WiFiClient client;
extern char *host;
extern int port;

extern SemaphoreHandle_t xMutexDiagnosticState;
extern SemaphoreHandle_t xMutexNormalState;

extern TaskHandle_t xMainTaskHandle;
extern TaskHandle_t xDiagnosticTaskHandle;
extern TaskHandle_t xNormalTaskHandle;

extern calismaModu calismaMod;

#endif