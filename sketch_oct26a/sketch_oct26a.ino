#include <ESP8266WiFi.h>

const char* ssid = "ATEL-VICTOR_TURBO";  // Rede WiFi
const char* password = "14081019";  // Senha da Rede WiFi

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Conectando");
  while( WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }
  Serial.println("Conectado");

}

void loop() {
  // Coloque seu código principal aqui, para ser executado repetidamente
}