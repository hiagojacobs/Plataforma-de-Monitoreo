#define BUZZER_PIN 4

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Ingrese número del 1 al 5:");
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();

    if (input >= '1' && input <= '5') {
      int nivel = input - '0';  
      int frecuencia = nivel * 500;  // 500Hz, 1000Hz, 1500Hz, etc.

      Serial.print("Frecuencia: ");
      Serial.print(frecuencia);
      Serial.println(" Hz");

      tone(BUZZER_PIN, frecuencia);
      delay(500);               // Suena por 0.5 segundos
      noTone(BUZZER_PIN);
    } else {
      Serial.println("Número entre 1 y 5");
    }
  }
}
