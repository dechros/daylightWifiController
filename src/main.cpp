#include <Arduino.h>
#include <WiFi.h>
const char *ssid = "83464349";
const char *password = "83464349";
/* create a server and listen on port 8088 */
WiFiServer server(8088);
WiFiClient client;

const char *host = "192.168.1.31";
const int port = 8088;

void setup()
{
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

    Serial.begin(9600);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    /* connect to your WiFi */
    WiFi.begin(ssid, password);
    /* wait until ESP32 connect to WiFi*/
    Serial.println(WiFi.broadcastIP());
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected with IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("connecting to ");
    Serial.println(host);
    /* Use WiFiClient class to create TCP connections */

    while (!client.connect(host, port))
    {
        Serial.println("connection failed");
        delay(5000);
    }

    Serial.print("connected to host");
    Serial.println(host);
}

void loop()
{
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
    while (client.connected())
    {
        delay(1500);

        /* This will send the data to the server */
        client.println("hello world");
        if (client.available())
        {

            uint8_t data[30];
            int len = client.read(data, 30);
            if (len < 30)
            {
                data[len] = '\0';
            }
            else
            {
                data[30] = '\0';
            }
            Serial.print("client sent: ");
            Serial.print((char *)data);
        }
    }
    // client.stop();
}