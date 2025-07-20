#include <Adafruit_Sensor.h>         // Librería para sensores Adafruit 
#include <DHT.h>                     // Librería específica DHT11

#define DHTPIN 4                     // GPIO4 Pin digital del DHT11
#define DHTTYPE DHT11                // Modelo del Sensor

DHT dht(DHTPIN, DHTTYPE);            // Objeto `dht` con pin y tipo

void setup() {
    Serial.begin(115200);           // Inicia la comunicación serial a 115200 baudios
    dht.begin();                    // Innicia el sensor DHT
}

void loop() {
    delay(2000);

    float humedad = dht.readHumidity();           // Lectura de humedad relativa en %
    float temperatura = dht.readTemperature();    // Lectura de temperatura en grados Celsius

    if (isnan(humedad) || isnan(temperatura)) {   // Verifica si la lectura falló
        Serial.println("Error al leer el sensor DHT11!");
        return;                                   // Sale del loop() si hubo error
    }

    Serial.print("Humedad: ");                    // Printa Texto
    Serial.print(humedad);                        // Printa humedad 
    Serial.print("%  |  Temperatura: ");          // Printa texto 
    Serial.print(temperatura);                    // Printa temperatura
    Serial.println("°C");                         // Printa Texto
}
