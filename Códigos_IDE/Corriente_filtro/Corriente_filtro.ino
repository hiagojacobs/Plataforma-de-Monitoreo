const int sensorPin = 1;                // Pin de Lectura del Sensor
float offsetVoltaje = 0;                // VAR de offset
unsigned long tiempoAnterior = 0;       // VAR de almacenamiento
const int intervaloLectura = 100;       // Intervalo en milisegundos (ajustable para mejor resolución)

// ------ Filtro de media móvil ---------

const int n = 50;                       // Numero de Muestras
float amostras[n];                      // Vector global de muestras para el filtro

float filtro_media_movel(float lecturaCorrente)
{
  // Desplazamiento de elementos del vector
  for (int i = n - 1; i > 0; i--) amostras[i] = amostras[i - 1];

  amostras[0] = lecturaCorrente;

  float acc = 0;
  for (int i = 0; i < n; i++) acc += amostras[i];

  return acc / n;  // Retorna la media, promedio de numero de lecturas definido
}

// // ------ Lectura Inicial ---------

void setup() {
  Serial.begin(115200);
  delay(3000);  // Tiempo para estabilizar

  long suma = 0;
  const int muestras = 100;

  for (int i = 0; i < muestras; i++) {
    suma += analogRead(sensorPin);
    delay(5);
  }

  offsetVoltaje = ((suma / (float)muestras) / 4095.0) * 3.3;  //Convierte la media ADC a voltaje real (0–3.3 V)

  Serial.println("Cruda\tFiltrada");  // Encabezado para el Plotter
}

// // ------ Lectura en Loop ---------

void loop() {
  if (millis() - tiempoAnterior >= intervaloLectura) { // A cada 100ms realiza nueva lectura 
    tiempoAnterior = millis();

    int lecturaADC = analogRead(sensorPin);
    float voltaje = (lecturaADC / 4095.0) * 3.3;                // Convierte la lectura a señal de voltaje
    float corrienteCruda = (voltaje - offsetVoltaje) / 0.008;  // Calcula la corriente medida usando la sensibilidad del sensor de 8mV

    float corrienteFiltrada = filtro_media_movel(corrienteCruda);  //aplica el filtro de media movil 

    // Eliminar ruido menor a 50 m
    if (abs(corrienteCruda) < 0.05) corrienteCruda = 0;
    if (abs(corrienteFiltrada) < 0.05) corrienteFiltrada = 0;

    // Imprime ambos valores con etiquetas para el Serial Plotter
    Serial.print(corrienteCruda, 2);
    Serial.print('\t');                       
    Serial.println(corrienteFiltrada, 2);
  }
}
