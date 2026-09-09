#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "Wemos-Motor";
const char* password = "12345678";

ESP8266WebServer server(80);


// =====================================================
// TB6612FNG PIN DEFINITIONS
// =====================================================

// Motor A
const uint8_t AIN1 = D1;   // GPIO5
const uint8_t AIN2 = D2;   // GPIO4
const uint8_t PWMA = D5;   // GPIO14

// Motor B
const uint8_t BIN1 = D6;   // GPIO12
const uint8_t BIN2 = D7;   // GPIO13
const uint8_t PWMB = D8;   // GPIO15


// =====================================================
// MOTOR VARIABLES
// =====================================================

int speedA = 700;
int speedB = 700;

bool motorsRunning = false;
bool reverseMode = false;


// =====================================================
// MOTOR A
// =====================================================

void motorAForward()
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMA, speedA);
}


void motorAReverse()
{
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  analogWrite(PWMA, speedA);
}


void motorAStop()
{
  analogWrite(PWMA, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
}


// =====================================================
// MOTOR B
// =====================================================

void motorBForward()
{
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMB, speedB);
}


void motorBReverse()
{
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  analogWrite(PWMB, speedB);
}


void motorBStop()
{
  analogWrite(PWMB, 0);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}


// =====================================================
// BOTH MOTORS
// =====================================================

void moveForward()
{
  motorAForward();
  motorBForward();

  motorsRunning = true;
  reverseMode = false;
}


void moveReverse()
{
  motorAReverse();
  motorBReverse();

  motorsRunning = true;
  reverseMode = true;
}


void stopMotors()
{
  motorAStop();
  motorBStop();

  motorsRunning = false;
}


// =====================================================
// UPDATE MOTOR SPEED
// =====================================================

void updateMotorSpeed()
{
  if (!motorsRunning)
    return;

  if (reverseMode)
  {
    motorAReverse();
    motorBReverse();
  }
  else
  {
    motorAForward();
    motorBForward();
  }
}


// =====================================================
// WEBPAGE
// =====================================================

void handleRoot()
{
  String html = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport"
content="width=device-width, initial-scale=1">

<title>Wemos Motor Control</title>

<style>

body
{
  margin: 0;
  padding: 20px;

  background: #111;
  color: white;

  font-family: Arial, sans-serif;

  text-align: center;
}


h1
{
  font-size: 30px;
}


.status
{
  margin: 20px;

  font-size: 22px;
}


.statusBox
{
  display: inline-block;

  padding: 10px 25px;

  border-radius: 10px;

  background: #333;
}


.control
{
  max-width: 500px;

  margin: auto;
}


button
{
  width: 90%;

  max-width: 350px;

  height: 70px;

  margin: 10px;

  border: none;

  border-radius: 15px;

  font-size: 22px;

  font-weight: bold;

  color: white;
}


.forward
{
  background: #168a35;
}


.reverse
{
  background: #1976d2;
}


.stop
{
  background: #c62828;
}


.sliderBox
{
  background: #222;

  padding: 20px;

  margin: 20px 0;

  border-radius: 15px;
}


.slider
{
  width: 90%;
}


.speedValue
{
  font-size: 20px;

  margin-top: 10px;
}

</style>

</head>


<body>


<h1>🚗 Wemos Motor Control</h1>


<div class="status">

Status:

<span id="status"
class="statusBox">

STOPPED

</span>

</div>


<div class="control">


<button
class="forward"
onclick="command('/forward')">

▶ START / FORWARD

</button>


<button
class="reverse"
onclick="command('/reverse')">

◀ REVERSE

</button>


<button
class="stop"
onclick="command('/stop')">

■ STOP

</button>


<div class="sliderBox">

<h2>Motor A</h2>

<input
type="range"
min="0"
max="1023"
value="700"
class="slider"
id="speedA"
oninput="speedChanged()">

<div class="speedValue">

Speed:
<span id="speedAText">700</span>

</div>

</div>


<div class="sliderBox">

<h2>Motor B</h2>

<input
type="range"
min="0"
max="1023"
value="700"
class="slider"
id="speedB"
oninput="speedChanged()">

<div class="speedValue">

Speed:
<span id="speedBText">700</span>

</div>

</div>


</div>


<script>


function command(url)
{
  fetch(url)

  .then(response => response.text())

  .then(data =>
  {
    document.getElementById("status").innerHTML = data;
  });
}


function speedChanged()
{
  let a =
    document.getElementById("speedA").value;

  let b =
    document.getElementById("speedB").value;


  document.getElementById("speedAText")
    .innerHTML = a;

  document.getElementById("speedBText")
    .innerHTML = b;


  fetch("/speed?a=" + a + "&b=" + b);
}


</script>


</body>

</html>

)rawliteral";


  server.send(200, "text/html", html);
}


// =====================================================
// FORWARD
// =====================================================

void handleForward()
{
  moveForward();

  server.send(
    200,
    "text/plain",
    "FORWARD"
  );
}


// =====================================================
// REVERSE
// =====================================================

void handleReverse()
{
  moveReverse();

  server.send(
    200,
    "text/plain",
    "REVERSE"
  );
}


// =====================================================
// STOP
// =====================================================

void handleStop()
{
  stopMotors();

  server.send(
    200,
    "text/plain",
    "STOPPED"
  );
}


// =====================================================
// SPEED
// =====================================================

void handleSpeed()
{
  if (server.hasArg("a"))
  {
    speedA =
      server.arg("a").toInt();
  }


  if (server.hasArg("b"))
  {
    speedB =
      server.arg("b").toInt();
  }


  speedA =
    constrain(speedA, 0, 1023);

  speedB =
    constrain(speedB, 0, 1023);


  updateMotorSpeed();


  server.send(
    200,
    "text/plain",
    "OK"
  );
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);


  // ---------------------------------------------------
  // Configure pins
  // ---------------------------------------------------

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);


  // ---------------------------------------------------
  // IMPORTANT:
  // Stop motors immediately
  // ---------------------------------------------------

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);


  // ---------------------------------------------------
  // ESP8266 PWM
  // ---------------------------------------------------

  analogWriteRange(1023);

  analogWriteFreq(1000);


  // ---------------------------------------------------
  // Create Wi-Fi Access Point
  // ---------------------------------------------------

  WiFi.mode(WIFI_AP);

  WiFi.softAP(
    ssid,
    password
  );


  // ---------------------------------------------------
  // Serial information
  // ---------------------------------------------------

  Serial.println();
  Serial.println("==============================");
  Serial.println(" WEMOS MOTOR CONTROLLER");
  Serial.println("==============================");

  Serial.print("WiFi: ");

  Serial.println(ssid);

  Serial.print("IP: ");

  Serial.println(
    WiFi.softAPIP()
  );


  // ---------------------------------------------------
  // WEB SERVER
  // ---------------------------------------------------

  server.on(
    "/",
    handleRoot
  );


  server.on(
    "/forward",
    handleForward
  );


  server.on(
    "/reverse",
    handleReverse
  );


  server.on(
    "/stop",
    handleStop
  );


  server.on(
    "/speed",
    handleSpeed
  );


  server.begin();


  Serial.println(
    "Web server started!"
  );

  Serial.println(
    "Connect to Wemos-Motor"
  );

  Serial.println(
    "Open 192.168.4.1"
  );
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  server.handleClient();
}
