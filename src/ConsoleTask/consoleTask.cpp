#include "consoleTask.h"

void consoleTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("consoleTaskLoop: Task created successfully");
        xSemaphoreGive(xMutexConsole);
    }

    while (true)
    {
        if (Serial.available())
        {
            TaskHandle_t whoOwnsConsoleMutex = xSemaphoreGetMutexHolder(xMutexConsole);
            if (whoOwnsConsoleMutex != xConsoleTaskHandle)
            {
                while (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdFALSE)
                {
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }
            }

            Serial.println("consoleTaskLoop: Incoming Data");
            String serialRead = Serial.readString();
            if (serialRead == "1")
            {
                EEPROMclear();
                Serial.println("consoleTaskLoop: EEPROM Cleared.");
            }

            if (serialRead == "31")
            {
                xSemaphoreGive(xMutexConsole);
            }
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    
}
