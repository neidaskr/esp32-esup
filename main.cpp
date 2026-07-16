#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "My_Electric_SUP";
WebServer server(80);
Servo esc;

const int ESC_PIN = 18; // GPIO Pin connected to ESC signal wire
int currentThrottle = 1000; // 1000us = stopped, 2000us = full throttle

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>input[type=range] { width: 80%; height: 50px; }</style></head><body>";
  html += "<center><h1>e-SUP Throttle</h1>";
  html += "<input type='range' min='1000' max='1500' value='" + String(currentThrottle) + "' oninput='updateThrottle(this.value)'>";
  html += "<p>Throttle Microseconds: <span id='val'>" + String(currentThrottle) + "</span></p></center>";
  html += "<script>function updateThrottle(val) {";
  html += "  document.getElementById(\"val\").innerHTML = val;";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open(\"GET\", \"/set?val=\" + val, true);";
  html += "  xhttp.send();";
  html += "}</script></body></html>";
  server.send(200, "text/html", html);
}

void handleSetThrottle() {
  if (server.hasArg("val")) {
    currentThrottle = server.arg("val").toInt();
    esc.writeMicroseconds(currentThrottle);
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  esc.attach(ESC_PIN, 1000, 2000);
  esc.writeMicroseconds(1000); // Send stop signal to arm the ESC
  
  WiFi.softAP(ssid);
  server.on("/", handleRoot);
  server.on("/set", handleSetThrottle);
  server.begin();
}

void loop() {
  server.handleClient();
}