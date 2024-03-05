#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Definição do endereço MAC do módulo Ethernet
IPAddress ip(192, 168, 2, 182); // IP do Servidor local do módulo Ethernet

EthernetClient client; // Definição do objeto client (requisição a API)
HttpClient http(client); // Definição do objeto http (forma como o programa fará a execução)

unsigned long startTime = 0; //
LiquidCrystal_I2C lcd(0x27,16, 2); // Padrão do módulo LCD 16 caracteres em linha, contendo duas colunas
byte SIMB1[8] = { B11111, 
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111
                 };
byte SIMB2[8] = { B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111,
                  B11111
                 };
byte SIMB3[8] = { B11111,
                  B11111,
                  B11111,
                  B11110,
                  B11110,
                  B11111,
                  B11111,
                  B11111
                 };
byte SIMB4[8] = { B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110,
                  B11110
                 };
byte SIMB5[8] = { B11100,
                  B11100,
                  B11100,
                  B00000,
                  B00000,
                  B11100,
                  B11100,
                  B11100
};// Letras personalizadas no leitor LCD, cada um desses bytes imprime em um dos caracteres do LCD 

#define Password_Length 8 
int signalPin = 7;
char Data[Password_Length]; 
char Master[Password_Length] = "1234567"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4; //Define a quantidade de linhas do teclado (4)
const byte COLS = 3; //Define a quantidade de colunas do teclado (3)
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};// Padrão do teclado que estamos usando
byte rowPins[ROWS] = {32, 33, 34, 35}; // Pinos de conexão referente a cada linha
byte colPins[COLS] = {36, 37, 38}; // Pinos de conexão referente a cada coluna

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); // Com as informações anteriores do teclado, aqui instanciamos o teclado

int getFingerprintIDez();
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(14, 15);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial3

#endif

int pinLed = 12;
int rele = 9;
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); // Inicia o objeto finger, para começar a usar o leitor biométrico
String nome = "Vini";
char cadstrar = '#';
String senhaDigitada;


void setup()  
{  
  while (!Serial) continue;

  // Initialize Ethernet library
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //Definição do endereço MAC do módulo Ethernet
  if (!Ethernet.begin(mac)) { //Verifica se o endereço MAC condiz com o do proprio módulo ethernet
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  delay(1000); // Aguarda 1 segundo

  Serial.println(F("Conectando...")); // Imprime Conectando...

  pinMode(signalPin, OUTPUT); 
  Serial.begin(9600); //Inicia o serial na frequencia de 9600
  lcd.init();  // Inicia o módulo lcd             
  lcd.backlight(); // Inicia a luz de fundo do módulo lcd
  aguardando(); //Chama função aguardando que inicia o objeto para imprimir as letras personalizadas
  pinMode(12, OUTPUT);
  pinMode(rele, OUTPUT);
  
  pinMode(10, OUTPUT);
  finger.begin(57600); // Inicia o módulo do leitor biométrico na frequencia 57600
  if(data_count == Password_Length-1){ 
    lcd.clear(); // Limpa o que estiver escrito no modulo lcd
    Serial.print("Informe seu ID:"); // Pede ao usuário para digitar o ID da biometria
    if (finger.storeModel(id) == FINGERPRINT_BADLOCATION) { // Verifica se a digital é válida ou não
      lcd.clear(); // Limpa o que estiver escrito no modulo lcd
      lcd.setCursor(0, 0); // Define o que será mostrado no lcd na a partir da linha 0, coluna 0
      Serial.print("Digital inválida");
      lcd.setCursor(0, 1); // Define o que será mostrado no lcd na a partir da linha 0, coluna 1
      Serial.print("Tente novamente");
      return;
    }
    lcd.clear();
    Serial.print("Gravando ID: " + id);  // Mostra ao usuário que a digital será gravada em determinado ID  
    while (!  getFingerprintEnroll(0) ); 
  }
}

void aguardando(){ // Função que instancia as letras personalizadas no módulo LCD
  lcd.createChar(0, SIMB1); //Instanciando os pixels
  lcd.createChar(1, SIMB2);
  lcd.createChar(2, SIMB3);
  lcd.createChar(3, SIMB4);
  lcd.createChar(4, SIMB5);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write((uint8_t)0); // Imprime a letra "I"
  lcd.setCursor(0, 1);
  lcd.write((uint8_t)1); // Imprime a letra "F"
  lcd.setCursor(1, 0);
  lcd.write((uint8_t)2);
  lcd.setCursor(1, 1);
  lcd.write((uint8_t)3);
  lcd.setCursor(2, 0);
  lcd.write((uint8_t)4);
  
  lcd.setCursor(3, 0);
  Serial.print("Aguardando o "); // Imprime a mensagem fixa
  lcd.setCursor(3,1);// seta o texto a seguir a partir do oitavo índice do lcd
  Serial.print("Acesso ao Lab ");// Aqui fica impressa a data (como não estou com o módulo de relógio estou imprimindo este texto para simular)
}

void loop() {
  getFingerprintIDez();
  char key = keypad.getKey(); //Aqui pegamos a tecla digitada no teclado
  
  if (key) {
    Serial.println(key); // Mostramos a tecla digitada
    if (key == '#') { // Caso o usuário pressione a tecla # (neste caso o # está atuando como o Enter)
      int codigoDigitado = checkPassword(); // Armazena as informações do id em uma variavel
      Serial.println(codigoDigitado); // Exibe a variavel com o id
      bool autorizado = consultarApi(codigoDigitado); // Verifica na API se o id digitado corresponde ao de algum usuário dentro da APi
      if (autorizado) { // Se o id digitado corresponder, abre o modo de cadastramento para a biometria
        getFingerprintEnroll(codigoDigitado);
        // enviarParaApi("Vinicio", "vinicio.eufrazio@aluno.ifsertao-pe.edu.br", codigoDigitado);
      }else {// caso contrário não permite cadastrar
        Serial.println("ID INVÁLIDO");
      }

    }
  }
}

void enviarParaApi(String nome, String email, String foto, String tipo, int idBiometria) { //Função para enviar o acesso para a API
  DynamicJsonDocument envia(2048);

  envia["nome"] = nome;//Cria o campo nome no arquivo json que será enviado
  envia["email"] = email;//Cria o campo email no arquivo json que será enviado
  envia["idBiometria"] = idBiometria;//Cria o campo idMatricula no arquivo json que será enviado
  envia["foto"] = foto;
  envia["tipo"] = tipo;
  String jsonStrings; //Cria uma variavel
  serializeJson(envia, jsonStrings); //Atribui os valores criados anteriormente a variavel criada agora
  // Serial.println(jsonStrings);
  
  EthernetClient client; //inicia o client, a api
  client.setTimeout(10000);
  if (!client.connect("api-labmaker-db7c20aa74d8.herokuapp.com", 80)) {//Se não existir uma conexão exibe que a conexão falhou
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));

  // Send HTTP request
  client.println(F("POST /addacessos HTTP/1.0")); //Prepara para enviar um POST pelo link /addacessos
  client.println(F("Host: api-labmaker-db7c20aa74d8.herokuapp.com")); // Define o link do host
  client.println(F("Content-Type: application/json")); //Define o formato que será enviado como json
  client.print(F("Content-Length: "));
  client.println(jsonStrings.length());
  client.println(F("Connection: close"));
  client.println();

  client.println(jsonStrings); //Envia os dados para a API

  if (client.println() == 0) { // Verifica se a requisição foi feita com sucesso
    Serial.println(F("Failed to send request"));
    client.stop();
    return;
  }

  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) { // Verifica se os dados foram enviados 
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    client.stop();
    return;
  }else{//Se forem enviados entra nesse else
    Serial.println("Adicionado com sucesso");
  }

  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    client.stop();
    return;
  }

  client.stop();
}


bool consultarApi(int codigoDigitado) {// função que vai fazer o tratamento dos dados da API, recebe como parametro o ID que o usuário digitou
    int index = 0;
    while (true) {
        EthernetClient client;
        client.setTimeout(10000);
        if (!client.connect("api-labmaker-db7c20aa74d8.herokuapp.com", 80)) {
            Serial.println(F("Connection failed"));
            return false;
        }

        Serial.println(F("Connected!"));

        // Send HTTP request
        client.println(F("GET / HTTP/1.0"));
        client.println(F("Host: api-labmaker-db7c20aa74d8.herokuapp.com"));
        
        client.println(F("Connection: close"));
        if (client.println() == 0) {
            Serial.println(F("Failed to send request"));
            client.stop();
            return false;
        }

        // Check HTTP status
        char status[32] = {0};
        client.readBytesUntil('\r', status, sizeof(status));
        if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
            Serial.print(F("Unexpected response: "));
            Serial.println(status);
            client.stop();
            return false;
        }

        // Skip HTTP headers
        char endOfHeaders[] = "\r\n\r\n";
        if (!client.find(endOfHeaders)) {
            Serial.println(F("Invalid response"));
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
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            client.stop();
            return false;
        }
        JsonObject Usuario = doc["usuarios"][index].as<JsonObject>();
        // Extract values
        Serial.println(F("Response:"));
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


String* consultarApiAcesso(int idBiometria) {// função que vai fazer o tratamento dos dados da API, recebe como parametro o ID que o usuário digitou

    String* Nome_Email = new String[3];
    int index = 0;
    while (true) {
        EthernetClient client;
        client.setTimeout(10000);
        if (!client.connect("api-labmaker-db7c20aa74d8.herokuapp.com", 80)) {
            Serial.println(F("Connection failed"));
            Nome_Email[0]="Erro";
        }

        Serial.println(F("Connected!"));

        // Send HTTP request
        client.println(F("GET / HTTP/1.0"));
        client.println(F("Host: api-labmaker-db7c20aa74d8.herokuapp.com"));
        
        client.println(F("Connection: close"));
        if (client.println() == 0) {
            Serial.println(F("Failed to send request"));
            client.stop();
            Nome_Email[0]="Erro";
        }

        // Check HTTP status
        char status[32] = {0};
        client.readBytesUntil('\r', status, sizeof(status));
        if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
            Serial.print(F("Unexpected response: "));
            Serial.println(status);
            client.stop();
            Nome_Email[0]="Erro";
            
        }

        // Skip HTTP headers
        char endOfHeaders[] = "\r\n\r\n";
        if (!client.find(endOfHeaders)) {
            Serial.println(F("Invalid response"));
            client.stop();
            Nome_Email[0]="Erro";
            
        }

        // Allocate the JSON document
        // Use arduinojson.org/v6/assistant to compute the capacity.
        const size_t capacity = 2048;
        DynamicJsonDocument doc(capacity);

        // Parse JSON object
        DeserializationError error = deserializeJson(doc, client);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            client.stop();
            Nome_Email[0]="Erro";
            
        }
        JsonObject Usuario = doc["usuarios"][index].as<JsonObject>();
        // Extract values
        Serial.println(F("Response:"));
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

int checkPassword() {
    char enteredPassword[2]; // Aumentado para incluir o caractere nulo de terminação de string
    uint8_t index = 0;

    // Aguarda o usuário inserir a senha
    while (index < 2) {
        char key = keypad.getKey();
        if (key) {
            // Adiciona o caractere inserido ao array da senha
            enteredPassword[index++] = key;
            Serial.print(key); // Mostra o caractere inserido para o usuário
        }
    }
    enteredPassword[index] = '\0'; // Adiciona o caractere nulo de terminação de string
    return atoi(enteredPassword); // Converte a senha de string para int
}

uint8_t getFingerprintID() { //Função que verifica se o dedo que o usuário colocou sobre o leitor existe na sua memória
  uint8_t p = finger.getImage(); //pega a imagem da biometria
  switch (p) {
    case FINGERPRINT_OK: //Se o dedo inserido existir dentro da memoria acessa esse caso
      
      break;
    case FINGERPRINT_NOFINGER: //Se o leitor não detectar nenhum dedo acessa esse caso
     
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        return p;
    case FINGERPRINT_IMAGEFAIL: //Se leitor não conseguir identificar a imagem acessa esse caso
      
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
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    tone(10, 4500);
    lcd.setCursor(0,0);
    for(int i = 0; i < 6; i++){
      digitalWrite(12, LOW);
      delay(50);
      digitalWrite(12, HIGH);
      delay(50);
      digitalWrite(12, LOW);
    }
    Serial.print("Acesso negado! A");
    lcd.setCursor(0,1);
    Serial.print("digital inexiste");
    delay(50);
    digitalWrite(12, HIGH);
    delay(1500);
    noTone(10);
    aguardando();
    return -1;
  }  

 
  String* dados = consultarApiAcesso(finger.fingerID);
  if (dados[0] != "Erro"){
    enviarParaApi(dados[0], dados[1], dados[2], "Entrou", finger.fingerID);
  }
  lcd.clear();
  digitalWrite(rele, HIGH);
  digitalWrite(12, LOW);
  tone(10, 2500);//Ligando o buzzer com uma frequência de 1500 Hz.
  lcd.setCursor(2,0);
  Serial.println("Bem vindo(a)" + dados[0]);
  lcd.setCursor(0,1);
  // Serial.print(nome);
  delay(1500);
  noTone(10);
  delay(1500);
  digitalWrite(rele, LOW);
  digitalWrite(12, HIGH);
  
  aguardando();
  // lcd.clear();
  // Serial.print("Liberar Acesso?");
  // lcd.setCursor(1,1);
  // Serial.print("Insira o Dedo!");
 
   return finger.fingerID; 
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t getFingerprintEnroll(int id) { // Função para cadastrar biometria, recebe como parâmetro o id digitado
  
  int p = -1;
  Serial.print("Esperando dedo para gravacao"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      digitalWrite(12, LOW);
      Serial.println("Imagem Capturada");
      break;
    case FINGERPRINT_NOFINGER:
      digitalWrite(12, HIGH);
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de Comunicacao");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Imagem");
      break;
    default:
      Serial.println("Erro Desconhecido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem Convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de Comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    default:
      Serial.println("Erro Desconhecido");
      return p;
  }
  
  Serial.println("Remova o Dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  digitalWrite(12, HIGH);
  Serial.println("Coloque o mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      digitalWrite(12, LOW);
      Serial.println("Imagem Capturada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de Comunicacao");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Imagem");
      break;
    default:
      Serial.println("Erro Desconhecido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagem confusa");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de Comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar recursos de impressão digital");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Criando Modelo para #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Impressoes combinadas!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicacao");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Biometrias nao conferem");
    return p;
  } else {
    Serial.println("Erro Desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de Comunicacao");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não pode armazenar neste local");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro de Escrita em Flash");
    return p;
  } else {
    Serial.println("Erro Desconhecido");
    return p;
  }   
}