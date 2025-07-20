#include <Wire.h>                      // Librería para comunicación I2C
#include <Adafruit_SSD1306.h>          // Librería para controlar pantallas OLED SSD1306

// Dimensiones de pantalla OLED
#define SCREEN_WIDTH 128              // Ancho del display OLED en píxeles
#define SCREEN_HEIGHT 64              // Alto del display OLED en píxeles
#define OLED_RESET    -1              // No se utiliza el pin de reset físico

// Instancia del objeto display con las dimensiones y referencia al bus I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Wire.begin(5, 6);                   // Inicializa I2C con SDA en GPIO 5 y SCL en GPIO 6 

  // Inicializa la pantalla con dirección I2C 0x3C y modo de alimentación con condensador interno
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();            // Limpia la pantalla para comenzar desde cero

  // -------- Línea 1 --------
  display.setTextSize(1.5);          // Establece el tamaño de texto , 1.5
  display.setTextColor(WHITE);       // Color del texto: blanco
  display.setCursor(0, 0);           // Posición inicial: esquina superior izquierda
  display.println("MINI DISPLAY I2C"); // Muestra el texto

  // -------- Línea 2 --------
  display.setCursor(0, 10);          // Mueve el cursor a la segunda línea
  display.setTextColor(BLACK, WHITE); // Texto negro sobre fondo blanco (modo invertido)
  display.println("TESTE");

  // -------- Línea 3 --------
  display.setCursor(0, 20);          // Tercera línea
  display.setTextColor(WHITE);       // Texto blanco normal
  display.println("DE");

  // -------- Línea 4 --------
  display.setCursor(0, 30);          // Cuarta línea
  display.setTextColor(BLACK, WHITE); // Modo invertido 
  display.println("FUNCIONAMIENTO");

  // -------- Línea 5 --------
  display.setCursor(0, 40);          // Quinta línea
  display.setTextColor(WHITE);       // Texto blanco normal
  display.println("TODO OK!! :)");

  display.display();                 // Actualiza el contenido en la pantalla física
}

void loop() {
  // Nada 
}
