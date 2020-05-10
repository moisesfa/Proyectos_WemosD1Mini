/***
 * Ejemplo básico para leer la Shield DHT22 Sensor de temperatura y humedad 
 * con un WeMos D1 Mini 
 * Usamos millis() en lugar de delay 
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

int dht22_tm = 4000;             // tiempo entre medidas 4 segundos
unsigned long dht22_lasttime;    // última actualización de tiempo

void setup() {
  Serial.begin(115200); 
  Serial.println("Prueba de la shield DHT22!");
  dht.begin();
}


void loop() {

  // Control de tiempos de la lectura del sensor
  if (millis() > dht22_lasttime + dht22_tm)  {

    //¡Leer la temperatura o la humedad toma alrededor de 250 milisegundos!
    // Las lecturas del sensor también pueden durar hasta 2 segundos (es un sensor muy lento)
    float hum = dht.readHumidity();
    float tem = dht.readTemperature();
    // Comprueba si alguna lectura falló y sale temprano (para intentarlo de nuevo).
    if (isnan(hum) || isnan(tem)) {
      Serial.println("¡Error al leer del sensor DHT!");
      return;
    }

    // Creo el string con 1 decimal y mostramos los valores leídos por el puerto serie
    String str_hum = "Humedad : " + String(hum, 1);
    Serial.print(str_hum);
    Serial.print(" %\t");
    String str_tem = "Temperatura : " + String(tem, 1) + " *C ";
    Serial.println(str_tem);
  
    dht22_lasttime = millis();
  }

}
