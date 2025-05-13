#include <Arduino.h>
#include <ESPmDNS.h>
#include <ConfigPortal32.h>

char*               ssid_pfix = (char*)"ha";
String              user_config_html = "";      

WebServer           server(80);
#define             RELAY 15

void turnOff() {
    String message = "TurnOff the Realy";
    digitalWrite(RELAY, LOW);
    server.send(200, "text/plain", message);
    Serial.println("off");
}

void turnOn() {
    String message = "TurnOn the Realy";
    digitalWrite(RELAY, HIGH);
    server.send(200, "text/plain", message);
    Serial.println("on");
}

void toggle() {
    if(digitalRead(RELAY)) {
        turnOff();
    } else {
        turnOn();
    }
}

void setup() {
    Serial.begin(115200);

    loadConfig();
    // *** If no "config" is found or "config" is not "done", run configDevice ***
    if(!cfg.containsKey("config") || strcmp((const char*)cfg["config"], "done")) {
        configDevice();
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // main setup
    Serial.printf("\nIP address : "); Serial.println(WiFi.localIP());
    pinMode(RELAY, OUTPUT);

    server.on("/turnOn", turnOn);
    server.on("/turnOff", turnOff);
    server.on("/toggle", toggle);
    server.onNotFound([]() {
        server.send(200, "text/html", "<h1>Not Found</h1>");
    }); 

    if (MDNS.begin("wonid")) {
        Serial.println("MDNS responder started with name: ha");
    }
    server.begin();
}

void loop() {
    server.handleClient();
}