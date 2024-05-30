#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 10     // Pinul digital conectat la senzorul DHT11
#define DHTTYPE DHT11 // Definirea tipului de senzor DHT11
#define LM35_PIN A1   // Pinul analogic la care este conectat senzorul LM35

DHT dht(DHTPIN, DHTTYPE);

const int rxPin = 2;
const int txPin = 3;

SoftwareSerial myBluetooth(rxPin, txPin);

void setup() {
  Serial.begin(9600);
  myBluetooth.begin(9600);
  Serial.println("Bluetooth este pregătit pentru comunicare");

  dht.begin(); // Inițializează senzorul DHT11
}

void loop() {
  // Citirea umidității de la DHT11
  float humidity = dht.readHumidity();

  // Verifică dacă citirea a reușit și dacă nu este NaN
  if (isnan(humidity)) {
    Serial.println("Eroare la citirea de la DHT11");
  } else {
    Serial.print("Umiditate: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Citirea temperaturii de la LM35
  int lm35Val = analogRead(LM35_PIN);
  float voltage = lm35Val * (5.0 / 1023.0);
  float temperatureLM35 = voltage * 100;  // Conversie tensiune în temperatură

  Serial.print("Temperatură LM35: ");
  Serial.print(temperatureLM35);
  Serial.println(" *C");

  if (myBluetooth.available()) {
    char received = myBluetooth.read();
    Serial.print("Recepționat: ");
    Serial.println(received);
  }

  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 2000) {
    StaticJsonDocument<200> jsonDoc;
    if (!isnan(humidity)) {
      jsonDoc["DHT11"] = humidity;
    } else {
      jsonDoc["DHT11"] = "Error";
    }
    jsonDoc["LM35"] = temperatureLM35;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    myBluetooth.println(jsonString);

    Serial.print("Trimis JSON: ");
    Serial.println(jsonString);

    lastSendTime = millis();
  }
  delay(2000);
}
