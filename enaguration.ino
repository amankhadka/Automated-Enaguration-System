// Simple WiFi-controlled car on NodeMCU V2 and L9110S motor drive board
// by Alan Wang and edited by Er. Sitaram Khadka to accomodate single motor control for enagurating curtain with web browser.

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi settings
#define WIFI_MODE           1                     // 1 = AP mode, 2 = STA mode
#define SSID_AP             "Enaguration WiFi"    // for AP mode
#define PASSWORD_AP         "P@ssw0rd"            // for AP mode
#define SSID_STA            "Enaguration WiFi"      // for STA mode
#define PASSWORD_STA        "P@ssw0rd"  // for STA mode

// motor settings
#define RIGHT_MOTOR_PIN1    4                     // pin 1 of right motor (D2)
#define RIGHT_MOTOR_PIN2    5                     // pin 2 of right motor (D1)
#define RIGHT_MOTOR_SPEED   1023                  //speed for right motor (0-1023)

IPAddress local_ip(192, 168, 4, 1); //IP for AP mode
IPAddress gateway(192, 168, 4, 1); //IP for AP mode
IPAddress subnet(255, 255, 255, 0); //IP for AP mode
ESP8266WebServer server(80);
int mode = 0; // set car drive mode (0 = stop)

// initialize
void setup() {
  Serial.begin(9600);
  Serial.println("NodeMCU Wifi Car");
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  control(); // stop the Motor

  if (WIFI_MODE == 1) { // AP mode
    WiFi.softAP(SSID_AP, PASSWORD_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
  } else { // STA mode
    WiFi.begin(SSID_STA, PASSWORD_STA);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP()); //the IP is needed for connection in STA mode
  }

  // setup web server to handle specific HTTP requests
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/forward", HTTP_GET, handle_forward);
  server.on("/backward", HTTP_GET, handle_backward);
  server.on("/stop", HTTP_GET, handle_stop);
  server.onNotFound(handle_NotFound);

  //start server
  server.begin();
  Serial.println("NodeMCU web server started.");
}

// handle HTTP requests and control car
void loop() { 
  server.handleClient();
  control();
}

// HTTP request: on connect
void handle_OnConnect() {
  mode = 0;
  Serial.println("Client connected");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: stop car
void handle_stop() {
  mode = 0;
  Serial.println("Stopped");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: go forward
void handle_forward() {
  mode = 1;
  Serial.println("Go forward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: go backward
void handle_backward() {
  mode = 2;
  Serial.println("Go backward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: other
void handle_NotFound() {
  mode = 0;
  Serial.println("Page error");
  server.send(404, "text/plain", "Not found");
}

// control car movement
void control() {
  switch (mode) {
    case 0: // stop car
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      break;
    case 1: // go forward
      analogWrite(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_SPEED);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      break;
    case 2: // go backward
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      analogWrite(RIGHT_MOTOR_PIN2, RIGHT_MOTOR_SPEED);
      break;

  }
}

// output HTML web page for user
String SendHTML() {
  String html = "<!DOCTYPE html>\n";
  html += "<html>\n";
  html += "<head>\n";
  html += "<title>Enaguration</title>\n";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<div align=\"center\" style=\"background-color: #a6a1a1\">\n";
  html += "<h1>Enaguration</h1>\n";
  html += "<br>\n";
  html += "<form method=\"GET\">\n";
  html += "<input type=\"button\" style=\"background-color: #e81d07;margin-left:auto;margin-right:auto;display:block;margin-top:22%;margin-bottom:0%;border-top: 2px solid #5f6c4d;border-right: 2px solid #000000;border-bottom: 2px solid #000000;border-left: 2px solid #5f6c4d;width: 180px; height: 100px;font-weight: bold; font-size: 20px;\" value=\"Enagurate \n(open)\" onclick=\"window.location.href='/forward'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Close\" onclick=\"window.location.href='/backward'\">\n";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Stop\" onclick=\"window.location.href='/stop'\">\n";
  html += "<br><br>\n";
  html += "</form>\n";
  html += "</div>\n";
  html += "<p style=\"font-family:verdana;font-color: #6b1010\">Automated Enaguration System</p>";
  html += "<p style=\"font-size:16px;font-family:'Courier New';font-color: #964702\">This is an automatated ENAGURATION project. The project is considered to be used in enaguration events like New construction, health-camp and other events.</p>";  
  html += "<p align=\"center\" style=\"font-family:verdana;font-color: #f55225\">Designed and developed by Sitaram Khadka</p>";
  html += "</body>\n";
  html += "</html>\n";
  return html;
}
