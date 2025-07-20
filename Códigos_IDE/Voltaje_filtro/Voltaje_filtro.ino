// Parametros del filtro
const int n = 10;
float muestrasVoltaje[n];

// Función de Filtro media móvil 
float filtro_media_movel(float lectura) {
  for (int i = n - 1; i > 0; i--) muestrasVoltaje[i] = muestrasVoltaje[i - 1];
  muestrasVoltaje[0] = lectura;

  float acc = 0;
  for (int i = 0; i < n; i++) acc += muestrasVoltaje[i];

  return acc / n;
}

// Lectura de voltaje
float readVoltage() {
  int analogValue = analogRead(3);                                         // Leer ADC
  float voltajeADC = (analogValue / 4095.0) * 3.3;                         // Convierte a voltios
  float voltajeBattery = (voltajeADC * ((10000.0 + 3300.0) / 3300.0)) + 1; // Ajuste por divisor resistivo y offset
  return voltajeBattery;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("VoltajeCrudo\tVoltajeFiltrado");  // Etiquetas para el Serial Plotter
}

void loop() {
  float voltajeCrudo = readVoltage();
  float voltajeFiltrado = filtro_media_movel(voltajeCrudo);    

  // Salida tabulada para dos curvas
  Serial.print(voltajeCrudo, 2);
  Serial.print('\t');
  Serial.println(voltajeFiltrado, 2);

  delay(50);  // Refresca cada 0.05s
}
