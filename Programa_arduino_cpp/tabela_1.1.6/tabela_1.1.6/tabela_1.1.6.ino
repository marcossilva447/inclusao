// Inclusão das Bibliotecas
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h> // Biblioteca para watchdog timer

// Inicia a serial para Módulo DFPlayer Mini e cria variável para controle do módulo
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer; 

// Inicializa a biblioteca LCD com o endereço I2C 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C, 16 colunas, 2 linhas

// Função para imprimir o número atômico no LCD e no monitor serial
void imprimirNumeroAtomico(int numeroAudio) {
  int numeroAtomico = numeroAudio + 1; // Calcula o número atômico
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reproduzindo:");
  lcd.setCursor(0, 1);
  lcd.print("Num. atomico ");
  lcd.print(numeroAtomico);

  // Envia a informação para o monitor serial
  Serial.print("Numero Atomico ");
  Serial.println(numeroAtomico);
}

// Função para reiniciar o Arduino
void reiniciarArduino() {
  wdt_enable(WDTO_15MS); // Habilita o watchdog timer para reiniciar o Arduino em 15 ms 
  while (true) {} // Aguarda o watchdog reiniciar o Arduino
}

const int linhas = 7; // Total de períodos (linhas)
const int colunas = 18; // Total de grupos (colunas)



// Matriz representando a tabela periódica
int tabelaPeriodica[linhas][colunas] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, // Primeiro período
    {3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 0}, // Segundo período
    {11, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 15, 16, 17, 18}, // Terceiro período
    {19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36}, // Quarto período
    {37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54}, // Quinto período
    {55, 56, 0, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86}, // Sexto período, com lantanídeos substituídos por zero
    {87, 88, 0, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118} // Sétimo período, com actinídeos substituídos por zero
};


int linhaAtual = 0; // Período inicial
int colunaAtual = 0; // Grupo inicial
const int botaoNextPin = 7; // Pin do botão Next
const int botaoPrevPin = 6; // Pin do botão Previous
const int botaoLantanideosPin = 8; // Pin do botão para lantanídeos
const int botaoActinideosPin = 9; // Pin do botão para actinídeos

void setup() {
    Serial.begin(115200); // Inicializa a serial do Arduino
    mySoftwareSerial.begin(9600); // comunicação com o módulo

    // Inicializa a comunicação com o LCD via I2C
    lcd.begin();
    lcd.backlight();
    lcd.print("Inicializando...");

    // Verifica se o módulo está conectado e se cartão SD foi inserido
    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini"));
    Serial.println(F("Inicializando módulo DFPlayer..."));
    lcd.setCursor(0, 1);
    lcd.print("DFP Mini");
    delay(2000);

    if (!myDFPlayer.begin(mySoftwareSerial)) {
        Serial.println(F("Não inicializado:"));
        Serial.println(F("- Verifique se módulo foi corretamente conectado ou"));
        Serial.println(F("- Insira um cartão SD"));
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Nao inicializado");
        lcd.setCursor(0, 1);
        lcd.print("Verif. conexoes");
        reiniciarArduino(); // Chama a função para reiniciar o Arduino
    }

    // Definições iniciais
    myDFPlayer.setTimeOut(500); // Timeout serial 500ms
    myDFPlayer.volume(20); // Volume (de 0 até 30)
    myDFPlayer.EQ(0); // Equalização normal

    Serial.println();
    Serial.print("Número de arquivos no cartão: ");
    int numFiles = myDFPlayer.readFileCounts(DFPLAYER_DEVICE_SD);
    Serial.println(numFiles);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Num. arquivos:");
    lcd.setCursor(0, 1);
    lcd.print(numFiles);

    delay(2000);

    Serial.println();
    Serial.println(F("Módulo DFPlayer Mini inicializado corretamente!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Modulo DFP Mini");
    lcd.setCursor(0, 1);
    lcd.print("inic. correto!");
    delay(2000);

    pinMode(botaoNextPin, INPUT_PULLUP); // Configurar o pino do botão Next como entrada com pull-up interno
    pinMode(botaoPrevPin, INPUT_PULLUP); // Configurar o pino do botão Previous como entrada com pull-up interno
    pinMode(botaoLantanideosPin, INPUT_PULLUP); // Configurar o pino do botão Lantanídeos como entrada com pull-up interno
    pinMode(botaoActinideosPin, INPUT_PULLUP); // Configurar o pino do botão Actinídeos como entrada com pull-up interno
}

// Variáveis globais para armazenar estado da reprodução e a próxima ação
bool isPlaying = false;
int nextAction = 0; // 0: Nenhuma ação, 1: Próxima faixa, 2: Faixa anterior, 3: Lantanídeos, 4: Actinídeos

void loop() {
    int eixoX = analogRead(A0); // Joystick eixo x
    int eixoY = analogRead(A1); // Joystick eixo y
    int estadoBotaoNext = digitalRead(botaoNextPin); // Ler o estado do botão Next
    int estadoBotaoPrev = digitalRead(botaoPrevPin); // Ler o estado do botão Previous
    int estadoBotaoLantanideos = digitalRead(botaoLantanideosPin); // Ler o estado do botão Lantanídeos
    int estadoBotaoActinideos = digitalRead(botaoActinideosPin); // Ler o estado do botão Actinídeos

    if (!isPlaying) {
        if (estadoBotaoNext == LOW) {
            nextAction = 1; // Próxima faixa
        } else if (estadoBotaoPrev == LOW) {
            nextAction = 2; // Faixa anterior
        } else if (estadoBotaoLantanideos == LOW) {
            nextAction = 3; // Lantanídeos
        } else if (estadoBotaoActinideos == LOW) {
            nextAction = 4; // Actinídeos
        } else {
            if (eixoX == 1022) {
                colunaAtual++;
                if (colunaAtual >= colunas) colunaAtual = 0; // Loop de volta ao início
            } else if (eixoX == 0) {
                colunaAtual--;
                if (colunaAtual < 0) colunaAtual = colunas - 1; // Loop de volta ao final
            }

            if (eixoY == 1022) {
                linhaAtual++;
                if (linhaAtual >= linhas) linhaAtual = 0; // Loop de volta ao início
            } else if (eixoY == 0) {
                linhaAtual--;
                if (linhaAtual < 0) linhaAtual = linhas - 1; // Loop de volta ao final
            }

            // Verifique o elemento atual na matriz
            int elementoAtual = tabelaPeriodica[linhaAtual][colunaAtual];

            // Ações com base no elemento atual
            if (elementoAtual != 0) {
                myDFPlayer.play(elementoAtual);
                imprimirNumeroAtomico(elementoAtual); // Chama a função para imprimir o número atômico
                isPlaying = true;
                delay(50); // Pequeno atraso para evitar movimentos muito rápidos
            }
        }
    } else {
        // Verifica se a reprodução atual terminou
        if (myDFPlayer.readState() != 1) { // 1 é o código para reprodução em andamento
            isPlaying = false;
            // Executa a próxima ação armazenada
            if (nextAction == 1) {
                myDFPlayer.next();
            } else if (nextAction == 2) {
                myDFPlayer.previous();
            } else if (nextAction == 3) {
                myDFPlayer.playFolder(1, 1);
            } else if (nextAction == 4) {
                myDFPlayer.playFolder(2, 1);
            }
            nextAction = 0; // Reseta a ação
            delay(50); // Delay menor para evitar múltiplas leituras do botão
        }
    }
}

