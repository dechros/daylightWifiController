#include <Arduino.h>
#include <WiFi.h>

#include "Globals/definitions.h"
#include "Globals/globalVariables.h"
#include "MainTask/mainTask.h"
#include "DiagnosticStateTask/diagnosticTask.h"
#include "NormalStateTask/normalTask.h"
#include "ConsoleTask/consoleTask.h"

void setup()
{
    EEPROM.begin(EEPROM_SIZE);
    Serial.begin(SERIAL_BAUDRATE);

    BaseType_t xReturnedTaskCreationVal;

    xMutexNormalState = xSemaphoreCreateMutex();
    xMutexDiagnosticState = xSemaphoreCreateMutex();
    xMutexConsole = xSemaphoreCreateMutex();

    xReturnedTaskCreationVal = xTaskCreate(mainTaskLoop, "MAIN", 2048, (void *)1, 32, &xMainTaskHandle);
    if (xReturnedTaskCreationVal == pdFALSE)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("setup: Main task creation unseuccesfull!");
            xSemaphoreGive(xMutexConsole);
        }

        while (true)
        {
            /* Error. Needs restart. */
        }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); /* Waiting for main task loop to take semaphores. */

    xReturnedTaskCreationVal = xTaskCreate(diagnosticTaskLoop, "DIAGNOSTIC", 4096, (void *)1, 31, &xDiagnosticTaskHandle);
    if (xReturnedTaskCreationVal == pdFALSE)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("setup: Diagnostic task creation unseuccesfull!");
            xSemaphoreGive(xMutexConsole);
        }

        while (true)
        {
            /* Error. Needs restart. */
        }
    }

    xReturnedTaskCreationVal = xTaskCreate(normalTaskLoop, "NORMAL", 2048, (void *)1, 30, &xNormalTaskHandle);
    if (xReturnedTaskCreationVal == pdFALSE)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("setup: Normal task creation unsuccesfull!");
            xSemaphoreGive(xMutexConsole);
        }

        while (true)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    xReturnedTaskCreationVal = xTaskCreate(consoleTaskLoop, "CONSOLE", 2048, (void *)1, 29, &xConsoleTaskHandle);
    if (xReturnedTaskCreationVal == pdFALSE)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("setup: Console task creation unsuccesfull!");
            xSemaphoreGive(xMutexConsole);
        }

        while (true)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    //  server.begin();
    //  while (!client.connect(host, port))
    //  {
    //      Serial.println("connection failed");
    //      delay(5000);
    //  }
    //
    //  Serial.print("connected to host");
    //  Serial.println(host);
}

void loop()
{
    //if (Serial.available())
    //{
    //    TaskHandle_t whoOwnsConsoleMutex = xSemaphoreGetMutexHolder(xMutexConsole);
    //    if (whoOwnsConsoleMutex != xLoopTaskHandle)
    //    {
    //        while (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdFALSE)
    //        {
    //            vTaskDelay(1 / portTICK_PERIOD_MS);
    //        }
    //    }
//
    //    Serial.println("loop: Incoming Data");
    //    String serialRead = Serial.readString();
    //    if (serialRead == "1")
    //    {
    //        EEPROMclear();
    //        Serial.println("loop: EEPROM Cleared.");
    //    }
//
    //    if (serialRead == "31")
    //    {
    //        xSemaphoreGive(xMutexConsole);
    //    }
    //}

    //Serial.println("loop: Active");
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // WiFiClient client = server.available();
    //   uint8_t data[30];
    //   if (client) {
    //     Serial.println("new client");
    //     /* check client is connected */
    //     while (client.connected()) {
    //         if (client.available()) {
    //             int len = client.read(data, 30);
    //             if(len < 30){
    //                 data[len] = '\0';
    //             }else {
    //                 data[30] = '\0';
    //             }
    //             Serial.print("client sent: ");
    //             Serial.println((char *)data);
    //         }
    //     }
    //   }
    //while (client.connected())
    //{
    //    delay(1500);
    //
    //    /* This will send the data to the server */
    //    client.println("hello world");
    //    if (client.available())
    //    {
    //
    //        uint8_t data[30];
    //        int len = client.read(data, 30);
    //        if (len < 30)
    //        {
    //            data[len] = '\0';
    //        }
    //        else
    //        {
    //            data[30] = '\0';
    //        }
    //        Serial.print("client sent: ");
    //        Serial.print((char *)data);
    //    }
    //}
    // client.stop();
}