#include "globalVariables.h"

char *ssid = "83464349";
char *password = "83464349";
String machineID = "";
/* create a server and listen on port 8088 */
WiFiServer server(8088);
WiFiClient client;
SemaphoreHandle_t xMutexDiagnosticState;
SemaphoreHandle_t xMutexNormalState;
calismaModu calismaMod = HATA;

TaskHandle_t xMainTaskHandle = NULL;
TaskHandle_t xDiagnosticTaskHandle = NULL;
TaskHandle_t xNormalTaskHandle = NULL;

char *host = "192.168.1.31";
int port = 8088;