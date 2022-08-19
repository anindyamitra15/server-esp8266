/**
 * @file main.cpp
 * @author anindyamitra15
 * @brief plain http server (not secure)
 * @date 2022-08-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
/**
 * SSID - (const char *) (in "quotes") ssid of wifi station
 * PASS - (const char *) (in "quotes") password of wifi station
 */
#include "SECRET.h"

#define PORT 80
#define BAUD 74880
#define TIMEOUT 2000 // ms

WiFiServer server(PORT);

void setup()
{
  Serial.begin(BAUD);
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(". ");
  }
  Serial.print("\nConnected with ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();
  if (!client)
    return;

  String req = client.readStringUntil('\n');
  req.trim();
  Serial.println(req);

  client.print("<html><body><h1>");
  client.print(req);   // this is only for the sending client
  client.println("</h1></body></html>");
  server.flush();
}