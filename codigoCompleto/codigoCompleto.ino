/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  Small bug-fix by Michael cochez

  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <ArduinoJson.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Definição do endereço MAC do módulo Ethernet
IPAddress ip(192, 168, 2, 182);                       // IP do Servidor local do módulo Ethernet

EthernetClient client;    // Definição do objeto client (requisição a API)
HttpClient http(client);  // Definição do objeto http (forma como o programa fará a execução)

unsigned long startTime = 0;         //
LiquidCrystal_I2C lcd(0x26, 16, 2);  // Padrão do módulo LCD 16 caracteres em linha, contendo duas colunas
LiquidCrystal_I2C lcd2(0x27, 16, 2);  // Padrão do módulo LCD 16 caracteres em linha, contendo duas colunas

byte SIMB1[8] = { B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111 };
byte SIMB2[8] = { B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111 };
byte SIMB3[8] = { B11111,
                  B11111,
                  B11111,
                  B11110,
                  B11110,
                  B11111,
                  B11111,
                  B11111 };
byte SIMB4[8] = { B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110 };
byte SIMB5[8] = { B11100,
                  B11100,
                  B11100,
                  B00000,
                  B00000,
                  B11100,
                  B11100,
                  B11100 };  // Letras personalizadas no leitor LCD, cada um desses bytes imprime em um dos caracteres do LCD




#define Password_Length 8
int signalPin = 7;
char Data[Password_Length];
char Master[Password_Length] = "1234567";
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4;  //Define a quantidade de linhas do teclado (4)
const byte COLS = 3;  //Define a quantidade de colunas do teclado (3)
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};                                        // Padrão do teclado que estamos usando
byte rowPins[ROWS] = { 32, 33, 34, 35 };  // Pinos de conexão referente a cada linha
byte colPins[COLS] = { 36, 37, 38 };      // Pinos de conexão referente a cada coluna

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  // Com as informações anteriores do teclado, aqui instanciamos o teclado
int getFingerprintIDez();
#if (defined(_AVR) || defined(ESP8266)) && !defined(AVR_ATmega2560_)
// Para placas como o Arduino UNO, usamos SoftwareSerial

SoftwareSerial leitor1(14, 15); // Leitor biométrico 1
SoftwareSerial leitor2(16, 17); // Leitor biométrico 2
#define SERIAL1_PTR &leitor1
#define SERIAL2_PTR &leitor2

#else
// Para placas com várias portas seriais (ex: Mega 2560)
#define SERIAL1_PTR &Serial3
#define SERIAL2_PTR &Serial2

#endif

// Agora passamos os ponteiros corretamente
Adafruit_Fingerprint finger(SERIAL1_PTR);
Adafruit_Fingerprint finger2(SERIAL2_PTR);


uint8_t id;
uint8_t nLeitor;

int ledVerde = 13;
int ledAmarelo = 12;
int ledVermelho = 11;
int buzzer = 10;
int ledVerde2 = 8;
int ledAmarelo2 = 7;
int ledVermelho2 = 6;
int buzzer2 = 5;
int pinLed = 12;
int rele = 9;

String nome = "Vini";
char cadstrar = '#';
String senhaDigitada;


void setup()
{
  
  while (!Serial) continue;  // For Yun/Leo/Micro/Zero/...

  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  //Definição do endereço MAC do módulo Ethernet
  if (!Ethernet.begin(mac)) {                           //Verifica se o endereço MAC condiz com o do proprio módulo ethernet
    lcd.println(F("Failed to configure Ethernet"));
    return;
  }
  delay(1000);  // Aguarda 1 segundo
  lcd.println(F("Conectando..."));  // Imprime Conectando...

  pinMode(signalPin, OUTPUT);
  Serial.begin(9600);  //Inicia o serial na frequencia de 9600
  lcd.init();          // Inicia o módulo lcd
  lcd.backlight();     // Inicia a luz de fundo do módulo lcd
  lcd2.init();
  lcd2.backlight();
  aguardando();        //Chama função aguardando que inicia o objeto para imprimir as letras personalizadas
  aguardando2();
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(rele, OUTPUT);

  // set the data rate for the sensor serial port
  finger.begin(57600);
  finger2.begin(57600);
  if (data_count == Password_Length - 1) {
    lcd.clear();                                             // Limpa o que estiver escrito no modulo lcd
    lcd.print("Informe seu ID:");                         // Pede ao usuário para digitar o ID da biometria
    if (finger.storeModel(id) == FINGERPRINT_BADLOCATION) {  // Verifica se a digital é válida ou não
      lcd.clear();                                           // Limpa o que estiver escrito no modulo lcd
      lcd.setCursor(0, 0);                                   // Define o que será mostrado no lcd na a partir da linha 0, coluna 0
      lcd.print("Digital inválida");
      lcd.setCursor(0, 1);  // Define o que será mostrado no lcd na a partir da linha 0, coluna 1
      lcd.print("Tente novamente");
      return;
    }
    lcd.clear();
    lcd.print("Gravando ID: " + id);  // Mostra ao usuário que a digital será gravada em determinado ID
    while (!getFingerprintEnroll(0))
      ;
  }
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
void aguardando() {          // Função que instancia as letras personalizadas no módulo LCD
  lcd.createChar(0, SIMB1);  //Instanciando os pixels
  lcd.createChar(1, SIMB2);
  lcd.createChar(2, SIMB3);
  lcd.createChar(3, SIMB4);
  lcd.createChar(4, SIMB5);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write((uint8_t)0);  // Imprime a letra "I"
  lcd.setCursor(0, 1);
  lcd.write((uint8_t)1);  // Imprime a letra "F"
  lcd.setCursor(1, 0);
  lcd.write((uint8_t)2);
  lcd.setCursor(1, 1);
  lcd.write((uint8_t)3);
  lcd.setCursor(2, 0);
  lcd.write((uint8_t)4);

  lcd.setCursor(3, 0);
  lcd.print("Aguardando o ");   // Imprime a mensagem fixa
  lcd.setCursor(3, 1);             // seta o texto a seguir a partir do oitavo índice do lcd
  lcd.print("Acesso ao Lab ");  // Aqui fica impressa a data (como não estou com o módulo de relógio estou imprimindo este texto para simular)
}

void aguardando2() {          // Função que instancia as letras personalizadas no módulo LCD
  lcd2.createChar(0, SIMB1);  //Instanciando os pixels
  lcd2.createChar(1, SIMB2);
  lcd2.createChar(2, SIMB3);
  lcd2.createChar(3, SIMB4);
  lcd2.createChar(4, SIMB5);

  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.write((uint8_t)0);  // Imprime a letra "I"
  lcd2.setCursor(0, 1);
  lcd2.write((uint8_t)1);  // Imprime a letra "F"
  lcd2.setCursor(1, 0);
  lcd2.write((uint8_t)2);
  lcd2.setCursor(1, 1);
  lcd2.write((uint8_t)3);
  lcd2.setCursor(2, 0);
  lcd2.write((uint8_t)4);

  lcd2.setCursor(3, 0);
  lcd2.print("Aguardando o ");   // Imprime a mensagem fixa
  lcd2.setCursor(3, 1);             // seta o texto a seguir a partir do oitavo índice do lcd2
  lcd2.print("Acesso ao Lab ");  // Aqui fica impressa a data (como não estou com o módulo de relógio estou imprimindo este texto para simular)
}
void loop()                     // run over and over again
{
  getFingerprintIDez();
  char key = keypad.getKey();  //Aqui pegamos a tecla digitada no teclado

  if (key) {
    lcd.clear();
    lcd2.clear();
    String senhaDigitada = senhaCadastro(key); 
    lcd.clear();
    lcd2.clear();                              // Mostramos a tecla digitada
    if (senhaDigitada == "40028") {
      lcd.print("DIGITE SEU ID: ");  // Verifica se a senha "1234" foi digitada
      int codigoDigitado = checkPassword();
      lcd.clear();            // Armazena as informações do id em uma variavel
      lcd.println(codigoDigitado);                  // Exibe a variavel com o id
      bool autorizado = consultarApi(codigoDigitado);  // Verifica na API se o id digitado corresponde ao de algum usuário dentro da APi
      if (autorizado) {                                // Se o id digitado corresponder, abre o modo de cadastramento para a biometria
        getFingerprintEnroll(codigoDigitado);
        aguardando();
        // enviarParaApi("Vinicio", "vinicio.eufrazio@aluno.ifsertao-pe.edu.br", codigoDigitado);
      } else {  // caso contrário não permite cadastrar
        lcd.println("ID INVÁLIDO");
        aguardando();
      }
    }else if(senhaDigitada == "67843"){
      digitalWrite(rele,HIGH);
      
      delay(5000);
      digitalWrite(rele, LOW);
      aguardando();
      aguardando2();
    }else if(senhaDigitada == "28004"){
      lcd2.print("DIGITE SEU ID: ");  // Verifica se a senha "1234" foi digitada
      int codigoDigitado = checkPassword();
      lcd2.clear();            // Armazena as informações do id em uma variavel
      lcd2.println(codigoDigitado);                  // Exibe a variavel com o id
      bool autorizado = consultarApi(codigoDigitado);  // Verifica na API se o id digitado corresponde ao de algum usuário dentro da APi
      if (autorizado) {                                // Se o id digitado corresponder, abre o modo de cadastramento para a biometria
        getFingerprintEnroll(codigoDigitado);
        aguardando2();
        // enviarParaApi("Vinicio", "vinicio.eufrazio@aluno.ifsertao-pe.edu.br", codigoDigitado);
      } else {  // caso contrário não permite cadastrar
        lcd2.println("ID INVÁLIDO");
        aguardando2();
      }
    }
    else{
      lcd.print("SENHA INCORRETA");
      delay(3000);
      aguardando();
      aguardando2();
    }
  }

  // Serial.println("Escolha o leitor para cadastro [1] [2]:");
  // nLeitor = readnumber();
  // if(nLeitor == 1){
  //   Serial.println("Leitor 1!");
  //   Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  //   id = readnumber();
  //   if (id == 0) {// ID #0 not allowed, try again!
  //     return;
  //   }
  //   Serial.print("Enrolling ID #");
  //   Serial.println(id);
  //   while (! getFingerprintEnroll() );
  // }else{
  //   Serial.println("Leitor 2!");
  //   Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  //   id = readnumber();
  //   if (id == 0) {// ID #0 not allowed, try again!
  //     return;
  //   }
  //   Serial.print("Enrolling ID #");
  //   Serial.println(id);
  //   while (! getFingerprintEnroll2() );
  // }
  
}
int checkPassword() {
  char enteredPassword[2];  // Aumentado para incluir o caractere nulo de terminação de string
  uint8_t index = 0;

  // Aguarda o usuário inserir a senha
  while (index < 2) {
    char key = keypad.getKey();
    if (key) {
      // Adiciona o caractere inserido ao array da senha
      enteredPassword[index++] = key;
      lcd.print(key);  // Mostra o caractere inserido para o usuário
      lcd2.print(key);
    }
  }
  enteredPassword[index] = '\0';  // Adiciona o caractere nulo de terminação de string
  return atoi(enteredPassword);   // Converte a senha de string para int
}
String senhaCadastro( char key){
  String senha = String(key);
  lcd.print(key);
  lcd2.print(key);
while(true){
  char digito = keypad.getKey();

  if(digito)
  {lcd.print(digito);
  lcd2.print(digito);
    senha = senha + String(digito);
  }

  if(senha.length() == 5){
    break;
  }
}
  return senha;
}
void enviarParaApi(String nome, String email, String foto, int idBiometria, String tipo) {  //Função para enviar o acesso para a API
  DynamicJsonDocument envia(2048);

  envia["nome"] = nome;                //Cria o campo nome no arquivo json que será enviado
  envia["email"] = email;              //Cria o campo email no arquivo json que será enviado
  envia["idBiometria"] = idBiometria;  //Cria o campo idMatricula no arquivo json que será enviado
  envia["foto"] = foto;
  envia["tipo"] = tipo;
  String jsonStrings;                 //Cria uma variavel
  serializeJson(envia, jsonStrings);  //Atribui os valores criados anteriormente a variavel criada agora
  // lcd.println(jsonStrings);

  EthernetClient client;  //inicia o client, a api
  client.setTimeout(10000);
  if (!client.connect("api-labmaker-db7c20aa74d8.herokuapp.com", 80)) {  //Se não existir uma conexão exibe que a conexão falhou
    lcd.println(F("Connection failed"));
    
    return;
  }

  lcd.println(F("Connected!"));

  // Send HTTP request
  client.println(F("POST /addacessos HTTP/1.0"));                      //Prepara para enviar um POST pelo link /addacessos
  client.println(F("Host: api-labmaker-db7c20aa74d8.herokuapp.com"));  // Define o link do host
  client.println(F("Content-Type: application/json"));                 //Define o formato que será enviado como json
  client.print(F("Content-Length: "));
  client.println(jsonStrings.length());
  client.println(F("Connection: close"));
  client.println();

  client.println(jsonStrings);  //Envia os dados para a API

  if (client.println() == 0) {  // Verifica se a requisição foi feita com sucesso
    lcd.println(F("Failed to send request"));
    
    client.stop();
    return;
  }

  char status[32] = { 0 };
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {  // Verifica se os dados foram enviados
    lcd.print(F("Unexpected response: "));
    lcd.println(status);
    client.stop();
    return;
  } else {  //Se forem enviados entra nesse else
    lcd.println("Adicionado com sucesso");
    
  }

  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    lcd.println(F("Invalid response"));
    client.stop();
    return;
  }

  client.stop();
}

bool consultarApi(int codigoDigitado) {  // função que vai fazer o tratamento dos dados da API, recebe como parametro o ID que o usuário digitou
  int index = 0;
  while (true) {
    EthernetClient client;
    client.setTimeout(10000);
    if (!client.connect("api-labmaker-db7c20aa74d8.herokuapp.com", 80)) {
      lcd.println(F("Connection failed"));
      return false;
    }

    lcd.println(F("Connected!"));

    // Send HTTP request
    client.println(F("GET / HTTP/1.0"));
    client.println(F("Host: api-labmaker-db7c20aa74d8.herokuapp.com"));

    client.println(F("Connection: close"));
    if (client.println() == 0) {
      lcd.println(F("Failed to send request"));
      client.stop();
      return false;
    }

    // Check HTTP status
    char status[32] = { 0 };
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      lcd.print(F("Unexpected response: "));
      lcd.println(status);
      client.stop();
      return false;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      lcd.println(F("Invalid response"));
      client.stop();
      return false;
    }

    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    const size_t capacity = 2048;
    DynamicJsonDocument doc(capacity);

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
      lcd.print(F("deserializeJson() failed: "));
      lcd.println(error.f_str());
      client.stop();
      return false;
    }
    JsonObject Usuario = doc["usuarios"][index].as<JsonObject>();
    // Extract values
    lcd.println(F("Response:"));
    String email = Usuario["email"].as<String>();
    int id = Usuario["idBiometria"].as<int>();

    if (id == codigoDigitado) {
      return true;
    }
    index++;
  }
  return false;

  client.stop();
}

String* consultarApiAcesso(int idBiometria) {  // função que vai fazer o tratamento dos dados da API, recebe como parametro o ID que o usuário digitou

  String* Nome_Email = new String[3];
  int index = 0;
  while (true) {
    EthernetClient client;
    
    client.setTimeout(10000);
    if (!client.connect("api-labmaker-db7c20aa74d8.herokuapp.com", 80)) {
      lcd.println(F("Connection failed"));
      Nome_Email[0] = "Erro";
    }

    lcd.println(F("Connected!"));

    // Send HTTP request https://<your-deployment-host>
    client.println(F("GET / HTTP/1.0"));
    client.println(F("Host: api-labmaker-db7c20aa74d8.herokuapp.com"));
    
    client.println(F("Connection: close"));
    if (client.println() == 0) {
      lcd.println(F("Failed to send request"));
      client.stop();
      Nome_Email[0] = "Erro";
    }

    // Check HTTP status
    char status[32] = { 0 };
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      lcd.print(F("Unexpected resdsponse: "));
      lcd.println(status);
      client.stop();
      Nome_Email[0] = "Erro";
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      lcd.println(F("Invalid response"));
      client.stop();
      Nome_Email[0] = "Erro";
    }

    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    const size_t capacity = 2048;
    DynamicJsonDocument doc(capacity);

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
      lcd.print(F("deserializeJson() failed: "));
      lcd.println(error.f_str());
      client.stop();
      Nome_Email[0] = "Erro";
    }
    JsonObject Usuario = doc["usuarios"][index].as<JsonObject>();
    // Extract values
    lcd.println(F("Response:"));
    String email = Usuario["email"].as<String>();
    String nome = Usuario["nome"].as<String>();
    String foto = Usuario["foto"].as<String>();
    int id = Usuario["idBiometria"].as<int>();

    if (id == idBiometria) {
      Nome_Email[0] = nome;
      Nome_Email[1] = email;
      Nome_Email[2] = foto;
      return Nome_Email;
      break;
    }
    index++;
  }
  client.stop();
}
uint8_t getFingerprintID() {      //Função que verifica se o dedo que o usuário colocou sobre o leitor existe na sua memória
  uint8_t p = finger.getImage();  //pega a imagem da biometria
  switch (p) {
    case FINGERPRINT_OK:  //Se o dedo inserido existir dentro da memoria acessa esse caso

      break;
    case FINGERPRINT_NOFINGER:  //Se o leitor não detectar nenhum dedo acessa esse caso

      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_IMAGEFAIL:  //Se leitor não conseguir identificar a imagem acessa esse caso

      return p;
    default:

      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:

      break;
    case FINGERPRINT_IMAGEMESS:

      return p;
    case FINGERPRINT_PACKETRECIEVEERR:

      return p;
    case FINGERPRINT_FEATUREFAIL:

      return p;
    case FINGERPRINT_INVALIDIMAGE:

      return p;
    default:

      return p;
  }

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {

    return p;
  } else {

    return p;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    // tone(buzzer, 4500);

    lcd.setCursor(0, 0);
    for (int i = 0; i < 6; i++) {
      digitalWrite(ledVermelho, LOW);
      delay(50);
      digitalWrite(ledVermelho, HIGH);
      delay(50);
      digitalWrite(ledVermelho, LOW);
    }
    lcd.print("Acesso negado! A");
    lcd.setCursor(0, 1);
    lcd.print("digital inexiste");
    delay(50);
    digitalWrite(ledVermelho, HIGH);
    delay(1500);
    // noTone(buzzer);
    aguardando();
    return -1;
  }


  String* dados = consultarApiAcesso(finger.fingerID);
  if (dados[0] != "Erro") {
    enviarParaApi(dados[0], dados[1], dados[2], finger.fingerID, "Saiu");
    
  }
  
  // tone(buzzer, 2500);  //Ligando o buzzer com uma frequência de 1500 Hz.
  lcd.clear();
  digitalWrite(rele, HIGH);
  
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVerde, HIGH);
  
  lcd.setCursor(2, 0);
  lcd.println("Até mais");
  lcd.setCursor(0, 1);
  lcd.println(dados[0]);
  // lcd.print(nome);
  delay(1500);
  digitalWrite(ledVerde, LOW);
  // noTone(buzzer);
  delay(1500);
  digitalWrite(rele, LOW);
  
  aguardando();
  aguardando2();
  // lcd.clear();
  // lcd.print("Liberar Acesso?");
  // lcd.setCursor(1,1);
  // lcd.print("Insira o Dedo!");

  return finger.fingerID;
}

uint8_t getFingerprintID2() {      //Função que verifica se o dedo que o usuário colocou sobre o leitor existe na sua memória
  uint8_t p = finger2.getImage();  //pega a imagem da biometria
  switch (p) {
    case FINGERPRINT_OK:  //Se o dedo inserido existir dentro da memoria acessa esse caso

      break;
    case FINGERPRINT_NOFINGER:  //Se o leitor não detectar nenhum dedo acessa esse caso

      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_IMAGEFAIL:  //Se leitor não conseguir identificar a imagem acessa esse caso

      return p;
    default:

      return p;
  }

  p = finger2.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:

      break;
    case FINGERPRINT_IMAGEMESS:

      return p;
    case FINGERPRINT_PACKETRECIEVEERR:

      return p;
    case FINGERPRINT_FEATUREFAIL:

      return p;
    case FINGERPRINT_INVALIDIMAGE:

      return p;
    default:

      return p;
  }

  p = finger2.fingerFastSearch();
  if (p == FINGERPRINT_OK) {

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {

    return p;
  } else {

    return p;
  }
}

int getFingerprintIDez2() {
  uint8_t p = finger2.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger2.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger2.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    // tone(buzzer, 4500);

    lcd2.setCursor(0, 0);
    for (int i = 0; i < 6; i++) {
      digitalWrite(ledVermelho, LOW);
      delay(50);
      digitalWrite(ledVermelho, HIGH);
      delay(50);
      digitalWrite(ledVermelho, LOW);
    }
    lcd2.print("Acesso negado! A");
    lcd2.setCursor(0, 1);
    lcd2.print("digital inexiste");
    delay(50);
    digitalWrite(ledVermelho, HIGH);
    delay(1500);
    // noTone(buzzer);
    aguardando2();
    return -1;
  }


  String* dados = consultarApiAcesso(finger.fingerID);
  if (dados[0] != "Erro") {
    enviarParaApi(dados[0], dados[1], dados[2], finger.fingerID, "Saiu");
    
  }
  
  // tone(buzzer, 2500);  //Ligando o buzzer com uma frequência de 1500 Hz.
  lcd2.clear();
  digitalWrite(rele, HIGH);
  
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVerde, HIGH);
  
  lcd2.setCursor(2, 0);
  lcd2.println("Até mais");
  lcd2.setCursor(0, 1);
  lcd2.println(dados[0]);
  // lcd.print(nome);
  delay(1500);
  digitalWrite(ledVerde, LOW);
  // noTone(buzzer);
  delay(1500);
  digitalWrite(rele, LOW);
  
  aguardando2();
  // lcd.clear();
  // lcd.print("Liberar Acesso?");
  // lcd.setCursor(1,1);
  // lcd.print("Insira o Dedo!");

  return finger2.fingerID;
}
uint8_t getFingerprintEnroll(int id) {  // Função para cadastrar biometria, recebe como parâmetro o id digitado

  int p = -1;
  lcd.clear();
  lcd.print("Esperando dedo");
  lcd.setCursor(0, 1);
  lcd.print("para gravacao");
  lcd.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        digitalWrite(ledVerde, HIGH);
        lcd.println("Imagem Capturada");
        digitalWrite(ledVerde, LOW);
        break;
      case FINGERPRINT_NOFINGER:
        digitalWrite(ledAmarelo, HIGH);
        lcd.println(".");
        digitalWrite(ledAmarelo, LOW);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.println("Erro de Comunicacao");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.println("Erro de Imagem");
        break;
      default:
        lcd.println("Erro Desconhecido");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.println("Imagem Convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.println("Imagen confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.println("Erro de Comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    default:
      lcd.println("Erro Desconhecido");
      return p;
  }
  lcd.clear();
  lcd.println("Remova o Dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  lcd.print("ID ");
  lcd.println(id);
  p = -1;
  digitalWrite(ledAmarelo, HIGH);
  lcd.clear();
  lcd.println("Coloque o dedo ");
  lcd.setCursor(0, 1);
  lcd.println("novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        digitalWrite(ledAmarelo, LOW);
        lcd.clear();
        lcd.println("Imagem Capturada");
        break;
      case FINGERPRINT_NOFINGER:
        lcd.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd.println("Erro de Comunicacao");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd.println("Erro de Imagem");
        break;
      default:
        lcd.println("Erro Desconhecido");
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.println("Imagem confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.println("Erro de Comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    default:
      lcd.println("Unknown error");
      return p;
  }

  // OK converted!
  lcd.clear();
  lcd.print("Cadastrando ID:");
  lcd.setCursor(0, 1);
  lcd.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.println("Impressoes combinadas!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.println("Erro de comunicacao");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.println("Biometrias nao conferem");
    return p;
  } else {
    lcd.println("Erro Desconhecido");
    return p;
  }

  lcd.print("ID ");
  lcd.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.println("Erro de Comunicacao");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.println("Não pode armazenar neste local");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.println("Erro de Escrita em Flash");
    return p;
  } else {
    lcd.println("Erro Desconhecido");
    return p;
  }
}
uint8_t getFingerprintEnroll2(int id) {  // Função para cadastrar biometria, recebe como parâmetro o id digitado

  int p = -1;
  lcd2.clear();
  lcd2.print("Esperando dedo");
  lcd2.setCursor(0, 1);
  lcd2.print("para gravacao");
  lcd2.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger2.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        digitalWrite(ledVerde, HIGH);
        lcd2.println("Imagem Capturada");
        digitalWrite(ledVerde, LOW);
        break;
      case FINGERPRINT_NOFINGER:
        digitalWrite(ledAmarelo, HIGH);
        lcd2.println(".");
        digitalWrite(ledAmarelo, LOW);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd2.println("Erro de Comunicacao");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd2.println("Erro de Imagem");
        break;
      default:
        lcd2.println("Erro Desconhecido");
        break;
    }
  }

  // OK success!

  p = finger2.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcd2.println("Imagem Convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd2.println("Imagen confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd2.println("Erro de Comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd2.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd2.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    default:
      lcd2.println("Erro Desconhecido");
      return p;
  }
  lcd2.clear();
  lcd2.println("Remova o Dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger2.getImage();
  }
  lcd2.print("ID ");
  lcd2.println(id);
  p = -1;
  digitalWrite(ledAmarelo, HIGH);
  lcd2.clear();
  lcd2.println("Coloque o dedo ");
  lcd2.setCursor(0, 1);
  lcd2.println("novamente");
  while (p != FINGERPRINT_OK) {
    p = finger2.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        digitalWrite(ledAmarelo, LOW);
        lcd2.clear();
        lcd2.println("Imagem Capturada");
        break;
      case FINGERPRINT_NOFINGER:
        lcd2.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        lcd2.println("Erro de Comunicacao");
        break;
      case FINGERPRINT_IMAGEFAIL:
        lcd2.println("Erro de Imagem");
        break;
      default:
        lcd2.println("Erro Desconhecido");
        break;
    }
  }

  // OK success!

  p = finger2.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      lcd2.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd2.println("Imagem confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd2.println("Erro de Comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd2.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd2.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    default:
      lcd2.println("Unknown error");
      return p;
  }

  // OK converted!
  lcd2.clear();
  lcd2.print("Cadastrando ID:");
  lcd2.setCursor(0, 1);
  lcd2.println(id);

  p = finger2.createModel();
  if (p == FINGERPRINT_OK) {
    lcd2.println("Impressoes combinadas!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd2.println("Erro de comunicacao");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd2.println("Biometrias nao conferem");
    return p;
  } else {
    lcd2.println("Erro Desconhecido");
    return p;
  }

  lcd2.print("ID ");
  lcd2.println(id);
  p = finger2.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd2.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd2.println("Erro de Comunicacao");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd2.println("Não pode armazenar neste local");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd2.println("Erro de Escrita em Flash");
    return p;
  } else {
    lcd2.println("Erro Desconhecido");
    return p;
  }
}