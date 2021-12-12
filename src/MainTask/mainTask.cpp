#include "mainTask.h"

void mainTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    Serial.println("Task mainTaskLoop created successfully");

    if (xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdFALSE)
    {
        Serial.println(" xMutexNormalState Alamadım");
        while (1)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        Serial.println("xMutexNormalState ALDIM ALDIM");
    }

    if (xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdFALSE)
    {
        Serial.println(" xMutexDiagnosticState Alamadım");
        while (1)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        Serial.println("xMutexDiagnosticState ALDIM ALDIM");
    }

    if (EepromRead() == true)
    {
        Serial.println("machineID: " + machineID);
    }
    else
    {
        xSemaphoreGive(xMutexDiagnosticState);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        while (xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdFALSE)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    while (true)
    {
        Serial.println("Wheil 1");
        TaskHandle_t whoOwnsNormalMutex = xSemaphoreGetMutexHolder(xMutexNormalState);
        TaskHandle_t whoOwnsDiagnosticMutex = xSemaphoreGetMutexHolder(xMutexDiagnosticState);

        if (diagnosticScan() == true)
        {
            Serial.println("diagnosticScan() == true");
            if (whoOwnsNormalMutex != xMainTaskHandle)
            {
                while (xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdFALSE)
                {
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
            xSemaphoreGive(xMutexDiagnosticState);

            calismaMod = DIAGNOSTIC;
        }
        else if (normalScan() == true)
        {
            Serial.println("normalScan() == true");
            if (whoOwnsDiagnosticMutex != xMainTaskHandle)
            {
                while (xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdFALSE)
                {
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
            xSemaphoreGive(xMutexNormalState);

            calismaMod = NORMAL;
        }
        else
        {
            Serial.println("No WiFi, machineID: " + machineID);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

bool EepromRead()
{
    if (EEPROM.read(0) == 35 && EEPROM.read(9) == 35)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            machineID = machineID + EEPROM.read(i + 1);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool diagnosticScan()
{

    for (size_t i = 0; i < WiFi.scanNetworks(); i++)
    {
        if (WiFi.SSID(i) == "#D#" + machineID + "#")
        {
            Serial.println("Found WiFi with diagnostic machineID: " + WiFi.SSID(i));
            return true;
        }
    }
    return false;
}

bool normalScan()
{

    for (size_t i = 0; i < WiFi.scanNetworks(); i++)
    {
        if (WiFi.SSID(i) == machineID)
        {
            Serial.println("Found WiFi with machineID: " + WiFi.SSID(i));
            return true;
        }
    }
    return false;
}