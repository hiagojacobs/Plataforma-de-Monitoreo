const int sensorPin = 1;                // Pin de Lectura del Sensor
float offsetVoltaje = 0;                // VAR Offset
unsigned long tiempoAnterior = 0;       // VAR de almacenamiento
const int intervaloLectura = 1000;      // tiempo en ms (1s)

// ----- Lectura Inicial ------

void setup() {
  Serial.begin(115200);

  // Calibración inicial (motor apagado)
  delay(3000);

  long suma = 0;
  const int muestras = 100;

  for (int i = 0; i < muestras; i++) {
    suma += analogRead(sensorPin);
    delay(5);
  }

  offsetVoltaje = ((suma / (float)muestras) / 4095.0) * 3.3;
}

// ----- Lectura en Loop ------

void loop() {
  if (millis() - tiempoAnterior >= intervaloLectura) {
    tiempoAnterior = millis();

    long suma = 0;
    const int muestras = 50;                                    // Define cantidad de Muestras de Lectura

    for (int i = 0; i < muestras; i++) {
      suma += analogRead(sensorPin);
      delay(2);
    }

    float voltaje = ((suma / (float)muestras) / 4095.0) * 3.3;  // Convierte el valor de Lectura ADC a Voltaje 0-3.3V
    float corriente = (voltaje - offsetVoltaje) / 0.008;        // Convierte a valor de Corriente con sensibilidad del Sensor de 8mV

    // Eliminar ruido menor a 50 mA
    if (abs(corriente) < 0.05) {
      corriente = 0;
    }

    Serial.println(corriente);
  }
}
