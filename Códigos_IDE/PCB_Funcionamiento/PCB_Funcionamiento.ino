#include <WiFi.h>                   // Librería conexión WiFi en ESP32
#include <WiFiClient.h>             // Librería para cliente TCP/IP 
#include <WebServer.h>              // Librería servidor web HTTP  ESP32
#include <ESPmDNS.h>                // Librería para habilitar mDNS 
#include <DHT.h>                    // Librería sensores DHT
#include <Wire.h>                   // Librería para comunicación I2C
#include <Adafruit_GFX.h>           // Librería gráfica Adafruit
#include <Adafruit_SSD1306.h>       // Librería pantallas OLED Adafruit

#define SCREEN_WIDTH 128            // Ancho de la pantalla OLED en píxeles
#define SCREEN_HEIGHT 64            // Alto de la pantalla OLED en píxeles
#define OLED_ADDR 0x3C              // Dirección I2C de la pantalla OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); 

const char *ssid = "iPhone de Hiago";      // Nombre (SSID) de la red WiFi a la que se desea conectar
const char *password = "senhasenha";       // Contraseña de la red WiFi

WebServer server(80);                      // Instancia del servidor web en el puerto 80
DHT dht(7, DHT11);                         // GPIO 7 sensor DHT11 

#define CURRENT_SENSOR_PIN 1              // GPIO 1 sensor de corriente

bool accesoConfirmado = false;            // Monitoreo fue activado 

// ------ Función Principal del Servidor Web (Página HTML) --------
void handleRoot() {
  char html[4000];
  snprintf(html, 4000,
           "<!DOCTYPE html>\
<html>\
  <head>\
    <meta charset='UTF-8'>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <title>Sistema de Monitoreo</title>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'>\
    <style>\
      body { margin: 0; padding: 0; font-family: Arial; background: #f0f0f0; color: #000; display: flex; flex-direction: column; height: 100vh; justify-content: space-between; }\
      .oscuro { background-color: #222 !important; color: #eee !important; }\
      .container { text-align: center; padding: 30px; margin: auto; background: white; border-radius: 10px; box-shadow: 0px 0px 15px rgba(0,0,0,0.1); }\
      .oscuro .container { background-color: #333; color: #fff; }\
      h2 { font-size: 2.5rem; margin-bottom: 20px; }\
      p { font-size: 2rem; margin: 10px 0; }\
      i { margin-right: 10px; }\
      .footer { display: flex; justify-content: space-between; padding: 20px; }\
      .boton-confirmar { margin-top: 20px; }\
      button { padding: 10px 20px; font-size: 1.2rem; cursor: pointer; }\
      #modalConfig { display: none; position: fixed; top: 0; left: 0; width: 100%%; height: 100%%; background: rgba(0,0,0,0.6); justify-content: center; align-items: center; }\
      .modal-content { background: white; padding: 30px; border-radius: 10px; width: 300px; text-align: center; }\
    </style>\
  </head>\
  <body id='body'>\
    <div class='container'>\
      <h2>Sistema de Monitoreo</h2>%s%s\
    </div>\
    <div class='footer'>\
      <button onclick=\"document.getElementById('modalConfig').style.display='flex'\">⚙️ Configuración</button>\
      <button onclick=\"document.body.classList.toggle('oscuro')\">�� Claro/Oscuro</button>\
    </div>\
    <div id='modalConfig'>\
      <div class='modal-content'>\
        <h3></h3>\
        <p></p>\
        <button onclick=\"document.getElementById('modalConfig').style.display='none'\">Cerrar</button>\
      </div>\
    </div>\
    <script>\
      %s\
    </script>\
  </body>\
</html>",
           accesoConfirmado ? "<p id='temp'><i class='fas fa-thermometer-half' style='color:#ca3517;'></i>Temperatura: -- °C</p>\
<p id='hum'><i class='fas fa-tint' style='color:#3498db;'></i>Humedad: -- %</p>\
<p id='volt'><i class='fas fa-bolt' style='color:#ffcc00;'></i>Voltaje: -- V</p>\
<p id='amp'><i class='fas fa-charging-station' style='color:#008000;'></i>Corriente: -- A</p>"
                           : "<p>Confirmar Conexión.</p>",
           accesoConfirmado ? "" : "<div class='boton-confirmar'><button onclick=\"location.href='/confirmar'\">Iniciar Monitoreo</button></div>",
           accesoConfirmado
               ? "setInterval(() => { fetch('/data').then(r => r.json()).then(d => {\
                   document.getElementById('temp').innerHTML = `<i class='fas fa-thermometer-half' style='color:#ca3517;'></i>Temperatura: ${d.temp.toFixed(2)} °C`;\
                   document.getElementById('hum').innerHTML = `<i class='fas fa-tint' style='color:#3498db;'></i>Humedad: ${d.hum.toFixed(2)} %`;\
                   document.getElementById('volt').innerHTML = `<i class='fas fa-bolt' style='color:#ffcc00;'></i>Voltaje: ${d.volt.toFixed(2)} V`;\
                   document.getElementById('amp').innerHTML = `<i class='fas fa-charging-station' style='color:#008000;'></i>Corriente: ${d.amp.toFixed(2)} A`;\
               }); }, 500);"
               : "");

  server.send(200, "text/html", html);
}

// ------ Función para Confirmación de Monitoreo--------
void handleConfirmar() {
  accesoConfirmado = true;                                 // Activa acceso para  monitoreo
  Serial.println("✅ Monitoreo activado por el usuario.");  
  server.sendHeader("Location", "/", true);                
  server.send(302, "text/plain", "");                      // Envía redirección con código HTTP 302 (Found)
}

// ------ Función para Enviar Datos en Formato JSON --------
void handleData() {
  float temp = readDHTTemperature();                       // VAR que almacena lectura de temperatura 
  float hum = readDHTHumidity();                           // VAR que almacena lectura de humedad
  float volt = readVoltage();                              // VAR que almacena lectura de voltaje
  float amp = readAmpere();                                // VAR que almacena lectura de corriente

  char json[200];                                          // Arreglo de caracteres para construir JSON
  snprintf(json, sizeof(json),                             // Formatea los valores como cadena JSON
           "{\"temp\":%.2f,\"hum\":%.2f,\"volt\":%.2f,\"amp\":%.2f}",
           temp, hum, volt, amp);

  server.send(200, "application/json", json);              // Envía la cadena JSON como respuesta HTTP
}


// ------ Función de Configuración Inicial --------
void setup() {
  Serial.begin(115200);                            // Inicia la comunicación serial a 115200 baudios para depuración
  dht.begin();                                     // Inicializa el sensor DHT11
  pinMode(CURRENT_SENSOR_PIN, INPUT);              // Configura el pin del sensor de corriente como entrada

  Wire.begin(5, 6);                                 // Inicia el bus I2C con SDA en GPIO 5 y SCL en GPIO 6
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    Serial.println("No se detectó la pantalla."); 
    while (true);                                  
  }

  WiFi.mode(WIFI_STA);                             // Configura el ESP32 como estación (cliente WiFi)
  WiFi.begin(ssid, password);                      // Inicia conexión a la red WiFi con SSID y contraseña

  while (WiFi.status() != WL_CONNECTED) {         
    delay(500);                                    
    Serial.print(".");                             // Muestra un punto en el monitor serial por cada intento
  }

  String ipStr = WiFi.localIP().toString();        // Obtiene la IP local asignada al ESP32
  Serial.println("\nConectado a WiFi");            // Mensaje de éxito
  Serial.print("IP: ");                            
  Serial.println(ipStr);                           // Muestra IP obtenida

  display.clearDisplay();                          
  display.setTextSize(2);                          
  display.setTextColor(SSD1306_WHITE);            
  display.setCursor(0, 0);                         
  display.println(ssid);                           // Muestra el nombre de red (SSID)
  display.setCursor(0, 32);                       
  display.println(ipStr);                          // Muestra la IP asignada
  display.display();                               // Envía los datos al display
  delay(3000);                                     

  if (MDNS.begin("esp32")) {                       // Inicia el servicio mDNS con nombre "esp32.local"
    Serial.println("MDNS responder iniciado");     // Mensaje de éxito
  }

  server.on("/", handleRoot);                      // Asocia la ruta raíz "/" con la función handleRoot
  server.on("/confirmar", handleConfirmar);        // Ruta para activar el monitoreo
  server.on("/data", handleData);                  // Ruta para devolver los datos en JSON
  server.begin();                                  // Inicia el servidor HTTP
  Serial.println("Servidor HTTP iniciado");        // Confirma en la consola
}


// ------ Función de Ejecución Continua (Loop Principal) --------
void loop() {
  server.handleClient();                          // Atiende cualquier solicitud entrante del cliente HTTP

  if (!accesoConfirmado) return;                  // Si no se ha confirmado el acceso, no hace nada

  float temp = readDHTTemperature();              // VAR que lee temperatura
  float humidity = readDHTHumidity();             // VAR que lee humedad
  float voltajeBat = readVoltage();               // VAR que lee voltaje
  float corriente = readAmpere();                 // VAR que lee corriente

  display.clearDisplay();                         
  display.setTextSize(2);                         
  display.setTextColor(SSD1306_WHITE);            
  display.setCursor(0, 0);                        
  display.printf("T:%.1fC\n", temp);              // Muestra la temperatura 

  display.setCursor(0, 16);                      
  display.printf("H:%.0f%%\n", humidity);         // Muestra la humedad 

  display.setCursor(0, 32);                       
  display.printf("V:%.2fV\n", voltajeBat);        // Muestra el voltaje 

  display.setCursor(0, 48);                       
  display.printf("A:%.2fA\n", corriente);         // Muestra la corriente 

  display.display();                              // Actualiza el contenido en pantalla

  delay(10);                                     
}

// ------ Función de Lectura de Temperatura --------
float readDHTTemperature() {
  return dht.readTemperature();  // Retorna la temperatura en grados Celsius
}

// ------ Función de Lectura de Humedad --------
float readDHTHumidity() {
  return dht.readHumidity();     // Retorna la humedad relativa (%) 
}

const int N_VOLT = 10;                    // Número de muestras para el filtro de voltaje
float muestrasVoltaje[N_VOLT];           // Arreglo que almacena las últimas N lecturas de voltaje

// ------ Función de Filtro de Media Móvil para Voltaje --------
float filtro_media_movil_voltaje(float lectura) {
  for (int i = N_VOLT - 1; i > 0; i--) {            // Desplaza todas las muestras una posición hacia la derecha
    muestrasVoltaje[i] = muestrasVoltaje[i - 1];
  }
  muestrasVoltaje[0] = lectura;                    // Coloca la nueva lectura en la primera posición del arreglo

  float suma = 0;
  for (int i = 0; i < N_VOLT; i++) {                // Suma todos los elementos del arreglo
    suma += muestrasVoltaje[i];
  }
  return suma / N_VOLT;                             // Retorna el promedio de las N muestras (valor suavizado)
}


// ------ Función de Lectura de Voltaje con Filtro --------
float readVoltage() {
  int analogValue = analogRead(3);                                         // Lee el valor analógico del pin A3 (0–4095)
  float voltajeADC = (analogValue / 4095.0) * 3.3;                         // Convierte el valor leido a voltaje en [0–3.3 V]
  float voltajeCrudo = (voltajeADC * ((10000.0 + 3300.0) / 3300.0)) + 1;   // Aplica el factor del divisor resistivo y offset
  float voltajeFiltrado = filtro_media_movil_voltaje(voltajeCrudo);        // Aplica el filtro de media móvil al voltaje
  return voltajeFiltrado;                                                  // Retorna el valor de voltaje suavizado
}

// ----------- Filtro Corriente (Media Móvil) -----------
const int N_MUESTRAS = 50;            // Número de muestras
float amostras[N_MUESTRAS];          // Arreglo que almacena las últimas lecturas

// ------ Función de Filtro de Media Móvil para Corriente --------
float filtro_media_movil(float lectura) {
  for (int i = N_MUESTRAS - 1; i > 0; i--) {  // Desplaza las muestras una posición a la derecha
    amostras[i] = amostras[i - 1];
  }
  amostras[0] = lectura;  // Inserta la nueva lectura al inicio del arreglo

  float suma = 0;
  for (int i = 0; i < N_MUESTRAS; i++) {  // Suma todas las lecturas almacenadas
    suma += amostras[i];
  }
  return suma / N_MUESTRAS;  // Retorna el promedio de las lecturas (valor suavizado)
}


// ------ Función de Lectura de Corriente con Calibración y Filtro --------
float readAmpere() {
  const int muestras = 50;                    // Número de muestras a
  const float sensibilidad = 0.01;            // Sensibilidad del sensor (10 mV)
  static float offsetVoltaje = 0;             // Offset  la última lectura
  static unsigned long tiempoAnterior = 0;    // Tiempo de
  static bool calibrado = false;              // Flag para calibrado
  static float corrienteFiltrada = 0;         // VAR corriente filtrada

  // --- Calibración Inicial del Sensor de Corriente ---
  if (!calibrado) {
    Serial.println("Calibrando sensor de corriente...");  
    delay(3000);                                         
    long suma = 0;
    for (int i = 0; i < 100; i++) {                       // Toma 100 muestras sin carga
      suma += analogRead(CURRENT_SENSOR_PIN);
      delay(5);
    }
    offsetVoltaje = ((suma / 100.0) / 4095.0) * 3.3;      // Calcula el voltaje de referencia (offset)
    Serial.print("Offset detectado: ");
    Serial.print(offsetVoltaje, 3);                       // Muestra el offset encontrado
    Serial.println(" V");
    calibrado = true;                                     // Marca la calibración como completada
  }

  // --- Lectura Periódica de Corriente con Filtrado ---
  if (millis() - tiempoAnterior >= 100) {                 // Verifica si pasó el intervalo de 100 ms
    tiempoAnterior = millis();                            // Actualiza el tiempo de la última lectura
    long suma = 0;
    for (int i = 0; i < muestras; i++) {                  // Toma varias lecturas y las promedia
      suma += analogRead(CURRENT_SENSOR_PIN);
      delay(2);
    }
    float voltaje = ((suma / (float)muestras) / 4095.0) * 3.3;  // Convierte a voltaje real
    float corrienteCruda = (voltaje - offsetVoltaje) / sensibilidad;  // Aplica la fórmula de corriente
    corrienteFiltrada = filtro_media_movil(corrienteCruda);          // Aplica el filtro de media móvil
    if (abs(corrienteFiltrada) < 0.05) corrienteFiltrada = 0;        // Elimina valores de ruido menores a 50 mA
    Serial.print("Corriente filtrada: ");
    Serial.print(corrienteFiltrada, 3);                               // Imprime el valor en amperios
    Serial.println(" A");
  }

  return corrienteFiltrada;                                           // Retorna la corriente medida y filtrada
}
