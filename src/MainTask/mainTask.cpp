#include "mainTask.h"

void mainTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);
    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("mainTaskLoop: Task created successfully");
        xSemaphoreGive(xMutexConsole);
    }

    if (xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdFALSE)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: xMutexNormalState not taken!");
            xSemaphoreGive(xMutexConsole);
        }
        while (true)
        {
            /* Error. Needs restart. */
        }
    }

    if (xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdFALSE)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: xMutexDiagnosticState not taken!");
            xSemaphoreGive(xMutexConsole);
        }
        while (true)
        {
            /* Error. Needs restart. */
        }
    }

    if (EepromRead() == true)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: machineID = " + machineID);
            xSemaphoreGive(xMutexConsole);
        }
    }
    else
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: machineID is empty.");
            xSemaphoreGive(xMutexConsole);
        }

        xSemaphoreGive(xMutexDiagnosticState);
        TaskHandle_t whoOwnsDiagnosticMutex;
        if (xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: Waiting diagnostic task to take its mutex.");
            xSemaphoreGive(xMutexConsole);
        }

        while (true)
        {
            whoOwnsDiagnosticMutex = xSemaphoreGetMutexHolder(xMutexDiagnosticState);
            if (whoOwnsDiagnosticMutex == xDiagnosticTaskHandle)
            {
                break;
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: Waiting diagnostic task to release its mutex.");
            xSemaphoreGive(xMutexConsole);
        }

        vTaskDelay(3000 / portTICK_PERIOD_MS);
        while (xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdFALSE)
        {
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("mainTaskLoop: xMutexDiagnosticState is taken.");
            xSemaphoreGive(xMutexConsole);
        }
    }

    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("mainTaskLoop: Loop is started.");
        xSemaphoreGive(xMutexConsole);
    }
    while (true)
    {
        TaskHandle_t whoOwnsNormalMutex = xSemaphoreGetMutexHolder(xMutexNormalState);
        TaskHandle_t whoOwnsDiagnosticMutex = xSemaphoreGetMutexHolder(xMutexDiagnosticState);

        if (diagnosticScan() == true)
        {
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("diagnosticScan() == true");
                xSemaphoreGive(xMutexConsole);
            }

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
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("normalScan() == true");
                xSemaphoreGive(xMutexConsole);
            }
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
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("No WiFi, machineID: " + machineID);
                xSemaphoreGive(xMutexConsole);
            }
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
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("Found WiFi with diagnostic machineID: " + WiFi.SSID(i));
                xSemaphoreGive(xMutexConsole);
            }
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
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("Found WiFi with machineID: " + WiFi.SSID(i));
                xSemaphoreGive(xMutexConsole);
            }
            return true;
        }
    }
    return false;
}