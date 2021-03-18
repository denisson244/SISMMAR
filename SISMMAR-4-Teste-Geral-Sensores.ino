/*****************************************************************
*Configuração e teste geral dos sensores
*DTH11, LDR, Humidade de solo.
*Denisson Silva - 08 de março de 2021
******************************************************************/

//Bibliotecas do DHT11
#include <DHT.h>
#include <DHT_U.h>

//Bibliotecas LCD I2C
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Definicoes do DHT11
#define DHTPIN 5 //pino de saida de dados
#define DHTTYPE DHT11 //tipo do sensor

//Definicoes Sensor de humidade deSolo
#define SENSOR_SOLO_PIN 0 //Entrada do sensor de umidade de solo
#define SUS_VCC 7 //Sensor Umidade Solo VCC
#define SUS_GND 6 //Sensor Umidade Solo GND

//Inicializacao DHT11
DHT dht(DHTPIN, DHTTYPE);

//Inicializacao LCD I2C
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

//Variaveis DHT11
int tempDHT;
int umdDHT;

//Variaveis LDR
int ldrPin = A7; //LDR no pino analogico A7
int ldrValor = 0;

//Variaveis SOLO
int umdSolo;

void setup() {
    Serial.begin(9600); //Inicia o monitor serial
    Serial.println("***** Teste Geral Sensores *****");
    //Setup LDR
    pinMode(ldrPin, INPUT);

    //Setup SOLO
    pinMode(SUS_VCC, OUTPUT);
    pinMode(SUS_GND, OUTPUT);

    //Setup DHT11
    dht.begin();

    //Setup LCD I2C
    lcd.begin (16,4);
}

void loop() {
    delay(2000);
    lerSensores();
    printSerial();
    printLCD();
}

//************FUNCOES****************//

void lerSensores(void){
    tempDHT = dht.readTemperature();
    umdDHT = dht.readHumidity();
    ldrValor = getLumen(ldrPin);
    umdSolo = getUmdSolo();
}

int getLumen(int ldrPino){
    int ldrValor = 0;
    for(int i = 0; i < 10; i++){ //Le 10 medicoes do LDR e as soma.
        ldrValor = ldrValor + analogRead(ldrPino);
        delay(100);
    }
    ldrValor = ldrValor / 10; //Calculo da média aritmética das medicoes.
    ldrValor = map(ldrValor, 1023, 0, 100, 0); //Escuro:0%, Claro:100%.
    delay(1000);
    return ldrValor;
}

int getUmdSolo(){
    int umdValor;
    digitalWrite(SUS_VCC, LOW); //Direciona a corrente eletrica.
    digitalWrite(SUS_GND, HIGH); 
    delay(1000); //Aguarda 1s para o direcionamento ter efeito.
    umdValor = analogRead(SENSOR_SOLO_PIN);
    
    digitalWrite(SUS_VCC, HIGH); //Inverte a direcao da corrente eletrica.
    digitalWrite(SUS_GND, LOW); 
    delay(1000); //Aguarda 1s para a inversao ter efeito.
    digitalWrite(SUS_VCC, LOW); //Para a corrente eletrica.

    umdValor = map(umdValor, 1023, 0, 100, 0);
    return umdValor;
}

void printSerial(){
    Serial.print("Temp DHT => ");
    Serial.print(tempDHT);
    Serial.print("oC  Umd-AR => ");
    Serial.print(umdDHT);
    Serial.print("%  Luminosidade => ");
    Serial.print(ldrValor);
    Serial.print("%  Umd-Solo => ");
    Serial.print(umdSolo);
    Serial.println("%");
}

void printLCD(){
    lcd.setBacklight(HIGH);
    lcd.setCursor(5,0);
    lcd.print("SISMMAR");
    lcd.setCursor(0,1);
    lcd.print("Temp ");
    lcd.print(tempDHT);
    lcd.setCursor(7,1);
    lcd.print("C Umd ");
    lcd.print(umdDHT);
    lcd.print("%");
    lcd.setCursor(0,2);
    lcd.print("Solo ");
    lcd.print(umdSolo);
    lcd.setCursor(7,2);
    lcd.print("% Luz ");
    lcd.print(ldrValor);
    lcd.print("%");
}
