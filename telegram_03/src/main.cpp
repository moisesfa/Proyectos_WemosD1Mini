/**
 * Ejemplo de lectura de un sensor DHT22 en la WeMos D1 Mini 
 * desde un bot de telegram con almacenamiento de mínimos y máximos
 * También permite activar alarmas de temperatura y/o humedad
 * 
 * Referencia
 * http://projects.whatimean.ch/2017/10/08/esp8266-telegram-sensor-bot/
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

char ssid[] = "nombre de la red"         	// el nombbre de su red SSID
char password[] = "contraseña de la red";       // la contraseña de su red

#define TELEGRAM_BOT_TOKEN "token del bot de telegram"
#define CHAT_ID_PROPIO "su chat id para uso restringido"
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
//float hum;
//float tem;
// minimo 0 acutal 1 máximo 2
float tem[3] = {420, -127, -127};
float hum[3] = {420, -127, -127};

float TemL = 25;   //temperature límite
byte TemDL = 0;    //Dirección del límite; 0: off; 1: if tem<temL; 2: if tem>temL
float HumL = 50;   //humedad límite
byte HumDL = 0;    //Dirección del límite; 0: off; 1: if hum<humL; 2: if hum>humL

String str_tem = "";
String str_hum = "";

String SState() {
  String msg = "Bot con DHT22.\n";
  msg += "Temperatura: " + String(tem[1], 2) + " °C\n";
  msg += "Humedad Relativa: " + String(hum[1], 2) + " %RH\n";
  msg += "Mínimos:\n" + String(tem[0]) + " °C - " + String(hum[0]) + " %RH\n";
  msg += "Máximos:\n" + String(tem[2]) + " °C - " + String(hum[2]) + " %RH\n";
  msg += "Límites de alarma y estado:\n";
  msg += "T: " + String(TemL) + " °C (" + String(TemDL) + ")\n";
  msg += "RH: " + String(HumL) + " %RH (" + String(HumDL) + ")\n";
  return msg;
}

void MinMax(){
  
  //Guardar valores mínimos y máximos
  if (tem[1] < tem[0]) {
    tem[0] = tem[1];
  }
  if (tem[1] > tem[2]) {
    tem[2] = tem[1];
  }
  if (hum[1] < hum[0]) {
    hum[0] = hum[1];
  }
  if (hum[1] > hum[2]) {
    hum[2] = hum[1];
  }

}


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    
    // Recibo el chat_id y el texto del mensaje 
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    // Si no tiene nombre le pongo Invitado
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Invitado";
    
    String msg;

    // Comandos
    if (text == "/start") {
      String welcome = "Bienvenido a la librería Universal Arduino Telegram Bot, " + from_name + ".\n";
      welcome += "Bot con DHT22.\n\n";
      welcome += "/temperatura : petición de temperatura\n";
      welcome += "/humedad : petición de humedad\n";
      welcome += "/estado : Obtener estado y parametros del sensor\n";
      welcome += "/alarmaT30 : Activar alarma a temperatura 30\n";
      welcome += "/alarmaH60 : Activar alarma a humedad 60\n";
      welcome += "/alarmaT : Desactivar alarma de temperatura\n";
      welcome += "/alarmaH : Desactivar alarma de humedad\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }

    if (text == "/temperatura") {
      bot.sendMessage(chat_id, str_tem , "");
    }

    if (text == "/humedad") {
      bot.sendMessage(chat_id, str_hum , "");
    }
    
    if (text == "/estado") {
      bot.sendMessage(chat_id, SState(), ""); //envia el estado
    }
    
    if (text.startsWith("/alarmaT")) {
      //if (chat_id == CHAT_ID_PROPIO) {
        if (text.substring(8) != "") {
          TemL = text.substring(8).toFloat();
          if (TemL > tem[1]) {
            TemDL = 2;
            msg = "Te notificaré una vez que la temperatura aumente a:\n";
          } else {
            TemDL = 1;
            msg = "Te notificaré una vez que la temperatura baje a:\n";
          }
          msg += String(TemL) + " °C";
          bot.sendMessage(chat_id, msg, "");
        } else {
          TemDL = 0; 
          msg = "Alarma desactivada";
          bot.sendMessage(chat_id, msg, "");
        }
      //}
      }

      if (text.startsWith("/alarmaH")) {
      //if (chat_id == CHAT_ID_PROPIO) {
        if (text.substring(8) != "") {
          HumL = text.substring(8).toFloat();
          if (HumL > hum[1]) {
            HumDL = 2;
            msg = "Te notificaré una vez que la temperatura aumente a:\n";
          } else {
            HumDL = 1;
            msg = "Te notificaré una vez que la temperatura baje a:\n";
          }
          msg += String(HumL) + " °C";
          bot.sendMessage(chat_id, msg, "");
        } else {
          HumDL = 0; 
          msg = "Alarma desactivada";
          bot.sendMessage(chat_id, msg, "");
        }
      //}
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

  // Comunicación de manera no segura
  //bot._debug = true;
  //client.setInsecure();
  
  ///In firefox enter your bot address (https://api.telegram.org/botXXXXXX.....) then right-click on an 
  ///empty area of the web page and select "View Page Info" from the context menu, in the next Info Window 
  //go to the last tab "Security", click on "View certificate" and a new window will appear. 
  //In the bottom there is a group "Fingerprints". Use SHA1 Fingerprint
  
  // Comunicación de manera segura
  bot._debug = true;
  const uint8_t fingerprint[20] = { 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 };
  client.setFingerprint(fingerprint); 
}

void loop() {
  String msg;
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
    hum[1] = dht.readHumidity();
    tem[1] = dht.readTemperature();
    if (isnan(hum[1]) || isnan(tem[1])) {
        Serial.println("¡Error al leer del sensor DHT!");
        return;
    }
    
    
    // Almacena valores mínimos y máximos 
    MinMax();
    
    // Creo el string y lo envio por el puerto serie
    str_tem = "Temperatura : " + String(tem[1], 2);
    Serial.println(str_tem);
    str_hum = "Humedad : " + String(hum[1], 2);
    Serial.println(str_hum);
  
    //Alarmas temperatura
    if (TemDL == 2) {
      if (TemL < tem[1]) {
        TemDL = 0;
        msg = "La temperatura alcanzada:\n";
        msg += String(TemL) + " °C\n";
        bot.sendMessage(CHAT_ID_PROPIO, msg, "");
        bot.sendMessage(CHAT_ID_PROPIO, SState(), "");  //send status
      }
    }
    if (TemDL == 1) {
      if (TemL > tem[1]) {
        TemDL = 0;
        msg = "La temperatura alcanzada:\n";
        msg += String(TemL) + " °C\n";
        bot.sendMessage(CHAT_ID_PROPIO, msg, "");
        bot.sendMessage(CHAT_ID_PROPIO, SState(), "");  //send status
      }
    }
    //Alarmas humedad
    if (HumDL == 2) {
      if (HumL < hum[1]) {
        HumDL = 0;
        msg = "La humedad alcanzada:\n";
        msg += String(HumL) + " %RH\n";
        bot.sendMessage(CHAT_ID_PROPIO, msg, "");
        bot.sendMessage(CHAT_ID_PROPIO, SState(), "");  //send status
      }
    }
    if (HumDL == 1) {
      if (HumL > hum[1]) {
        HumDL = 0;
        msg = "La humedad alcanzada:\n";
        msg += String(HumL) + " %RH\n";
        bot.sendMessage(CHAT_ID_PROPIO, msg, "");
        bot.sendMessage(CHAT_ID_PROPIO, SState(), "");  //send status
      }
    }
    
    dht22_lasttime = millis();
  }
  
  
}