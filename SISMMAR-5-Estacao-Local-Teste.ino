/****************************************************************
 * Estação LOcal - Sensores e Atuadores
 *       Sesor DHT11 sensor (Pino D5) para ler temperatura e umidade do ar 
 *       LDR (Pino A7) usado como sensor de luminisidade => 0% (escuridão) a 100% (claridade)
 *       Sensor de Unidade de Sol em % (Pino A0). VCC conectado no Pino D7 e GND no D6   
 *       Atuadores: Bomba/LED Vermelho (Pino 10) e Lampada (Pino D8)
 *       2 botões para ativação local da bomba (Pino D11) e da Lampada (Pino D9)    
 *       I2C LCD 16x4 para monitoramento local (SDA => Pino A4 e SCL => Pino A5)   
 * Denisson Silva - 15 de março de 2021
 ****************************************************************/

//Biblioteca Sensor DHT
#include <DHT.h>

//Bibliotecas do LCD I2C
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Definição dos sensores
#define DHTPIN 5          //Pino de dados DHT conectado ao pino 5 do Arduino
#define DHTTYPE DHT11     //DHT 11 (se o seu sensor for o DHT 22 use #define DHTTYPE DHT22) 
#define LDR_PIN A7        //usado para entrada do sensor de luminosidade
#define UMD_SOLO_PIN A0   //usado para entrada do sensor de umidade do solo
#define SUS_VCC 7         //usado para Fonte de Alimentação(VCC)do sensor de umidade do solo
#define SUS_GND 6         //usado para terra (GND)do sensor de umidade do solo

//Atuadores: Botões e LEDs
#define BOMBA_ON 11   //push-button
#define BOMBA_PIN 10 
#define LAMP_ON 9     //push-button
#define LAMP_PIN 8 
#define LED_ALERTA 13 //LED de alerta

//Variáveis usadas pelos sensores
int tempDHT; 
int umdDHT;
int lumin;
int umdSolo;

//Variáveis usadas pelos Atuadores
boolean bombaStatus = 0;
boolean lampStatus = 0;

//Variáveis usadas como temporizadores
long intervaloTempo = 300; //Define o intervalo de tempo em segundos ( 5 min) para ler os sensores
long inicioTempo = 0;
long tempoDecorrido = 0;

//Inicializa o sensor DHT
DHT dht(DHTPIN, DHTTYPE); 
//Initializa LCD I2C
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); //Define o endereço do LCD para 0x27, display de 16x4
                                                       //Use scanner I2C para confirmar o endereço
void setup(){
    pinMode(BOMBA_PIN, OUTPUT);
    pinMode(LAMP_PIN, OUTPUT);
    pinMode(LED_ALERTA, OUTPUT);
    pinMode(BOMBA_ON, INPUT_PULLUP); //push-button
    pinMode(LAMP_ON, INPUT_PULLUP); //push-button
    pinMode(SUS_VCC,OUTPUT);
    pinMode(SUS_GND,OUTPUT);
    digitalWrite(BOMBA_PIN, HIGH);
    digitalWrite(LAMP_PIN, HIGH);
    Serial.begin(9600); 
    Serial.println("ArduFarmBot Local Station Test");
    dht.begin();
    lcd.begin(16,4);
    lerSensores(); //Leitura inicial
    inicioTempo = millis(); //Inicia o relógio do programa
}

void loop(){
    //Inicia contador de tempo para medições
    tempoDecorrido = millis() - inicioTempo; 
    lerCmdoLocal(); //Lê o status do botão local
    printDadosLCD();
    if (tempoDecorrido > (intervaloTempo*1000)){
        lerSensores();
        printDadosMonSerial();
        inicioTempo = millis();  
    }
}

/****************************************************************
* Lê os comandos locais (os botões Bomba e Lâmpada são normalmente "HIGH")
****************************************************************/
void lerCmdoLocal(){  
    int digiValor = debounce(BOMBA_ON);
    if (!digiValor){
        bombaStatus = !bombaStatus;
        printDadosLCD();
        aplicaCmdo();
    }
    digiValor = debounce(LAMP_ON);
    if (!digiValor){
        lampStatus = !lampStatus;
        printDadosLCD();
        aplicaCmdo();
    }
}

/***************************************************
* Recebe os comandos e ações dos atuadores
****************************************************/
void aplicaCmdo(){
    if (bombaStatus == 0) 
        digitalWrite(BOMBA_PIN, HIGH);
    if (bombaStatus == 1) 
        digitalWrite(BOMBA_PIN, LOW);
    if (lampStatus == 0) 
        digitalWrite(LAMP_PIN, HIGH);
    if (lampStatus == 1) 
        digitalWrite(LAMP_PIN, LOW);
}

/***************************************************
* Lê os dados captados pelos sensores
****************************************************/
void lerSensores(void){
    tempDHT = dht.readTemperature();   //Lê os valores de temperatura e umidade do sensor DHT
    umdDHT = dht.readHumidity();
    lumin = capLumin(LDR_PIN);
    umdSolo = capUmdSolo();
}

/***************************************************
* Captura dados de luminosidade, 0% escuro total a 100% luz total
****************************************************/
int capLumin(int analogPin){
    int analogValor = 0;
    for(int i = 0; i < 10; i++){ //Lê o sensor 10 vezes para obter a média aritmetica
        analogValor += analogRead(analogPin);   
        delay(50);
    }
    analogValor = analogValor/10; //Luz abaixo de 300: Escuro. Acima de 800: Claro.
    analogValor = map(analogValor, 1023, 0, 0, 100); //LDR escuro: 0% => claro: 100%
    return analogValor;  
}

/***************************************************
* Captura dados do sensor de umidade do solo
****************************************************/
int capUmdSolo(){
    int i = 0;
    int analogValor = 0;
    for(i = 0; i < 1; i++){ //Leia o sensor "i" vezes e obter a média aritmetica
        digitalWrite(LED_ALERTA, HIGH);
        digitalWrite(SUS_VCC,LOW);   //Conduz a corrente através do sensor em uma direção
        digitalWrite(SUS_GND,HIGH);
        delay(500);   //Aguarda meio segundo para que os efeitos de capacitância se acomodem
        analogValor += analogRead(UMD_SOLO_PIN);
        digitalWrite(LED_ALERTA, LOW);
        digitalWrite(SUS_VCC,HIGH);   //Reverte a corrente elétrica
        digitalWrite(SUS_GND,LOW);
        delay(500);   
        digitalWrite(SUS_VCC,LOW);   //Para a corrente elétrica
    }
    digitalWrite(SUS_VCC,LOW);   //Para a corrente elétrica
    analogValor = analogValor/(i);
    analogValor = map(analogValor, 1015, 3, 0, 100); //1015: 0 (no ar) => 003: 100% (colocando um curto-circuito)
    Serial.println (analogValor);
    return analogValor;
}

/***************************************************
* Mostra dados capturados no Monitor Serial
****************************************************/
void printDadosMonSerial(void){
    Serial.print("   Temp DHT ==> ");
    Serial.print(tempDHT);
    Serial.print("oC  Hum DHT ==> ");
    Serial.print(umdDHT);
    Serial.print("%  Luminosity ==> ");
    Serial.print(lumin);
    Serial.print("%  Soil Moisture ==> ");
    Serial.print(umdSolo);
    Serial.print("%  Pump: ");
    Serial.print(bombaStatus);
    Serial.print("    Lamp: ");
    Serial.println(lampStatus);
}

/***************************************************
* Mostra dados capturados LCD I2C
****************************************************/
void printDadosLCD(void){
    lcd.setCursor (5,0); lcd.print("SISMMAR"); //1a linha
    lcd.setCursor (0,1); lcd.print("Temp "); lcd.print(tempDHT); //2a linha
    lcd.print("C Umd "); lcd.print(umdDHT); lcd.print("%");
    lcd.setCursor (0,2); lcd.print("Solo "); lcd.print(umdSolo); lcd.print("%"); //3a linha
    lcd.print(" Luz "); lcd.print(lumin); lcd.print("%");
    lcd.setCursor (0,3); lcd.print("Bomb "); lcd.print(bombaStatus); //4a linha
    lcd.print("   Lamp "); lcd.print(lampStatus);
}

/***************************************************
* Debouncing de push-button
****************************************************/
boolean debounce(int pin){
    boolean estado;
    boolean previaEstado;
    const int debounceDelay = 60;
    previaEstado = digitalRead(pin);
    for(int cont = 0; cont < debounceDelay; cont++){
        delay(1);
        estado = digitalRead(pin);
        if(estado != previaEstado){
            cont =0;
            previaEstado = estado;
        } 
    }
    return estado;
}
