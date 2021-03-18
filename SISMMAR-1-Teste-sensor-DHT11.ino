/*****************************************************************
*Sensor DHT - Configuracao e teste.
*Baseado no codigo original escrito por ladyada, dominio publico.
*Denisson Silva - 04 de março de 2021
******************************************************************/

//Inclusao de bibliotecas
#include <DHT.h>
#include <DHT_U.h>


//Definicoes do sensor
#define DHTPIN 5 //pino de saida de dados
#define DHTTYPE DHT11 //tipo do sensor

//Variaveis usadas pelo sensor
float tempDHT;
float umdDHT;
float fic;

//Inicializa o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    Serial.println("***** DHT 11 - Configuracoes e testes *****");
    dht.begin();
}

void loop() {
    //Aguarda dois segundos entre as medicoes.
    delay(2000);

    //Le os valores de temperatura e umidade do sensor DHT.
    tempDHT = dht.readTemperature();
    umdDHT = dht.readHumidity();

    //Verifica se alguma leitura falhou e tenta ler novamente.
    if(isnan(umdDHT) || isnan(tempDHT)){
        Serial.println("Falha na leitura do sensor DHT!");
        return;
    }

    //Calcula e converte a temperatura em celsius
    float fic = dht.computeHeatIndex(tempDHT, umdDHT, false);

    //Mostra as medicoes no monitor serial
    Serial.print("  Temp DHT ==> ");
    Serial.print(tempDHT);
    Serial.print(" oC Umid DHT ==> ");
    Serial.print(umdDHT);
    Serial.println("%");
    //Serial.print(fic);
    //Serial.println(" oC ");
}
