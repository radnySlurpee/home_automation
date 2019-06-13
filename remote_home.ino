#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "VENDOFY";
const char* password = "helloworld";

IPAddress ip(192, 168, 137, 100); //set static ip
IPAddress gateway(192, 168, 137, 1); //set getteway
IPAddress subnet(255, 255, 255, 0);//set subnet

ESP8266WebServer server(80);

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 14;

IRrecv irrecv(kRecvPin);

decode_results results;

bool relay1_isOpen = true;
bool relay2_isOpen = true;
bool relay3_isOpen = true;
bool relay4_isOpen = true;

void setup() {
  Serial.begin(115200);

  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  // Serial.println();
  // Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  // Serial.println(kRecvPin);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);


  // WiFi Config

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", HTTP_POST, handlePost);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  IR_remote();
}

void handlePost() {

  if (server.hasArg("status") && server.hasArg("relay")) {

    if (server.arg("relay") == "1") {
      relay1_isOpen = (server.arg("status") == "on") ? false : true;
      digitalWrite(D1, (server.arg("status") == "on") ? LOW : HIGH);
    } else if (server.arg("relay") == "2") {
      relay2_isOpen = (server.arg("status") == "on") ? false : true;
      digitalWrite(D2, (server.arg("status") == "on") ? LOW : HIGH);
    } else if (server.arg("relay") == "3") {
      relay3_isOpen = (server.arg("status") == "on") ? false : true;
      digitalWrite(D3, (server.arg("status") == "on") ? LOW : HIGH);
    } else if (server.arg("relay") == "4") {
      relay4_isOpen = (server.arg("status") == "on") ? false : true;
      digitalWrite(D4, (server.arg("status") == "on") ? LOW : HIGH);
    }
    server.send(200, "text/plain", "success");
  } else {
    server.send(400, "text/plain", "400: Invalid Request");
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void IR_remote () {
  if (irrecv.decode(&results)) {
    
    if (results.value == 16191727) {

      if(relay1_isOpen == true) {
        digitalWrite(D1, LOW);
        relay1_isOpen = false;
      } else {
        digitalWrite(D1, HIGH);
        relay1_isOpen = true;
      }

    } else if (results.value == 16199887) {

      if(relay2_isOpen == true) {
        digitalWrite(D2, LOW);
        relay2_isOpen = false;
      } else {
        digitalWrite(D2, HIGH);
        relay2_isOpen = true;
      }

    } else if (results.value == 16189687) {

      if(relay3_isOpen == true) {
        digitalWrite(D3, LOW);
        relay3_isOpen = false;
      } else {
        digitalWrite(D3, HIGH);
        relay3_isOpen = true;
      }

    } else if (results.value == 16197847) {

      if(relay4_isOpen == true) {
        digitalWrite(D4, LOW);
        relay4_isOpen = false;
      } else {
        digitalWrite(D4, HIGH);
        relay4_isOpen = true;
      }
    }
    
    //  print() & println() can't handle printing long longs. (uint64_t)
    //  serialPrintUint64(results.value); Serial.println("");
    irrecv.resume();  // Receive the next value
  }

  // delay(100);
}
 