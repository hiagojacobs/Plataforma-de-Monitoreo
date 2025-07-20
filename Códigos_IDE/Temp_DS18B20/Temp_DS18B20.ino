#include <OneWire.h>                  // Librería comunicación OneWire 
#include <DallasTemperature.h>        // Librería para sensor DS18B20

#define ONE_WIRE_BUS 5                // Define el pin GPIO 5 como de datos

OneWire oneWire(ONE_WIRE_BUS);        // Crea un objeto OneWire conectado al pin definido
DallasTemperature sensors(&oneWire);  // Crea un objeto "sensors" para manejar sensores Dallas en ese bus

void setup() {
  Serial.begin(115200);               // Comunicación serial a 115200 baudios 
  sensors.begin();                    // Comunicación serial con el sensor 
}

void loop() {
  sensors.requestTemperatures();      // Solicita temperatura al sensor 
  float tempC = sensors.getTempCByIndex(0);  // Obtiene la temperatura en grados Celsius 
  
  Serial.print("Temperatura: ");      // Printa Texto
  Serial.print(tempC);                // Printa Temperatura 
  Serial.println(" °C");              // Printa Texto
  
  delay(2000);                        // Espera 2 sgundos
}
