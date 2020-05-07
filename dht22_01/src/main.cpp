/***
 * Ejemplo básico para leer la Shield DHT22 Sensor de temperatura y humedad 
 * con un WeMos D1 Mini
 * */

#include <Arduino.h>

// Librerias usadas incluidas en platformio.ini
#include <Adafruit_Sensor.h>
#include "DHT.h"

// Este es el pin que usa la Shield y coincide con el LED_BUILDIN
#define DHTPIN D4       // D4 o 2
#define DHTTYPE DHT22   // DHT 22 (AM2302)

// Inicializa el sensor DHT 
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(115200); 
  Serial.println("Prueba de la shield DHT22!");
  dht.begin();
}


void loop() {
  
  // Espere unos segundos entre mediciones.
  delay(2000);
  
  //¡Leer la temperatura o la humedad toma alrededor de 250 milisegundos!
  // Las lecturas del sensor también pueden durar hasta 2 segundos (es un sensor muy lento)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  // Comprueba si alguna lectura falló y sale temprano (para intentarlo de nuevo).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
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

}
