/***
 * Ejemplo básico de Deep Sleep - Sueño profundo con un WeMos D1 Mini
 * Parpadea el led se a dormir durante 10 segundos
 * Es necesario unir los pines D0 y RST para que se repita el proceso
 * Para reprogramar el ESP, desconecte la unión D0 - RST 
 * 
 * Este código y más documentación en:
 * 
 * http://jdelacasa.es/2016/08/30/wemos-d1-mini-consumo-en-modo-deepsleep/
 * https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
 * 
 * */

#include <Arduino.h>

// Duerme durante 10 segundos => 10e6 => 10 * 1000000
const int segundosDormido = 10e6;
const int segundos = segundosDormido / 1000000;
 
void setup() {
  
  Serial.begin(115200);

  // Espere a que se inicie el serial.
  while(!Serial) { }

  Serial.println("\n\nDespierto");
  
  // Led en placa como salida
  pinMode(LED_BUILTIN, OUTPUT);
 
  // Conecte D0 a RST para despertarse
  pinMode(D0, WAKEUP_PULLUP);
 
  // LED: LOW = on, HIGH = off
  Serial.println("Comienza a parpadear");
  for (int i = 0; i < 20; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
  Serial.println("Deja de parpadear");
 
  Serial.printf("Dormir durante %d segundos\n\n", segundos);
 
  // Instrucción para ponerse a dormir
  ESP.deepSleep(segundosDormido);
}
 
void loop() {
  // Vacío
}