#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <avr/wdt.h> // Biblioteca para watchdog timer

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C, 16 colunas, 2 linhas

// Função para exibir texto no LCD caractere por caractere
void exibirTextoLCD(const char* linha1, const char* linha2) {
    lcd.clear(); // Limpa a tela do LCD

    // Exibe a primeira linha
    lcd.setCursor(0, 0);
    for (int i = 0; linha1[i] != '\0'; i++) {
        lcd.print(linha1[i]);
        delay(200); // Atraso de 200ms entre os caracteres
    }

    // Exibe a segunda linha
    lcd.setCursor(0, 1);
    for (int i = 0; linha2[i] != '\0'; i++) {
        lcd.print(linha2[i]);
        delay(200); // Atraso de 200ms entre os caracteres
    }
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

    // Mensagem inicial
    exibirTextoLCD("Inicializando...", "Checando: Audio");
    delay(2000); // Atraso para visualização

    // Verifica se o módulo DFPlayer Mini está inicializado corretamente
    if (!myDFPlayer.begin(mySoftwareSerial)) {
        exibirTextoLCD("Erro: Verifique", "Problemas");
        reiniciarArduino();
    }

    myDFPlayer.setTimeOut(500); // Timeout serial 500ms
    myDFPlayer.volume(20); // Volume (de 0 até 30)
    myDFPlayer.EQ(0); // Equalização normal

    exibirTextoLCD("    Sucesso!    ", "   Bem-vindo!   ");
    delay(5000); // Aguarda 5 segundos antes de mudar a mensagem

    // Exibe "Tabela Periodica" e "****IFSP-CPV****"
    exibirTextoLCD("Tabela Periodica", "    IFSP-CPV    ");

    pinMode(botaoNextPin, INPUT_PULLUP); // Configurar o pino do botão Next como entrada com pull-up interno
    pinMode(botaoPrevPin, INPUT_PULLUP); // Configurar o pino do botão Previous como entrada com pull-up interno
    pinMode(botaoLantanideosPin, INPUT_PULLUP); // Configurar o pino do botão Lantanídeos como entrada com pull-up interno
    pinMode(botaoActinideosPin, INPUT_PULLUP); // Configurar o pino do botão Actinídeos como entrada com pull-up interno
}

unsigned long previousMillis = 0; // Variável para armazenar o tempo
const long interval = 10; // Intervalo mínimo recomendado para verificação (10 ms)
unsigned long lastActivityMillis = 0; // Variável para armazenar o tempo da última atividade
const long timeout = 60000; // Tempo de inatividade em milissegundos (60 segundos)

// Variáveis globais para armazenar estado da reprodução e a próxima ação
bool isPlaying = false;
bool isLoopEnabled = false; // Variável para armazenar o estado do loop
int elementoAtual = 1; // Variável para armazenar o elemento atual, inicializada com 1


void loop() {
    unsigned long currentMillis = millis(); // Obtém o tempo atual

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // Atualiza o tempo

        int eixoX = analogRead(A0); // Joystick eixo x
        int eixoY = analogRead(A1); // Joystick eixo y
        int estadoBotaoNext = digitalRead(botaoNextPin); // Ler o estado do botão Next
        int estadoBotaoPrev = digitalRead(botaoPrevPin); // Ler o estado do botão Previous
        int estadoBotaoLantanideos = digitalRead(botaoLantanideosPin); // Ler o estado do botão Lantanídeos
        int estadoBotaoActinideos = digitalRead(botaoActinideosPin); // Ler o estado do botão Actinídeos

        bool posicaoMudou = false;

        // Mapeia o movimento do joystick
        if (eixoX == 1022) {
            colunaAtual++;
            if (colunaAtual >= colunas) colunaAtual = 0; // Loop de volta ao início
            posicaoMudou = true;
        } else if (eixoX == 0) {
            colunaAtual--;
            if (colunaAtual < 0) colunaAtual = colunas - 1; // Loop de volta ao final
            posicaoMudou = true;
        }

        if (eixoY == 1022) {
            linhaAtual++;
            if (linhaAtual >= linhas) linhaAtual = 0; // Loop de volta ao início
            posicaoMudou = true;
        } else if (eixoY == 0) {
            linhaAtual--;
            if (linhaAtual < 0) linhaAtual = linhas - 1; // Loop de volta ao final
            posicaoMudou = true;
        }

        // Se a posição mudou, armazena e reproduz o novo áudio
        if (posicaoMudou) {
            elementoAtual = tabelaPeriodica[linhaAtual][colunaAtual];
            if (elementoAtual != 0) {
                myDFPlayer.play(elementoAtual);                
                isPlaying = true;
                lastActivityMillis = currentMillis; // Atualiza o tempo da última atividade
            }
        }

        // Ação para o botão Next
        if (estadoBotaoNext == LOW) {
            elementoAtual++; // Incrementa o elemento atual
            myDFPlayer.play(elementoAtual); // Reproduz o novo áudio            
            isPlaying = true;
            lastActivityMillis = currentMillis; // Atualiza o tempo da última atividade
            delay(200); // Pequeno atraso para evitar múltiplas leituras
        }

        // Ação para o botão Previous
        if (estadoBotaoPrev == LOW) {
            if (elementoAtual > 1) {
                elementoAtual--; // Decrementa o elemento atual
                myDFPlayer.play(elementoAtual); // Reproduz o novo áudio                
                isPlaying = true;
                lastActivityMillis = currentMillis; // Atualiza o tempo da última atividade
            }
            delay(200); // Pequeno atraso para evitar múltiplas leituras
        }

        // Verifica se a reprodução atual terminou
        if (isPlaying && myDFPlayer.readState() != 1) { // 1 é o código para reprodução em andamento
            isPlaying = false;
            lastActivityMillis = currentMillis; // Reinicia o tempo de inatividade após o áudio terminar
        }

        // Verifica se o dispositivo está ocioso por 60 segundos
        if ((currentMillis - lastActivityMillis >= timeout) && !isPlaying) {
            myDFPlayer.play(1); // Reproduz o áudio 1            
            isPlaying = true;
            lastActivityMillis = currentMillis; // Atualiza o tempo da última atividade
        }
    }
}

