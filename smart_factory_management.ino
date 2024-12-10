#include <Servo.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define rainPin 2
#define flamePin 3
#define gasPin 4
#define trigPin 5
#define echoPin 6
#define buzzerPin 8
#define servoPin 9
#define hc05TxPin 10
#define hc05RxPin 11
#define dhtPin 7
#define relayPin 12

int servoAngle = 0;
int servoDirection = 1;

Servo myServo;
SoftwareSerial bluetoothSerial(hc05RxPin, hc05TxPin);
DHT dht(dhtPin, DHT11);

void setup() {
  pinMode(rainPin, INPUT);
  pinMode(flamePin, INPUT);
  pinMode(gasPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(servoAngle);

  digitalWrite(relayPin, LOW);

  Serial.begin(9600);
  bluetoothSerial.begin(9600);
  dht.begin();
}

void loop() {
  myServo.write(20);
  delay(1000);
  myServo.write(60);
  delay(1000);

  int rainStatus = digitalRead(rainPin);

  if (rainStatus == LOW) {
    delay(200);
    rainStatus = digitalRead(rainPin);
  }

  int flameStatus = digitalRead(flamePin);
  int gasStatus = digitalRead(gasPin);

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2;

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature)  isnan(humidity)) {
    return;
  }

  String rainStatusText = (rainStatus == LOW) ? "Rain Detected" : "No Rain Detected";
  String flameStatusText = (flameStatus == LOW) ? "Flame Detected" : "No Flame Detected";
  String gasStatusText = (gasStatus == LOW) ? "Gas Detected" : "No Gas Detected";
  String distanceStatusText = (distance < 50) ? "Possible threat = " : "no threat = ";

  if (gasStatus == LOW  temperature > 30) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }

  bluetoothSerial.print("Rain: ");
  bluetoothSerial.print(rainStatusText);
  bluetoothSerial.print(" | Flame: ");
  bluetoothSerial.print(flameStatusText);
  bluetoothSerial.print(" | Gas: ");
  bluetoothSerial.print(gasStatusText);
  bluetoothSerial.print(" | Distance: ");
  bluetoothSerial.print(distanceStatusText);
  bluetoothSerial.print(distance);
  bluetoothSerial.print("| Temp: ");
  bluetoothSerial.print(temperature);
  bluetoothSerial.print("°C | Humidity: ");
  bluetoothSerial.println(humidity);

  Serial.print("Rain: ");
  Serial.print(rainStatusText);
  Serial.print(" | Flame: ");
  Serial.print(flameStatusText);
  Serial.print(" | Gas: ");
  Serial.print(gasStatusText);
  Serial.print(" | Distance: ");
  Serial.print(distanceStatusText);
  Serial.print(distance);
  Serial.print(" | Temp: ");
  Serial.print(temperature);
  Serial.print("°C | Humidity: ");
  Serial.println(humidity);

  if (rainStatus == LOW  flameStatus == LOW  gasStatus == LOW || distance < 20) {
    digitalWrite(buzzerPin, HIGH);
    delay(2000);
    digitalWrite(buzzerPin, LOW);
  }

  delay(500);
}