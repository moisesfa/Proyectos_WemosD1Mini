/**
 * Ejemplo básico de lectura de un sensor DHT22 en la WeMos D1 Mini 
 * desde un bot de telegram.
 * */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <UniversalTelegramBot.h>
// Library for interacting with the Telegram API
// Search for "Telegram" in the Library manager and install
// The universal Telegram library
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot


#include <ArduinoJson.h>
// Library used for parsing Json from the API responses
// NOTE: There is a breaking change in the 6.x.x version,
// install the 5.x.x version instead
// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------

#include <Adafruit_Sensor.h>
#include "DHT.h"

#define DHTPIN D4       // PIN del sensor dht22 
#define DHTTYPE DHT22   // DHT 22  (AM2302)

char ssid[] = "nombre de la red";         		  // el nombre de su red SSID
char password[] = "contraseña de la red";       // la contraseña de su red

#define TELEGRAM_BOT_TOKEN "token del bot de telegram"

//------- ---------------------- ------

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);

// Inicializa el sensor DHT
DHT dht(DHTPIN, DHTTYPE);

int Bot_mtbs = 1000; // tiempo medio entre escaneo de mensajes
long Bot_lasttime;   // la última vez que se realizó la exploración de mensajes
bool Start = false;

int dht22_mtbs = 4000; 
long dht22_lasttime;  
float hum;
float tem;
String str_tem = "";
String str_hum = "";

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/temperatura") {
      bot.sendMessage(chat_id, str_tem , "");
    }

    if (text == "/humedad") {
      bot.sendMessage(chat_id, str_hum , "");
    }
    
    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "Bot example.\n\n";
      welcome += "/temperatura : solicito la temperatura\n";
      welcome += "/humedad : solicito la humedad\n";
      //welcome += "/status : Returns current status of LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  dht.begin();
  Serial.println("DHTxx Begin()");
  
  // Establezca WiFi en modo estación y desconéctese de un AP si era anteriormente estaba conectado
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Intente conectarse a la red Wifi
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();

}

void loop() {
  
  // Control de tiempos del bot
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }

  // Control de tiempos del sensor
  if (millis() > dht22_lasttime + dht22_mtbs)  {
    hum = dht.readHumidity();
    tem = dht.readTemperature();
    if (isnan(hum) || isnan(tem)) {
        Serial.println("¡Error al leer del sensor DHT!");
        return;
    }
    // Creo el string 
    str_tem = "Temperatura : " + String(tem, 2);
    Serial.println(str_tem);
    str_hum = "Humedad : " + String(hum, 2);
    Serial.println(str_hum);
  
    dht22_lasttime = millis();
  }

  
}