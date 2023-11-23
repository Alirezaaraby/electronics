#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AccelStepper.h>

/* Put your SSID & Password */
const char* ssid = "NodeMCU";       // Enter SSID here
const char* password = "12345678";  // Enter Password here

/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);

bool motor_status = LOW;

#define motorPin1 5
#define motorPin2 4
#define motorPin3 0
#define motorPin4 2

AccelStepper stepper(AccelStepper::HALF4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(100.0);

  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_motoron);
  server.on("/led1off", handle_motoroff);
  server.on("/setspeed", handle_setSpeed);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if (motor_status) {
    stepper.runSpeed();
  }
}

void handle_OnConnect() {
  motor_status = LOW;
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(motor_status));
}

void handle_motoron() {
  motor_status = HIGH;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true));
}

void handle_motoroff() {
  motor_status = LOW;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false));
}

void handle_setSpeed() {
  if (server.hasArg("speed")) {
    int newSpeed = server.arg("speed").toInt();
    stepper.setSpeed(newSpeed);
  }
  server.send(200, "text/html", SendHTML(false));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t motor_stat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Stepper Motor Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "input[type='number'] {width: 80px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP8266 Web Server</h1>\n";
  ptr += "<h3>Using Access Point(AP) Mode</h3>\n";

  ptr += "<form action='/setspeed' method='GET'>";
  ptr += "Stepper Motor Speed: ";
  ptr += "<input type='number' name='speed' value='0' step='1'>";
  ptr += "<input type='submit' value='Set Speed'>";
  ptr += "</form>";

  if (motor_stat) {
    ptr += "<p>Motor Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
  } else {
    ptr += "<p>Motor Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";
  }

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
