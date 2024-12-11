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

// Função para imprimir o número atômico no LCD
void imprimirNumeroAtomico(int numeroAudio) {
  int numeroAtomico = numeroAudio + 1; // Calcula o número atômico
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reproduzindo:");
  lcd.setCursor(0, 1);
  lcd.print("Num. atomico ");
  lcd.print(numeroAtomico);
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
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3}, // Primeiro período
    {4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0}, // Segundo período
    {12, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 15, 16, 17, 18, 19}, // Terceiro período
    {20, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33, 34, 35, 36, 37}, // Quarto período
    {38, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 51, 52, 53, 54, 55}, // Quinto período
    {56, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 82, 83, 84, 85, 86, 87}, // Sexto período
    {88, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 114, 115, 116, 117, 118, 119} // Sétimo período
};

int linhaAtual = 0; // Período inicial
int colunaAtual = 0; // Grupo inicial
const int botaoNextPin = 7; // Pin do botão Next
const int botaoPrevPin = 6; // Pin do botão Previous
const int botaoLantanideosPin = 8; // Pin do botão para lantanídeos
const int botaoActinideosPin = 9; // Pin do botão para actinídeos

void setup() {
    Serial.begin(115200); // Inicializa a serial do Arduino
    mySoftwareSerial.begin(9600); //comunicação com o módulo

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

void loop() {
    int eixoX = analogRead(A0); // Joystick eixo x
    int eixoY = analogRead(A1); // Joystick eixo y
    int estadoBotaoNext = digitalRead(botaoNextPin); // Ler o estado do botão Next
    int estadoBotaoPrev = digitalRead(botaoPrevPin); // Ler o estado do botão Previous
    int estadoBotaoLantanideos = digitalRead(botaoLantanideosPin); // Ler o estado do botão Lantanídeos
    int estadoBotaoActinideos = digitalRead(botaoActinideosPin); // Ler o estado do botão Actinídeos

    if (estadoBotaoNext == LOW) {
        myDFPlayer.next(); // Navegar para o próximo áudio
        delay(500); // Debounce para evitar múltiplas leituras do botão
    } else if (estadoBotaoPrev == LOW) {
        myDFPlayer.previous(); // Navegar para o áudio anterior
        delay(500); // Debounce para evitar múltiplas leituras do botão
    } else if (estadoBotaoLantanideos == LOW) {
        myDFPlayer.playFolder(1, 1); // Tocar a primeira faixa da pasta 1 (Lantanídeos)
        delay(500); // Debounce para evitar múltiplas leituras do botão
    } else if (estadoBotaoActinideos == LOW) {
        myDFPlayer.playFolder(2, 1); // Tocar a primeira faixa da pasta 2 (Actinídeos)
        delay(500); // Debounce para evitar múltiplas leituras do botão
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
        }

        delay(200); // Pequeno atraso para evitar movimentos muito rápidos
    }
}