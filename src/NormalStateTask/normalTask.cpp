#include "normalTask.h"

void normalTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("normalTaskLoop: Task created successfully");
        xSemaphoreGive(xMutexConsole);
    }

    while (true)
    {
        if ((xSemaphoreTake(xMutexNormalState, (TickType_t)10) == pdTRUE))
        {
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("normalTaskLoop: xMutexNormalState alindi");
                xSemaphoreGive(xMutexConsole);
            }
        }
        xSemaphoreGive(xMutexNormalState);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}