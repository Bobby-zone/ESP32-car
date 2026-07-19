#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

const char *ssid = "ESP32-Car";
const char *password = "12345678";

const int ain1 = 23;
const int ain2 = 22;
const int pwma = 21;

const int bin1 = 19;
const int bin2 = 18;
const int pwmb = 17;

const int pota = 32;
const int potb = 33;

int motorSpeed = 50;

const int pwmaCh = 0;
const int pwmbCh = 1;
const int pwmFreq = 25000;
const int pwmResolution = 8;

WebServer server(80);

void setMotor(int left, int right) {
  // left motor
  if (left > 0) {
    digitalWrite(ain1, HIGH);
    digitalWrite(ain2, LOW);
    ledcWrite(pwmaCh, left);
  } else if (left < 0) {
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, HIGH);
    ledcWrite(pwmaCh, -left);
  } else {
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, LOW);
    ledcWrite(pwmaCh, 0);
  }

  // right motor
  if (right > 0) {
    digitalWrite(bin1, HIGH);
    digitalWrite(bin2, LOW);
    ledcWrite(pwmbCh, right);
  } else if (right < 0) {
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, HIGH);
    ledcWrite(pwmbCh, -right);
  } else {
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, LOW);
    ledcWrite(pwmbCh, 0);
  }
}

void stop() { setMotor(0, 0); }

void forward() { setMotor(motorSpeed, motorSpeed); }

void left() { setMotor(motorSpeed * 0.1, motorSpeed); }

void right() { setMotor(motorSpeed, motorSpeed * 0.1); }

void reverse() { setMotor(-motorSpeed, -motorSpeed); }

// Web page controller html code
const char webpage[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Car</title>
<style>
body{
    font-family:Arial;
    text-align:center;
    margin-top:60px;
    user-select:none;
}
h1{
    font-size:40px;
}
p{
    font-size:22px;
}
.controller {
    display: grid;
    grid-template-columns: repeat(3, 100px);
    grid-template-rows: repeat(2, 80px);
    gap: 10px;
    justify-content: center;
    margin-top: 30px;
}
.controller button {
    width: 100px;
    height: 70px;
    font-size: 30px;
    border-radius: 15px;
}
.controller button:nth-child(1) {
    grid-column: 2;
    grid-row: 1;
}

.controller button:nth-child(2) {
    grid-column: 1;
    grid-row: 2;
}

.controller button:nth-child(3) {
    grid-column: 3;
    grid-row: 2;
}

.controller button:nth-child(4) {
    grid-column: 2;
    grid-row: 2;
}

</style>
</head>
<body>

<h1>ESP32 Car</h1>
<p>Speed: <span id="speedValue">50</span></p>
<input type="range" min="0" max="255" value="50" id="speed">

<div class="controller">
    <button onmousedown="send('forward')" onmouseup="send('stop')" ontouchstart="send('forward')" ontouchend="send('stop')">⬆</button>
    <button onmousedown="send('left')" onmouseup="send('stop')" ontouchstart="send('left')" ontouchend="send('stop')">⬅︎</button>
    <button onmousedown="send('right')" onmouseup="send('stop')" ontouchstart="send('right')" ontouchend="send('stop')">➡︎</button>
    <button onmousedown="send('reverse')" onmouseup="send('stop')" ontouchstart="send('reverse')" ontouchend="send('stop')">⬇︎</button>
</div>

<script>
// adjust the motor speed according to slider input
const slider = document.getElementById("speed");
const speedValue = document.getElementById("speedValue");

function send(cmd){
    fetch("/" + cmd);
}

// sent speed 
slider.oninput = function() {
    speedValue.innerHTML = this.value;
    send("speed?value=" + this.value);
}

// listen to keydown wasd key
document.addEventListener("keydown", function(e){
    if(e.repeat) return;

    if(e.key=="w") send("forward");
    if(e.key=="s") send("reverse");
    if(e.key=="a") send("left");
    if(e.key=="d") send("right");
});

// listen to keyup then stop the motor
document.addEventListener("keyup", function(e){
    send("stop");
});

</script>

</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(pwma, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(pwmb, OUTPUT);

  pinMode(pota, INPUT);
  pinMode(potb, INPUT);

  ledcSetup(pwmaCh, pwmFreq, pwmResolution);
  ledcSetup(pwmbCh, pwmFreq, pwmResolution);
  ledcAttachPin(pwma, pwmaCh);
  ledcAttachPin(pwmb, pwmbCh);

  // start wifi
  WiFi.softAP(ssid, password);

  Serial.println("WiFi started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() { server.send(200, "text/html", webpage); });

  server.on("/forward", []() {
    forward();
    server.send(200, "text/plain", "forward");
  });
  server.on("/reverse", []() {
    reverse();
    server.send(200, "text/plain", "reverse");
  });
  server.on("/left", []() {
    left();
    server.send(200, "text/plain", "left");
  });
  server.on("/right", []() {
    right();
    server.send(200, "text/plain", "right");
  });
  server.on("/stop", []() {
    stop();
    server.send(200, "text/plain", "stop");
  });
  server.on("/speed", []() {
    if (server.hasArg("value")) {
      motorSpeed = server.arg("value").toInt();
      motorSpeed = constrain(motorSpeed, 0, 255);

      Serial.print("Speed = ");
      Serial.println(motorSpeed);
    }
    server.send(200, "text/plain", "ok speed");
  });

  server.begin();
}

void loop() { server.handleClient(); }