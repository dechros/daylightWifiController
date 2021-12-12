#include "diagnosticTask.h"

void diagnosticTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    Serial.println("Task diagnosticTaskLoop created successfully");

    while (1)
    {
        if ((xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdTRUE))
        {
            Serial.println("xMutexDiagnosticState taken");

            if (machineID == "")
            {
                if (connectWithoutMachineID() == false)
                {
                    Serial.println("Cannot find network with ID: " + machineID);
                }
            }
            else
            {
                if (connectWithMachineID() == false)
                {
                    Serial.println("Cannot find network: /" + machineID + "/");
                }
            }
        }
        else
        {
            Serial.println("Waiting for Diagnostic WiFi Connection");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

bool connectWithMachineID()
{
    for (size_t i = 0; i < WiFi.scanNetworks(); i++)
    {
        String ssid = WiFi.SSID(i);
        if (WiFi.SSID(i) == "#D#" + machineID + "#")
        {
            char ssidArray[8];
            ssid.toCharArray(ssidArray, 8);
            WiFi.begin(ssidArray, ssidArray);
            while (WiFi.status() != WL_CONNECTED)
            {
                vTaskDelay(500);
                Serial.print(".");
            }
            return true;
        }
    }
    return false;
}

bool connectWithoutMachineID()
{
    for (size_t i = 0; i < WiFi.scanNetworks(); i++)
    {
        String ssid = WiFi.SSID(i);
        if (ssid[0] == '#' && ssid[1] == 'D' && ssid[2] == '#')
        {
            char ssidArray[8];
            ssid.toCharArray(ssidArray, 8);
            WiFi.begin(ssidArray, ssidArray);
            while (WiFi.status() != WL_CONNECTED)
            {
                vTaskDelay(500);
                Serial.print(".");
            }
            return true;
        }
    }
    return false;
}