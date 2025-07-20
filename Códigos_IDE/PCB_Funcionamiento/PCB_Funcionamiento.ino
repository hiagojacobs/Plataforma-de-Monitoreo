#include <WiFi.h>                   // Librería  WiFi ESP32
#include <WiFiClient.h>             // Librería Cliente TCP/IP 
#include <WebServer.h>              // Librería servidor web  ESP32
#include <ESPmDNS.h>                // Librería nombres locales con .local (mDNS)
#include <DHT.h>                    // Librería sensor DHT11 
#include <Wire.h>                   // Librería comunicación I2C
#include <Adafruit_GFX.h>           // Librería Adafruit OLED
#include <Adafruit_SSD1306.h>       // Librería Adafruit pantalla OLED controlador SSD1306

#define SCREEN_WIDTH 128            // Ancho de la pantalla OLED en píxeles
#define SCREEN_HEIGHT 64            // Alto de la pantalla OLED en píxeles
#define OLED_ADDR 0x3C              // Dirección I2C de la pantalla OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  

const char *ssid = "iPhone de Hiago";       // Nombre (SSID) de la red WiFi a la que se va a conectar
const char *password = "senhasenha";        // Contraseña de la red WiFi

WebServer server(80);                       // Crea servidor web en el puerto HTTP 80
DHT dht(7, DHT11);                          // GPIO 7 sensor DHT11 

#define CURRENT_SENSOR_PIN 1                // GPIO 1  sensor de corriente

bool accesoConfirmado = false;             // VAR para confirmar acesso

// ------ Función para Generar Página Web en Servidor Local --------
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
      <button onclick=\"document.body.classList.toggle('oscuro')\">🌙 Claro/Oscuro</button>\
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

// ------ Función para Confirmación de Botón de Acceso --------

void handleConfirmar() {
  accesoConfirmado = true;                                 // Activa el monitoreo al confirmar desde la interfaz web
  Serial.println("✅ Monitoreo activado por el usuario.");  // Muestra mensaje de confirmación en el monitor serial
  server.sendHeader("Location", "/", true);                // Envía cabecera para redireccionar al usuario a la página principal
  server.send(302, "text/plain", "");                      // Responde con código HTTP 302 (redirección temporal)
}

// ------ Función que entrega datos en formato JSON --------

void handleData() {
  float temp = readDHTTemperature();                       // Almacena valor de Función de Lectura de Temperatura
  float hum = readDHTHumidity();                           // Almacena valor de Función de Lectura de Humedad
  float volt = readVoltage();                              // Almacena valor de Función de Lectura de Voltaje
  float amp = readAmpere();                                // Almacena valor de Función de Lectura de Corriente

  char json[200];                                          // Crea un arreglo para almacenar la respuesta JSON
  snprintf(json, sizeof(json),                             // Formatea los datos en una cadena JSON
           "{\"temp\":%.2f,\"hum\":%.2f,\"volt\":%.2f,\"amp\":%.2f}",
           temp, hum, volt, amp);

  server.send(200, "application/json", json);              // Envía la respuesta al cliente con tipo MIME application/json
}

// ------ Función Principal de Definición--------

void setup() {
  Serial.begin(115200);                            // Inicia la comunicación serial a 115200 baudios 
  dht.begin();                                     // Inicia el sensor DHT11
  pinMode(CURRENT_SENSOR_PIN, INPUT);              // Configura el pin del sensor de corriente como entrada

  Wire.begin(5, 6);                                 // Inicia la comunicación I2C con SDA en GPIO 5 y SCL en GPIO 6
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {  
    Serial.println("No se detectó la pantalla.");  
    while (true);                                 
  }

  WiFi.mode(WIFI_STA);                             // Configura el ESP32 en modo estación (cliente WiFi)
  WiFi.begin(ssid, password);                      // Intenta conectarse a la red WiFi con las credenciales proporcionadas
  while (WiFi.status() != WL_CONNECTED) {          // Espera hasta que se conecte
    delay(500);                                    // Espera medio segundo entre intentos
    Serial.print(".");                             // Imprime un punto por cada intento para mostrar progreso
  }

  String ipStr = WiFi.localIP().toString();        // Obtiene la IP local asignada como string
  Serial.println("\nConectado a WiFi");            // Mensaje de conexión exitosa
  Serial.print("IP: ");                            // Muestra Texto de IP
  Serial.println(ipStr);                           // Muestra la IP

  display.clearDisplay();                          // Limpia la pantalla 
  display.setTextSize(2);                          // Tamaño Texto
  display.setTextColor(SSD1306_WHITE);             // Color Blanco
  display.setCursor(0, 0);                         
  display.println(ssid);                           // Muestra el nombre de red en pantalla
  display.setCursor(0, 32);                        
  display.println(ipStr);                          // Muestra la IP en pantalla
  display.display();                               
  delay(3000);                                     

  if (MDNS.begin("esp32")) {                       // Inicia el servicio mDNS para poder acceder vía esp32.local
    Serial.println("MDNS responder iniciado");     // Mensaje de confirmación
  }

  server.on("/", handleRoot);                      // Asocia la ruta raíz "/" a la función handleRoot()
  server.on("/confirmar", handleConfirmar);        // Ruta para activar monitoreo manualmente
  server.on("/data", handleData);                  // Ruta para obtener los datos en formato JSON
  server.begin();                                  // Inicia el servidor web
  Serial.println("Servidor HTTP iniciado");        // Confirma que el servidor está activo
}

// ------ Función de Ejecución Continua--------

void loop() {
  server.handleClient();                          // Atiende peticiones HTTP entrantes (si las hay)

  if (!accesoConfirmado) return;                  // Si no se confirmó acceso, no se realiza monitoreo ni se actualiza pantalla

  float temp = readDHTTemperature();              // VAR de temperatura
  float humidity = readDHTHumidity();             // VAR de humedad
  float voltajeBat = readVoltage();               // VAR de voltaje
  float corriente = readAmpere();                 // VAR de corriente

  display.clearDisplay();                         
  display.setTextSize(2);                         
  display.setTextColor(SSD1306_WHITE);            
  display.setCursor(0, 0);                        
  display.printf("T:%.1fC\n", temp);              // Muestra temperatura en Display

  display.setCursor(0, 16);                       
  display.printf("H:%.0f%%\n", humidity);         // Muestra humedad en Display

  display.setCursor(0, 32);                       
  display.printf("V:%.2fV\n", voltajeBat);        // Muestra voltaje en Display

  display.setCursor(0, 48);                       
  display.printf("A:%.2fA\n", corriente);         // Muestra corriente en Display

  display.display();                              

  delay(10);                                      
}

// ------ Función de Lectura de Temperatura --------
float readDHTTemperature() {
  return dht.readTemperature();  // Retorna la temperatura en grados Celsius medida por el sensor DHT11
}

// ------ Función de Lectura de Humedad --------
float readDHTHumidity() {
  return dht.readHumidity();     // Retorna la humedad relativa (%) medida por el sensor DHT11
}

// ------ Función de Lectura de Voltaje --------
float readVoltage() {
  int analogValue = analogRead(3);                       // Lee el valor analógico del pin A3 (0 a 4095)
  float voltajeADC = (analogValue / 4095.0) * 3.3;       // Convierte el valor leído a voltaje en escala de 0 a 3.3V
  return (voltajeADC * ((10000 + 3300.0) / 3300.0)) + 1; // Ajusta el valor según el divisor resistivo (10k + 3.3k) y agrega offset
}


// ------ Función de Lectura de Corriente --------
float readAmpere() {
  const int muestras = 50;                          // Número de muestras 
  const float sensibilidad = 0.01;                  // Sensibilidad del sensor en V/A (10 mV)
  static float offsetVoltaje = 0;                   // Offset de voltaje
  static unsigned long tiempoAnterior = 0;          // Tiempo de la última medición 
  static bool calibrado = false;                    // VAR que indicad calibración
  static float corriente = 0;                       // VAR que almacena valor de corriente

  if (!calibrado) {                                 // Si no está calibrado
    Serial.println("Calibrando sensor de corriente... ");
    delay(3000);                                    
    long suma = 0;
    for (int i = 0; i < 100; i++) {                 // Realiza 100 lecturas para calcular el promedio del offset
      suma += analogRead(CURRENT_SENSOR_PIN);
      delay(5);                                     
    }
    offsetVoltaje = ((suma / 100.0) / 4095.0) * 3.3; // Convierte el promedio leído a voltaje
    Serial.print("Offset detectado: ");             // Muestra el offset calculado
    Serial.print(offsetVoltaje, 3);
    Serial.println(" V");
    calibrado = true;                               // Marca como calibrado para no repetir este bloque
  }

  if (millis() - tiempoAnterior >= 1000) {          // Solo mide corriente cada 1 segundo
    tiempoAnterior = millis();                      // Actualiza el tiempo de la última medición
    long suma = 0;
    for (int i = 0; i < muestras; i++) {            // Realiza múltiples lecturas para suavizar la señal
      suma += analogRead(CURRENT_SENSOR_PIN);
      delay(2);
    }
    float voltaje = ((suma / (float)muestras) / 4095.0) * 3.3; // Promedia y convierte el valor a voltaje
    corriente = (voltaje - offsetVoltaje) / sensibilidad;      // Calcula corriente aplicando la sensibilidad

    if (abs(corriente) < 0.05) corriente = 0;       // Elimina pequeñas variaciones (ruido menor a 50 mA)

    Serial.print("Corriente: ");                    // Muestra el resultado en el monitor serial
    Serial.print(corriente, 3);
    Serial.println(" A");
  }

  return corriente;                                 // Retorna el valor de corriente calculado
}

