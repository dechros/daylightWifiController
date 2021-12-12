#include "normalTask.h"

void normalTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    Serial.println("Task normalTaskLoop created successfully");

    while (1)
    {
        if ((xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdTRUE))
        {
            Serial.println("xMutexNormalState alındı");
        }
        else if ((xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdFALSE))
        {
            Serial.println("Waiting for NORMAL WiFi Connection");
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}