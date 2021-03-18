/*****************************************************************
*Sensor LDR - Teste.
*Denisson Silva - 04 de março de 2021
******************************************************************/
 
int ldrPin = A7; //LDR no pino analogico A7
 
void setup() {
    pinMode(ldrPin, INPUT);
    Serial.begin(9600); //Inicia a comunicação serial
    Serial.println("***** LDR - Teste *****");
}
 
void loop() {
    ///ler o valor do LDR
    luminosidade(ldrPin);  
}

//Função para ler as medições do LDR
int luminosidade(int ldrPino){
    int ldrValor = 0;
    for(int i = 0; i < 10; i++){ //Le 10 medicoes do LDR e as soma.
        ldrValor = ldrValor + analogRead(ldrPino);
        delay(100);
    }
    ldrValor = ldrValor / 10; //Calculo da média aritmética das medicoes.
    ldrValor = map(ldrValor, 1023, 0, 100, 0); //Escuro:0%, Claro:100%.
    Serial.print("Luminosidade ==> ");
    Serial.print(ldrValor);
    Serial.println("%");
    delay(1000);
    return ldrValor;
}
