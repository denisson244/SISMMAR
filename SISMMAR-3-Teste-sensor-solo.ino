/*****************************************************************
*Sensor de Umidade de Solo - Configuracao e teste.
*Baseado no codigo original "How to: Soil Moisture Measurement.
*(http;//forum.arduino.c/index.php?topic=37975.0)
*Denisson Silva - 08 de março de 2021
******************************************************************/

#define SENSOR_SOLO_PIN 1 //Entrada do sensor de umidade de solo
#define SUS_VCC 7 //Sensor Umidade Solo VCC
#define SUS_GND 6 //Sensor Umidade Solo GND

int umdSolo;

void setup() {
    Serial.begin(9600);
    Serial.println("***** Sensor de solo - Teste *****");
    pinMode(SUS_VCC, OUTPUT);
    pinMode(SUS_GND, OUTPUT);
}

void loop() {
    umdSolo = getUmidadeSolo();
    Serial.print("Umidade do solo: ");
    Serial.print(umdSolo);
    Serial.println(" %");
}

//Leitura das medicoes do sensor de solo.
int getUmidadeSolo(){
    int umdValor;
    digitalWrite(SUS_VCC, LOW); //Direciona a corrente eletrica.
    digitalWrite(SUS_GND, HIGH); 
    delay(1000); //Aguarda 1s para o direcionamento ter efeito.
    umdValor = analogRead(SENSOR_SOLO_PIN);
    
    digitalWrite(SUS_VCC, HIGH); //Inverte a direcao da corrente eletrica.
    digitalWrite(SUS_GND, LOW); 
    delay(1000); //Aguarda 1s para a inversao ter efeito.
    digitalWrite(SUS_VCC, LOW); //Para a corrente eletrica.

    umdValor = map(umdValor, 1023, 0, 0, 100);
    return umdValor;
}
