#include "globalVariables.h"

char *ssid = "83464349";
char *password = "83464349";
String machineID = "";
/* create a server and listen on port 8088 */
WiFiServer server(8088);
WiFiClient client;
SemaphoreHandle_t xMutexDiagnosticState;
SemaphoreHandle_t xMutexNormalState;
SemaphoreHandle_t xMutexConsole;
calismaModu calismaMod = HATA;

TaskHandle_t xMainTaskHandle = NULL;
TaskHandle_t xDiagnosticTaskHandle = NULL;
TaskHandle_t xNormalTaskHandle = NULL;
TaskHandle_t xConsoleTaskHandle = NULL;

char *host = "192.168.1.31";
int port = 8088;

void EEPROMclear()
{
    for (int i = 0; i < 512; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

void breakPoint(String consoleMessage)
{
    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.print(consoleMessage);
        xSemaphoreGive(xMutexConsole);
    }
    getchar();
}