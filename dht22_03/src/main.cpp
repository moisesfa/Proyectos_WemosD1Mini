/***
 * Ejemplo básico para leer la Shield DHT22 Sensor de temperatura y humedad 
 * con un WeMos D1 Mini
 * Parpadea el led y se va a dormir durante 1 minuto (se puede modificar hasta unos 71 minutos)
 * Es necesario unir los pines D0 y RST para que se repita el proceso
 * Para reprogramar el ESP, desconecte la unión D0 - RST
 * */

#include <Arduino.h>

// Librerias usadas incluidas en platformio.ini
#include <Adafruit_Sensor.h>
#include "DHT.h"

// Este es el pin que usa la Shield y coincide con el LED_BUILDIN
#define DHTPIN D4       // D4 o 2
#define DHTTYPE DHT22   // DHT 22 (AM2302)

// El valor máximo para un entero sin signo de 32 bits es 4294967295 o 0xffffffff. 
// Por lo tanto, el intervalo máximo de sueño profundo parece ser de ~ 71 minutos. 
// Para probar dormir durante un minuto

//uint32_t minutosDormido = 60 * 60 * 1000000;
//uint32_t minutosDormido = 3600000000;
const uint32 minutosDormido = 60e6;
const uint32 minutos = minutosDormido / 60000000;

void setup() {

  Serial.begin(115200);
  // Esperamos a que se inicialize el puerto serie
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
 
  Serial.println("Lectura del sensor DHT22");
  
  // Pongo el pin como entrada para que pueda leer el sensor
  // Antes lo uso como led  
  pinMode(LED_BUILTIN, INPUT);
  // Inicializa el sensor DHT 
  DHT dht(DHTPIN, DHTTYPE);
  dht.begin();

  // Espere unos segundos.
  delay(2000);
  
  // ¡Leer la temperatura o la humedad toma alrededor de 250 milisegundos!
  // Las lecturas del sensor también pueden durar hasta 2 segundos (es un sensor muy lento)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Comprueba si alguna lectura falló y sale temprano (para intentarlo de nuevo).
  if (isnan(h) || isnan(t)) {
    Serial.println("¡Error al leer el sensor DHT!");
    return;
  }

  // Mostramos los valores leídos por el puerto serie
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.println("");


  //Pausa antes del sleep
  delay(1000);
  Serial.printf("Dormir durante %d minutos\n\n", minutos);
  // Instrucción para ponerse a dormir
  ESP.deepSleep(minutosDormido);

}


void loop() {
  // Vacío
}
