#include "diagnosticTask.h"

#include <bits/stdc++.h>

void diagnosticTaskLoop(void *pvParameters)
{
    configASSERT(((uint32_t)pvParameters) == 1);

    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("diagnosticTaskLoop: Task created successfully");
        xSemaphoreGive(xMutexConsole);
    }

    while (true)
    {
        if ((xSemaphoreTake(xMutexDiagnosticState, (TickType_t)10) == pdTRUE))
        {
            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
            {
                Serial.println("diagnosticTaskLoop: xMutexDiagnosticState taken");
                xSemaphoreGive(xMutexConsole);
            }

            if (machineID == "") /* State for empty Machine ID */
            {
                if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                {
                    Serial.println("diagnosticTaskLoop: MachineID empty state is active");
                    xSemaphoreGive(xMutexConsole);
                }

                while (true)
                {
                    if (connectWithoutMachineID() == true)
                    {
                        String tempString = readMachineIDdata();
                        if (saveMachineIDtoEEPROM(tempString) == true)
                        {
                            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                            {
                                Serial.println("diagnosticTaskLoop: Machince ID saved succesfully.");
                                xSemaphoreGive(xMutexConsole);
                            }
                            break;
                        }
                        else
                        {
                            if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                            {
                                Serial.println("diagnosticTaskLoop: Machince ID saving error!");
                                xSemaphoreGive(xMutexConsole);
                            }

                            while (true)
                            {
                                /* Error state. Needs restart. */
                            }
                        }
                    }
                    else
                    {
                        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                        {
                            Serial.println("diagnosticTaskLoop: Cannot connected any network for diagnostic. Trying again.");
                            xSemaphoreGive(xMutexConsole);
                        }
                    }
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
            }
            else /* State for previously saved Machine ID */
            {
                if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                {
                    Serial.println("diagnosticTaskLoop: MachineID previously saved state is active");
                    xSemaphoreGive(xMutexConsole);
                }
                if (connectWithMachineID() == false)
                {
                    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                    {
                        Serial.println("Cannot find network: /" + machineID + "/");
                        xSemaphoreGive(xMutexConsole);
                    }
                }
            }
            xSemaphoreGive(xMutexDiagnosticState);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

bool saveMachineIDtoEEPROM(String TCPmachineID)
{
    EEPROM.write(0, 35);
    EEPROM.write(9, 35);
    for (uint8_t i = 1; i < 9; i++)
    {
        EEPROM.write(i, TCPmachineID[i - 1] - 48);
    }
    EEPROM.commit();

    String tempMachineID = "";
    for (uint8_t i = 1; i < 9; i++)
    {
        tempMachineID = tempMachineID + EEPROM.read(i);
    }

    if (TCPmachineID == tempMachineID)
    {
        return true;
    }
    else
    {
        return false;
    }
}

String readMachineIDdata()
{
    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("readMachineIDdata: Trying to connect TCP server.");
        xSemaphoreGive(xMutexConsole);
    }
    while (client.connect(host, port) == false)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    String incomingData = "";
    while (client.connected() == true) /* Reading NEED_MACHINE_ID_DATA from TCP */
    {
        client.println(NEED_MACHINE_ID_DATA);

        if (client.available() == true)
        {
            uint8_t *data = new uint8_t[1];
            client.read(data, 1);
            incomingData = incomingData + (char *)data;

            int incomingDataLength = incomingData.length();
            if (incomingDataLength > 3)
            {
                String footer = footer +
                                incomingData[incomingDataLength - 3] +
                                incomingData[incomingDataLength - 2] +
                                incomingData[incomingDataLength - 1];

                if (footer == MESSAGE_FOOTER)
                {
                    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                    {
                        Serial.print("readMachineIDdata: incomingData = " + incomingData);
                        xSemaphoreGive(xMutexConsole);
                    }
                }
            }
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    return "89632145";
}

bool connectWithMachineID()
{
    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
    {
        Serial.println("connectWithMachineID: Not Connected");
        xSemaphoreGive(xMutexConsole);
    }
    int wifiCount = WiFi.scanNetworks();
    for (size_t i = 0; i < wifiCount; i++)
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
                if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                {
                    Serial.print(".");
                    xSemaphoreGive(xMutexConsole);
                }
            }
            return true;
        }
    }
    return false;
}

bool connectWithoutMachineID()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("connectWithoutMachineID: Scanning networks.");
            xSemaphoreGive(xMutexConsole);
        }
        WiFi.disconnect(false, true);
        WiFi.scanDelete();
        WiFi.scanNetworks(true, false, false, 100, 0);
        int wifiCount = 0;
        while (true)
        {
            vTaskDelay(1 / portTICK_PERIOD_MS);
            wifiCount = WiFi.scanComplete();
            if (wifiCount > 0)
            {
                break;
            }
        }
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("connectWithoutMachineID: Scan complete.");
            xSemaphoreGive(xMutexConsole);
        }

        for (size_t i = 0; i < wifiCount; i++)
        {
            if (WiFi.SSID(i).isEmpty() == false)
            {
                if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                {
                    Serial.println(WiFi.SSID(i));
                    xSemaphoreGive(xMutexConsole);
                }
            }
        }

        for (size_t i = 0; i < wifiCount; i++)
        {
            String ssid = WiFi.SSID(i);
            //if (ssid[0] == '#' && ssid[1] == 'D' && ssid[2] == '#')
            if (ssid[0] == 'D' && ssid[1] == 'I' && ssid[2] == 'A')
            {
                char *ssidArray = (char *)malloc(sizeof(char) * 12);
                ssid.toCharArray(ssidArray, 12);
                if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                {
                    Serial.println("connectWithoutMachineID: Trying to connect to: " + ssid);
                    xSemaphoreGive(xMutexConsole);
                }

                WiFi.mode(WIFI_STA);
                WiFi.begin((char *)"DIAGNOSTIC89632145", (char *)"DIAGNOSTIC");
                int connectionRetryCounter = 0;
                while (WiFi.status() != WL_CONNECTED)
                {
                    connectionRetryCounter++;
                    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                    {
                        Serial.print(WiFi.status());
                        xSemaphoreGive(xMutexConsole);
                    }

                    if (connectionRetryCounter >= WIFI_MAX_RETRY)
                    {
                        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                        {
                            Serial.println("connectWithoutMachineID: Reconnection is triggered: " + ssid);
                            xSemaphoreGive(xMutexConsole);
                        }
                        connectionRetryCounter = 0;
                        WiFi.reconnect();
                    }
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                if (WiFi.status() == WL_CONNECTED)
                {
                    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                    {
                        Serial.println("connectWithoutMachineID: Connected! " + ssid);
                        xSemaphoreGive(xMutexConsole);
                    }
                    return true;
                }
                else
                {
                    if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                    {
                        Serial.println("connectWithoutMachineID: Not Connected! " + ssid);
                        xSemaphoreGive(xMutexConsole);
                    }
                    return false;
                }
                if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
                {
                    Serial.println("connectWithoutMachineID: Undhandled error.");
                    xSemaphoreGive(xMutexConsole);
                }
            }
        }
        if (xSemaphoreTake(xMutexConsole, (TickType_t)10) == pdTRUE)
        {
            Serial.println("connectWithoutMachineID: Cannot find any actife WiFi.");
            xSemaphoreGive(xMutexConsole);
        }

        return false;
    }
    else
    {
        return true;
    }
}