// Función para leer voltaje 
float readVoltage() {
  int analogValue = analogRead(3);                                         // Leer valor del ADC en el pin A3
  float voltajeADC = (analogValue / 4095.0) * 3.3;                         // Convertir lectura a voltios (para referencia de 3.3V)
  float voltajeTrue= (voltajeADC * ((10000.0 + 3300.0) / 3300.0)) + 1; // Calcular voltaje real, ajuste de conversión
  return voltajeTrue;
}

void setup() {
  Serial.begin(115200); // Iniciar comunicación serial a 115200 baudios
  delay(1000); // Retardo inicial
}

void loop() {
  float voltage = readVoltage(); // Leer voltaje
  Serial.println(voltage);       // Solo el valor numérico para el plotter
  delay(500);                    // Refrescar cada 0.5 segundos (puedes ajustar)
}
