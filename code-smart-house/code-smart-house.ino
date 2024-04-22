#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* apSSID = "ESP32_AP"; // Tên mạng WiFi của ESP32
const char* apPassword = "12345678"; // Mật khẩu của mạng WiFi

WebServer server(80);
Servo servo;

const int device1Pin = 4; // Chân GPIO điều khiển thiết bị 1
const int device2Pin = 5; // Chân GPIO điều khiển thiết bị 2
const int servoPin = 18; // Chân GPIO điều khiển servo

bool device1State = false;
bool device2State = false;

float temperature = 0.0;
float humidity = 0.0;

const char* htmlContent = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Device Control</title>
    <style>
        * {
            padding: 0;
            margin: 0;
        }
        :root {
            --background-header: #31af65;
            --background-footer: #28cb43;
            --background-body: #081b29;
            --color-text:  #00d8f0;
            --color-border:rgb(158, 158, 158);
            --color-point: rgb(253, 253, 253);
            --color-toggle-on: rgb(38, 0, 255);
            --color-toggle-off: rgb(53, 45, 45);
            --color-background-second:  #112e42;


        }
        body {
            height: 100vh;
            position: relative;
            background: var(--background-body);
            display: flex;
            flex-direction: column;
            color: var(--color-text);
            align-items: center;
        }

        .header {
            width: 100%;
            height: 40px;
            display: flex;
            justify-content: center;
            align-items: center;
            background: var(--color-background-second);
        }

        .container {
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
            margin-top: 10px;
            margin-bottom: 40px;
        }

        .container-title {
            margin: 20px;
        }

        h2 {
            position: relative;
            color: var(--background-body);
            font-size: 27px;
            -webkit-text-stroke: 0.3vw #21323f;
            text-transform: uppercase;
        }

        h2::before {
            content: attr(data-text);
            position: absolute;
            top:  0;
            left: 0;
            width: 0;
            height: 100%;
            color: var(--color-text);
            -webkit-text-stroke: 0vw #21323f;
            border-right: 2px solid --color-text;
            overflow: hidden;
            animation: animate 6s linear infinite;
        }
        @keyframes animate {
           0%,10%,100%
           {
            width: 0;
           }
           70%,90%
           {
            width: 100%;
           }
        }
        .container-toggle {
            padding: 10px;
            height: 50vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: column;
            border: 3px;
            border-style: solid;
            border-color: var(--color-border);
        }
        .toggle-row1 {
            height: 10vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        .toggle-row2 {
            height: 10vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        .toggle-row3 {
            height: 10vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        .toggle-row4 {
            height: 10vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .toggle-row4 {
            height: 10vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .toggle-row5 {
            height: 10vh;
            background-color: var(--background-body);
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .deviceButton {
            background-color:var(--color-toggle-off);
            width: 50px;
            height: 25px;
            border-radius: 10px;
            cursor: pointer;
            position: relative;
            margin: 0 10px;
            transition: 0.5s;
            border: 2px;
            border-style: solid;
            border-color: var(--color-border);
        }

        .deviceButton::before {
            position: absolute;
            content: '';
            background-color: var(--color-point);
            width: 17px;
            height: 17px;
            border-radius: 50%;
            margin: 2.5px;
            transition: 0.5s;
            border: 2px;
            border-style: solid;
            border-color: var(--color-border);
        }

        input:checked + .deviceButton {
            background-color: var(--color-toggle-on);
        }

        input:checked + .deviceButton::before {
            transform: translateX(25px);
        }

        input {
            display: none;
        }

        .parameter {
            border: 2px;
            border-style: solid;
            border-color: var(--color-border);
            display: flex;
            align-items: center;
            justify-content: space-between;
            width: 405px;
            height:50px;
        }

        .parameter-column1 {
            margin-left: 50px;
        }

        .parameter-column2 {
            margin-right: 50px;
        }

        #sensorData {
            margin-top: 20px;
            font-size: 20px;
        }

        .footer {
            height: 40px;
            width: 100%;
            font-size: 15px;
            display: flex;
            align-items: center;
            justify-content: center;
            background: var(--color-background-second);
        }
    </style>
</head>
<body>
    <header class="header">
        <h3>Thắng</h3>
    </header>
    <div class="container">
        <div class="container-title">
            <h2 data-text="Controller">Controller</h2>
        </div>
        <div class="container-toggle">
            <div class="toggle-row1 row">
                <h3>Led 1:</h3>
                <input type="checkbox" id="device1Checkbox">
                <label for="device1Checkbox" class="deviceButton"></label>
                <h3>Fan 1:</h3>
                <input type="checkbox" id="device2Checkbox">
                <label for="device2Checkbox" class="deviceButton"></label>
                <h3>Door 1:</h3>
                <input type="checkbox" id="device3Checkbox">
                <label for="device3Checkbox" class="deviceButton"></label>
            </div>
            <div class="toggle-row2 row">
                <h3>Led 2:</h3>
                <input type="checkbox" id="device4Checkbox">
                <label for="device4Checkbox" class="deviceButton"></label>
                <h3>Fan 2:</h3>
                <input type="checkbox" id="device5Checkbox">
                <label for="device5Checkbox" class="deviceButton"></label>
                <h3>Door 2:</h3>
                <input type="checkbox" id="device6Checkbox">
                <label for="device6Checkbox" class="deviceButton"></label>
            </div>
            <div class="toggle-row3 row">
                <h3>Led 3:</h3>
                <input type="checkbox" id="device7Checkbox">
                <label for="device7Checkbox" class="deviceButton"></label>
                <h3>Fan 3:</h3>
                <input type="checkbox" id="device8Checkbox">
                <label for="device8Checkbox" class="deviceButton"></label>
                <h3>Door 3:</h3>
                <input type="checkbox" id="device9Checkbox">
                <label for="device9Checkbox" class="deviceButton"></label>
            </div>
            <div class="toggle-row4 row">
                <h3>Led 4:</h3>
                <input type="checkbox" id="device10Checkbox">
                <label for="device10Checkbox" class="deviceButton"></label>
                <h3>Fan 4:</h3>
                <input type="checkbox" id="device11Checkbox">
                <label for="device11Checkbox" class="deviceButton"></label>
                <h3>Door 4:</h3>
                <input type="checkbox" id="device12Checkbox">
                <label for="device12Checkbox" class="deviceButton"></label>
            </div>
            <div class="toggle-row5 row">
                <h3>Led 5:</h3>
                <input type="checkbox" id="device13Checkbox">
                <label for="device13Checkbox" class="deviceButton"></label>
                <h3>Fan 5:</h3>
                <input type="checkbox" id="device14Checkbox">
                <label for="device14Checkbox" class="deviceButton"></label>
                <h3>Door 5:</h3>
                <input type="checkbox" id="device15Checkbox">
                <label for="device15Checkbox" class="deviceButton"></label>
            </div>
        </div>
        <div class="parameter" id="sensorData">
            <div class="parameter-column1">
                Temperature: <span id="temperature"></span>°C<br>
            </div>
            <div class="parameter-column2" >
                Humidity: <span id="humidity"></span>%
            </div>
        </div>
    </div>
    <!-- Footer -->
    <footer class="footer">
        <h3>Copyright © 2024 by Thang | All Rights Reserved.</h3>
    </footer>

    <script>
        function toggleDevice(deviceNumber) {
            var xhr = new XMLHttpRequest();
            var checkboxId = "device" + deviceNumber + "Checkbox";
            xhr.open("GET", "/device?device=" + deviceNumber + "&state=" + (document.getElementById(checkboxId).checked ? "on" : "off"), true);
            xhr.send();
        }
        document.getElementById("device1Checkbox").addEventListener("change", function() { toggleDevice(1); });
        document.getElementById("device2Checkbox").addEventListener("change", function() { toggleDevice(2); });
        document.getElementById("device3Checkbox").addEventListener("change", function() { toggleDevice(3); });
        document.getElementById("device4Checkbox").addEventListener("change", function() { toggleDevice(4); });
        document.getElementById("device5Checkbox").addEventListener("change", function() { toggleDevice(5); });
        document.getElementById("device6Checkbox").addEventListener("change", function() { toggleDevice(6); });
        document.getElementById("device7Checkbox").addEventListener("change", function() { toggleDevice(7); });
        document.getElementById("device8Checkbox").addEventListener("change", function() { toggleDevice(8); });
        document.getElementById("device9Checkbox").addEventListener("change", function() { toggleDevice(9); });
        document.getElementById("device10Checkbox").addEventListener("change", function() { toggleDevice(10); });
        document.getElementById("device11Checkbox").addEventListener("change", function() { toggleDevice(11); });
        document.getElementById("device12Checkbox").addEventListener("change", function() { toggleDevice(12); });
        document.getElementById("device13Checkbox").addEventListener("change", function() { toggleDevice(13); });
        document.getElementById("device14Checkbox").addEventListener("change", function() { toggleDevice(14); });
        document.getElementById("device15Checkbox").addEventListener("change", function() { toggleDevice(15); });
        
        function updateSensorData() {
            var xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function() {
                if (xhr.readyState == XMLHttpRequest.DONE) {
                    if (xhr.status == 200) {
                        var data = JSON.parse(xhr.responseText);
                        document.getElementById("temperature").textContent = data.temperature;
                        document.getElementById("humidity").textContent = data.humidity;
                    }
                }
            };
            xhr.open("GET", "/sensordata", true);
            xhr.send();
        }
        setInterval(updateSensorData, 2000);
    </script>
</body>
</html>
)=====";

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void handleDevice() {
  int deviceNumber = server.arg("device").toInt();
  bool newState = server.arg("state") == "on";

  if (deviceNumber == 1) {
    digitalWrite(device1Pin, newState ? HIGH : LOW);
    device1State = newState;
  } else if (deviceNumber == 2) {
    digitalWrite(device2Pin, newState ? HIGH : LOW);
    device2State = newState;
  } else if (deviceNumber == 3) {
    if (newState) {
      servo.write(90); // Xoay servo góc 90 độ
    } else {
      servo.write(0); // Xoay servo về góc 0 độ
    }
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSensorData() {
  String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  server.send(200, "application/json", data);
}

void setup() {
  Serial.begin(115200);
  pinMode(device1Pin, OUTPUT); // Đặt chân GPIO điều khiển thiết bị 1 làm OUTPUT
  pinMode(device2Pin, OUTPUT); // Đặt chân GPIO điều khiển thiết bị 2 làm OUTPUT
  servo.attach(servoPin); // Gắn đối tượng servo vào chân GPIO điều khiển servo
  
  // Khởi động chế độ Access Point với tên và mật khẩu được chỉ định
  WiFi.softAP(apSSID, apPassword);

  Serial.println("Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/device", handleDevice);
  server.on("/sensordata", handleSensorData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Update sensor data every 2 seconds
  static unsigned long lastUpdateTime = 0;
  if (millis() - lastUpdateTime >= 2000) {
    temperature = random(20, 30); // Random temperature between 20 to 30 Celsius
    humidity = random(40, 60); // Random humidity between 40 to 60%
    lastUpdateTime = millis();
  }
}

