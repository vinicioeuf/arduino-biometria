#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
unsigned long startTime = 0;
LiquidCrystal_I2C lcd(0x27,16, 2);
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
};

#define Password_Length 8
int signalPin = 7;
char Data[Password_Length]; 
char Master[Password_Length] = "1234567"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int getFingerprintIDez();
#if (defined(_AVR) || defined(ESP8266)) && !defined(__AVR_ATmega2560_)
SoftwareSerial mySerial(14, 15);
#else

#define mySerial Serial3

#endif

int pinLed = 12;
int rele = 9;
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
String nome = "Vini";
char cadstrar = '#';
void setup()  
{  
  pinMode(signalPin, OUTPUT);
  Serial.begin(9600);
  lcd.init();                    
  lcd.backlight();
  aguardando();
  pinMode(12, OUTPUT);
  pinMode(rele, OUTPUT);
  
  pinMode(10, OUTPUT);
  finger.begin(57600);
  if(data_count == Password_Length-1){
    lcd.clear();
    Serial.print("Informe seu ID:");
    if (finger.storeModel(id) == FINGERPRINT_BADLOCATION) {
      lcd.clear();
      lcd.setCursor(0, 0);
      Serial.print("Digital inválida");
      lcd.setCursor(0, 1);
      Serial.print("Tente novamente");
      return;
    }
    lcd.clear();
    Serial.print("Gravando ID: " + id);    
    while (!  getFingerprintEnroll() );;
  }
}

void aguardando(){
  lcd.createChar(0, SIMB1);
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
  Serial.print("Aguardando o"); // Imprime a mensagem fixa
  lcd.setCursor(3,1);// seta o texto a seguir a partir do oitavo índice do lcd
  Serial.print("Acesso ao Lab");// Aqui fica impressa a data (como não estou com o módulo de relógio estou imprimindo este texto para simular)
}

void loop()                   
{
  char key = keypad.getKey();
  if(key){
    Serial.println(key);
    if(key == '#'){
      // Verifica se a senha inserida é correta
      Serial.println(checkPassword());
      if(checkPassword()) {
        // Chama a função para registrar a impressão digital
        getFingerprintEnroll();
      } else {
        Serial.println("Senha incorreta");
      }
    }
  }
  
  delay(50);
  getFingerprintIDez();
  digitalWrite(12, HIGH);
  delay(50);
}

bool checkPassword() {
  char enteredPassword[Password_Length];
  uint8_t index = 0;
  
  // Aguarda o usuário inserir a senha
  while(index < Password_Length - 1) {
    char key = keypad.getKey();
    if(key) {
      if(key == '*') {
        // Usuário cancelou a entrada da senha
        return false;
      } else if (key == '#') {
        // Usuário finalizou a entrada da senha
        getFingerprintEnroll();
        // enteredPassword[index] = '\0'; // Adiciona o caractere nulo para indicar o fim da string
        break;
      } else {
        // Adiciona o caractere inserido ao array da senha
        enteredPassword[index++] = key;
        Serial.print(key); // Mostra o caractere inserido para o usuário
      }
    }
  }
  
  // Compara a senha inserida com a senha mestra
  if(strcmp(enteredPassword, Master) == 0) {
    return true; // Senha correta
  } else {
    return false; // Senha incorreta
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      
      break;
    case FINGERPRINT_NOFINGER:
     
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        return p;
    case FINGERPRINT_IMAGEFAIL:
      
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
  if (p != FINGERPRINT_OK){
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
  lcd.clear();
  digitalWrite(rele, HIGH);
  digitalWrite(12, LOW);
  tone(10, 2500);//Ligando o buzzer com uma frequência de 1500 Hz.
  lcd.setCursor(2,0);
  Serial.print("Bem vindo(a)");
  lcd.setCursor(0,1);
  Serial.print(nome);
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

uint8_t getFingerprintEnroll() {

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