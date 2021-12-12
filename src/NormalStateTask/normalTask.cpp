#include "normalTask.h"

void normalTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("Task normalTaskLoop created successfully");
        xSemaphoreGive(xMutexConsole);
    }

    while (true)
    {
        if ((xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdTRUE))
        {
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("normalTaskLoop: xMutexNormalState alındı");
                xSemaphoreGive(xMutexConsole);
            }
        }
        xSemaphoreGive(xMutexNormalState);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}