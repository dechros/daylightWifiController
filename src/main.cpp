#include <Arduino.h>
#include <WiFi.h>

#include "Globals/definitions.h"
#include "Globals/globalVariables.h"
#include "MainTask/mainTask.h"
#include "DiagnosticStateTask/diagnosticTask.h"
#include "NormalStateTask/normalTask.h"

void setup()
{
    xMutexNormalState = xSemaphoreCreateMutex();
    xMutexDiagnosticState = xSemaphoreCreateMutex();

    EEPROM.begin(EEPROM_SIZE);
    
    Serial.begin(SERIAL_BAUDRATE);

    BaseType_t xReturnedMainTask;

    xReturnedMainTask = xTaskCreate(mainTaskLoop, "MAIN", 2048, (void *)1, 32, &xMainTaskHandle);

    if (xReturnedMainTask == pdFALSE)
    {
        while (1)
        {
            /* Error. Needs restart. */
        }
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    BaseType_t xReturnedDiagnosticTask;

    xReturnedDiagnosticTask = xTaskCreate(diagnosticTaskLoop, "DIAGNOSTIC", 2048, (void *)1, 31, &xDiagnosticTaskHandle);

    if (xReturnedDiagnosticTask == pdFALSE)
    {
        while (1)
        {
            /* Error. Needs restart. */
        }
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    BaseType_t xReturnedNormalTask;

    xReturnedNormalTask = xTaskCreate(normalTaskLoop, "NORMAL", 2048, (void *)1, 30, &xNormalTaskHandle);

    if (xReturnedNormalTask == pdFALSE)
    {
        while (1)
        {
            Serial.println("NORMAL TASK CREATE ERROR");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    // Serial.begin(9600);
    //   Serial.print("Connecting to ");
    //   Serial.println(ssid);
    //   /* connecting to WiFi */
    //   WiFi.begin(ssid, password);
    //   /*wait until ESP32 connect to WiFi*/
    //   while (WiFi.status() != WL_CONNECTED) {
    //       delay(500);
    //       Serial.print(".");
    //   }
    //   Serial.println("");
    //   Serial.println("WiFi connected with IP address: ");
    //   Serial.println(WiFi.localIP());
    //   /* start Server */
    //   server.begin();

    
  //  Serial.print("Connecting to ");
  //  Serial.println(ssid);
  //  /* connect to your WiFi */
  //  WiFi.begin(ssid, password);
  //  /* wait until ESP32 connect to WiFi*/
  //  Serial.println(WiFi.broadcastIP());
  //  while (WiFi.status() != WL_CONNECTED)
  //  {
  //      delay(500);
  //      Serial.print(".");
  //  }
  //  Serial.println("");
  //  Serial.println("WiFi connected with IP address: ");
  //  Serial.println(WiFi.localIP());
//
  //  Serial.print("connecting to ");
  //  Serial.println(host);
  //  /* Use WiFiClient class to create TCP connections */
//
  //  while (!client.connect(host, port))
  //  {
  //      Serial.println("connection failed");
  //      delay(5000);
  //  }
//
  //  Serial.print("connected to host");
  //  Serial.println(host);
}//

void loop()
{
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    Serial.println("-------------------------");

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
    
    //vector<int> a;




/*
    int wifiCount = WiFi.scanNetworks();

    Serial.println("wifi count: " + wifiCount);

    for (size_t i = 0; i < wifiCount; i++)
    {
        Serial.println(WiFi.SSID(i));
        if (WiFi.SSID(i) == "83464349")
        {
            Serial.println("GOTCHU BITCH");
            Serial.println("////");
        }
    }
    delay (3000);
*/

/*
    uint8_t machineID[8] = {8,9,6,3,2,1,4,5};
    EEPROM.write(0, 35);
    EEPROM.write(9, 35);
    for (uint8_t i = 1; i < 9; i++)
    {
        EEPROM.write(i, machineID[i-1]);
    }
    EEPROM.commit();

    for (uint8_t i = 0; i < 10; i++)
    {
        Serial.println(EEPROM.read(i));
    }
    Serial.println("/");

    delay(3000);
*/


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