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
/**
 * SSID - (const char *) (in "quotes") ssid of wifi station
 * PASS - (const char *) (in "quotes") password of wifi station
 */
#include "SECRET.h"
/**
 * use "openssl req -x509 -nodes -newkey rsa:2048 -keyout key.pem -out cert.pem -days 4096"
 * to generate key.pem and cert.pem
 * put the contents of cert.pem to x509.h
 * enclosed in "const char server_cert[] PROGMEM = R"EOF(<paste-here>)EOF";"
 * put the contents of key.pem to key.h
 * enclosed in "const char server_private_key[] PROGMEM = R"EOF(<paste-here>)EOF";"
 */
#include "key.h"
#include "x509.h"

#define PORT 443
#define BAUD 74880
#define TIMEOUT 2000 // ms
static const char *HTTP_RES = "HTTP/1.0 200 OK\r\n"
                              "Connection: close\r\n"
                              "Content-Length: 62\r\n"
                              "Content-Type: text/html; charset=iso-8859-1\r\n"
                              "\r\n"
                              "<html>\r\n"
                              "<body>\r\n"
                              "<p>Hello from ESP8266!</p>\r\n"
                              "</body>\r\n"
                              "</html>\r\n";

WiFiServerSecure server(PORT);

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

  PrivateKey *serverPrivKey = new PrivateKey(server_private_key);
  X509List *serverCertList = new X509List(server_cert);
  server.setRSACert(serverCertList, serverPrivKey);
  // server.setECCert(serverCertList, (BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN), serverPrivKey);

  server.begin();
}

void loop()
{
  WiFiClientSecure client = server.available();
  if (!client)
    return;

  String req = client.readStringUntil('\n');
  req.trim();
  Serial.println(req);
  uint32_t timeout = millis() + 1000;
  int lcwn = 0;
  for (;;)
  {
    unsigned char x = 0;
    if ((millis() > timeout) || (client.available() && client.read(&x, 1) < 0))
    {
      client.stop();
      Serial.printf("Connection error, closed\n");
      return;
    }
    else if (!x)
    {
      yield();
      continue;
    }
    else if (x == 0x0D)
    {
      continue;
    }
    else if (x == 0x0A)
    {
      if (lcwn)
      {
        break;
      }
      lcwn = 1;
    }
    else
      lcwn = 0;
  }

  client.write((uint8_t *)HTTP_RES, strlen(HTTP_RES));
  client.flush();
  client.stop();
}