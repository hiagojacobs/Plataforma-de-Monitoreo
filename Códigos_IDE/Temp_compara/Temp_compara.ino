#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- DHT11 ---
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- DS18B20 ---
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  dht.begin();
  sensors.begin();
  delay(2000);  // Tiempo inicial para estabilizar sensores

  // Encabezado para el Plotter o MATLAB
  Serial.println("TempDHT11\tTempDS18B20");
}

void loop() {
  // Leer sensor DHT11
  float tempDHT = dht.readTemperature();

  // Leer sensor DS18B20
  sensors.requestTemperatures();
  float tempDS = sensors.getTempCByIndex(0);

  // Validar y mostrar
  if (isnan(tempDHT)) {
    Serial.print("NaN\t");
  } else {
    Serial.print(tempDHT, 2);
    Serial.print("\t");
  }

  if (tempDS == DEVICE_DISCONNECTED_C) {
    Serial.println("NaN");
  } else {
    Serial.println(tempDS, 2);
  }

  delay(2000);  // Esperar 2 segundos entre lecturas
}
