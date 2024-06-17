#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <DHT.h>

// Define o servo motor
Servo servoMotor;

// Define as especificações do DHT
#define DHTPIN 3
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// Define as especificações do LCD
#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

// Variáveis de entrada
const int servoPin = A1;
const int potPin = A0;

const int buzzerBat = 4;
const int buzzerTemp = 5;

// Bytes / Símbolos
byte tempByte[8] = { B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110 };
byte heartByte[8] = { B00000, B01010, B11111, B11111, B11111, B01110, B00100, B00000 };
byte aguaByte[8] = { B00100, B00100, B01010, B01010, B10001, B10111, B11111, B01110 };

// Variaveis de nivel
String nivelBat = "";
String nivelTemp = "";
float nivelAgua = 100.0; // Supondo 100% de hidratação no início

void setup() {
  lcd.init();
  lcd.backlight();

  dht.begin();

  servoMotor.attach(servoPin);

  pinMode(buzzerBat, OUTPUT);
  pinMode(buzzerTemp, OUTPUT);

  // Criar os caracteres personalizados
  lcd.createChar(0, tempByte);
  lcd.createChar(1, heartByte);
  lcd.createChar(2, aguaByte);

  // Mostrar mensagem de boas-vindas
  lcd.setCursor(0, 0);
  lcd.print("Seja bem-vindo ao");
  lcd.setCursor(0, 1);
  lcd.print("controle de pilotos");
  lcd.setCursor(0, 2);
  lcd.print("da Formula E");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Este projeto simula");
  lcd.setCursor(0, 1);
  lcd.print("a captacao de dados");
  lcd.setCursor(0, 2);
  lcd.print("dos pilotos durante");
  lcd.setCursor(0, 3);
  lcd.print("uma corrida.");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Use o potenciometro");
  lcd.setCursor(0, 1);
  lcd.print("para simular o pulso");
  lcd.setCursor(0, 2);
  lcd.print("e o DHT para simular");
  lcd.setCursor(0, 3);
  lcd.print("temperatura corporal");
  delay(3000);
  lcd.clear();

  Serial.begin(9600);
}

void loop() {
  int potValue = analogRead(potPin);
  int batimentoCard = map(potValue, 0, 1023, 60, 200);
  int batimentointervalo = map(batimentoCard, 60, 180, 1000, 333);
  float temp = dht.readTemperature();

  // Calcular a perda de peso
  float perdaPeso = 0.0;
  if (nivelAgua > 0) {
    if (temp >= 36.0) {
      perdaPeso = (temp - 35.0) * 0.05; // Perda de peso aumenta com a temperatura
      nivelAgua -= perdaPeso; // Reduzir o nível de água no corpo
    } else {
      perdaPeso = 0.0;
    }
  }

  if (nivelAgua < 0) nivelAgua = 0; // Não permitir valores negativos

  // Mostra os batimentos no LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.print(" ");
  lcd.print(batimentoCard);
  lcd.print(" BPM");

  lcd.setCursor(12, 0);
  lcd.print(nivelBat);

  // Mostra a temperatura no LCD
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.print(" ");
  lcd.print(temp);
  lcd.print(" C");

  lcd.setCursor(12, 1);
  lcd.print(nivelTemp);

  // Mostra o nível de água corporal
  lcd.setCursor(0, 2);
  lcd.write(byte(2));
  lcd.print(" ");
  lcd.print(nivelAgua);
  lcd.print(" %");

  // Mostra a perda de peso no LCD
  lcd.setCursor(0, 3);
  lcd.print("Peso: -");
  lcd.print(perdaPeso);
  lcd.print(" kg/h");

  // Mostra a temperatura no terminal
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.print("Nível: ");
  Serial.println(nivelTemp);

  // Mostra os batimentos no terminal
  Serial.print("Batimento Cardiaco: ");
  Serial.print(batimentoCard);
  Serial.println(" BPM");
  Serial.print("Nível: ");
  Serial.println(nivelBat);

  // Mostra a perda de peso no terminal
  Serial.print("Perda de Peso: ");
  Serial.print(perdaPeso);
  Serial.println(" kg/h");
  Serial.print("Nível de Água: ");
  Serial.print(nivelAgua);
  Serial.println(" %");

  // Lógica para LEDs e buzina para temperatura
  if (temp < 36.0) {
    digitalWrite(buzzerTemp, LOW);
    nivelTemp = "Normal";
  } else if (temp >= 36.0 && temp < 38.5) {
    digitalWrite(buzzerTemp, LOW);
    nivelTemp = "Elevado";
  } else {
    digitalWrite(buzzerTemp, HIGH);
    nivelTemp = "Critico";
  }

  // Lógica para LEDs e buzina para batimentos cardíacos
  if (batimentoCard < 150) {
    digitalWrite(buzzerBat, LOW);
    nivelBat = "Normal";
  } else if (batimentoCard >= 150 && batimentoCard < 190) {
    digitalWrite(buzzerBat, LOW);
    nivelBat = "Elevado";
  } else {
    digitalWrite(buzzerBat, HIGH);
    nivelBat = "Critico";
  }

  // Separa a atualização do Terminal
  Serial.println("-----------------------");

  // Controlar o servo motor para bater na mesma frequência que o batimento cardíaco
  servoMotor.write(90); 
  delay(batimentointervalo / 2); 
  servoMotor.write(70); 
  delay(batimentointervalo / 2); 
  servoMotor.write(110); 
  delay(batimentointervalo / 2);

  delay(1000);
}
