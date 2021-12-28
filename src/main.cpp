#include <Arduino.h>

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

    EEPROMclear();

    if (EepromRead() == false)
    {
        Serial.println("Setup: EEPROM IS EMPTY! Entering Diagnostic State!");
        // if (WiFi.status() != WL_CONNECTED)

        int setupState = WiFi_SCAN_STATE;
        String ssid = "";
        int wifiCount = 0;
        uint8_t data[30];
        char ssidCharArray[19];
        while (setupState != END_OF_GIRIS_STATE_MACHINE)
        {
            switch (setupState)
            {
            case WiFi_SCAN_STATE:
            {
                bool diagnosticWiFiFound = false;
                wifiCount = WiFi.scanNetworks();
                Serial.println("Setup: Scanning Networks...");
                delay(3000);
                if (wifiCount > 0)
                {
                    for (size_t i = 0; i < wifiCount; i++)
                    {
                        String ssidSubstring = "";
                        ssid = WiFi.SSID(i);
                        ssidSubstring = ssid.substring(0, 10);
                        if (ssidSubstring == "DIAGNOSTIC")
                        {
                            diagnosticWiFiFound = true;
                            Serial.println("Setup: Found Diagnostic WiFi Network!");
                            break;
                        }
                    }
                }
                if (diagnosticWiFiFound == true)
                {
                    setupState = WiFi_CONNECT_STATE;
                    break;
                }
            }
            break;
            case WiFi_CONNECT_STATE:
            {
                ssid.toCharArray(ssidCharArray, 19);

                WiFi.mode(WIFI_STA);

                WiFi.begin((char *)ssidCharArray, (char *)ssidCharArray);
                int connectionRetryCounter = 0;
                Serial.println("Setup: Trying to connect Diagnostic WiFi Network... SSID: " + ssid);
                while (WiFi.status() != WL_CONNECTED)
                {
                    connectionRetryCounter++;

                    if (connectionRetryCounter >= WIFI_MAX_RETRY)
                    {
                        Serial.println("Setup: Reconnection is triggered. SSID: " + ssid);
                        connectionRetryCounter = 0;
                        WiFi.reconnect();
                    }
                    delay(500);
                }
                if (WiFi.status() == WL_CONNECTED)
                {
                    Serial.println("Setup: Connected to Diagnostic WiFi Network! SSID: " + ssid);
                    setupState = TCP_CONNECT_STATE;
                }
            }
            break;
            case TCP_CONNECT_STATE:
            {
                if (WiFi.status() == WL_CONNECTED)
                {
                    Serial.println("Setup: TCP/IP Connection starting...");

                    while (client.connect(host, port) == false)
                    {
                        Serial.println("Setup: Trying to Connect TCP/IP Server...");
                        /*
                            client.connect(host, port);
                            client.flush();
                            client.stop();
                        */
                       delay(500);
                    }

                    Serial.println("Setup: Connected TCP/IP Server!");
                    setupState = TCP_GET_MACHINEID_STATE;
                }
                else
                {
                    Serial.println("Setup: Lost WiFi Connection.");
                    setupState = WiFi_SCAN_STATE;
                }
            }
            break;
            case TCP_GET_MACHINEID_STATE:
            {
                if (WiFi.status() == WL_CONNECTED)
                {
                    if (client.connected() == false)
                    {
                        setupState = TCP_CONNECT_STATE;
                        break;
                    }
                    client.println(NEED_MACHINE_ID_DATA);
                    if (client.available())
                    {
                        Serial.println("Setup: Message Came!");
                        int incomingDataLength = client.read(data, 30);
                        char dataChar[30];
                        for (int i = 0; i < incomingDataLength; i++)
                        {
                            dataChar[i] = (char)data[i];
                        }
                        
                        String message = String(dataChar);
                        Serial.println("Setup: Message: " + message);
                        if (incomingDataLength < 30)
                        {
                            data[incomingDataLength] = '\0';
                        }
                        else
                        {
                            data[30] = '\0';
                        }

                        if (incomingDataLength > 6)
                        {
                            String header = header +
                                            (char)data[0] +
                                            (char)data[1] +
                                            (char)data[2];

                            String footer = footer +
                                            (char)data[incomingDataLength - 3] +
                                            (char)data[incomingDataLength - 2] +
                                            (char)data[incomingDataLength - 1];

                            Serial.println("Setup: header: " + header);
                            Serial.println("Setup: footer: " + footer);

                            if (header == MESSAGE_HEADER)
                            {
                                if (footer == MESSAGE_FOOTER)
                                {
                                    Serial.println("Setup: MachineID Received.");
                                    Serial.println((char *)data);
                                    setupState = EEPROM_MACHINEID_WRITE_STATE;
                                    client.stop();
                                }
                            }
                        }
                    }
                    delay(1000);
                }
                else
                {
                    Serial.println("Setup: Lost WiFi Connection.");
                    setupState = WiFi_SCAN_STATE;
                }
            }
            break;
            case EEPROM_MACHINEID_WRITE_STATE:
            {
                EEPROM.write(0, 35);
                EEPROM.write(9, 35);
                for (uint8_t i = 1; i < 9; i++)
                {
                    EEPROM.write(i, data[i + 2] - 48); /* Write int value not char value */
                }
                if (EEPROM.commit() == true)
                {
                    setupState = END_OF_GIRIS_STATE_MACHINE;
                }
                else
                {
                    setupState = ERROR_STATE;
                }
            }
            break;
            case ERROR_STATE:
            {
                Serial.println("Setup: Stuck in ERROR_STATE!");
            }
            break;
            }
        }

        while (1)
        {
            for (int i = 1; i < 9; i++)
            {
                Serial.print(EEPROM.read(i));
            }
            Serial.println("");
            delay(3000);
        }
    }

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
    // if (Serial.available())
    //{
    //     TaskHandle_t whoOwnsConsoleMutex = xSemaphoreGetMutexHolder(xMutexConsole);
    //     if (whoOwnsConsoleMutex != xLoopTaskHandle)
    //     {
    //         while (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdFALSE)
    //         {
    //             vTaskDelay(1 / portTICK_PERIOD_MS);
    //         }
    //     }
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

    // Serial.println("loop: Active");
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
    // while (client.connected())
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