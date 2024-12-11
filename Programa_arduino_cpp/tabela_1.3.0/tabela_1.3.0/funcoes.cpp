#include "funcoes.h"
#include <SoftwareSerial.h>

// Definições de pinos
const int botaoNextPin = 6;
const int botaoPrevPin = 7;
const int botaoLantanideosPin = 8;
const int botaoActinideosPin = 9;
const int rxPin = 10;  // Pino RX para SoftwareSerial
const int txPin = 11;  // Pino TX para SoftwareSerial

// Variáveis globais
unsigned long previousMillis = 0;
const long interval = 10;
unsigned long lastActivityMillis = 0;
const long timeout = 60000;

bool isPlaying = false;
bool isLoopEnabled = false;
int elementoAtual = 1;
int linhaAtual = 0;
int colunaAtual = 0;

// Instâncias do DFPlayer e SoftwareSerial
DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(rxPin, txPin);  // RX, TX

void inicializarSistema() {
    // Inicializa o sistema (LCD, DFPlayer, pinos, etc.)
    Serial.begin(115200);
    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial)) {
        Serial.println(F("Erro ao iniciar DFPlayer"));
        while (true);
    }
    myDFPlayer.volume(20);  // Volume inicial
    
    // Configuração dos pinos dos botões
    pinMode(botaoNextPin, INPUT_PULLUP);
    pinMode(botaoPrevPin, INPUT_PULLUP);
    pinMode(botaoLantanideosPin, INPUT_PULLUP);
    pinMode(botaoActinideosPin, INPUT_PULLUP);
}

void verificarEstados() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        int eixoX = analogRead(A0);
        int eixoY = analogRead(A1);
        int estadoBotaoNext = digitalRead(botaoNextPin);
        int estadoBotaoPrev = digitalRead(botaoPrevPin);
        int estadoBotaoLantanideos = digitalRead(botaoLantanideosPin);
        int estadoBotaoActinideos = digitalRead(botaoActinideosPin);

        bool posicaoMudou = false;

        // Mapeia o movimento do joystick
        if (eixoX == 1022) {
            colunaAtual++;
            if (colunaAtual >= colunas) colunaAtual = 0;
            posicaoMudou = true;
        } else if (eixoX == 0) {
            colunaAtual--;
            if (colunaAtual < 0) colunaAtual = colunas - 1;
            posicaoMudou = true;
        }

        if (eixoY == 1022) {
            linhaAtual++;
            if (linhaAtual >= linhas) linhaAtual = 0;
            posicaoMudou = true;
        } else if (eixoY == 0) {
            linhaAtual--;
            if (linhaAtual < 0) linhaAtual = linhas - 1;
            posicaoMudou = true;
        }

        // Se a posição mudou, armazena e reproduz o novo áudio
        if (posicaoMudou) {
            elementoAtual = tabelaPeriodica[linhaAtual][colunaAtual];
            if (elementoAtual != 0) {
                myDFPlayer.play(elementoAtual);
                imprimirNumeroAtomico(elementoAtual);
                isPlaying = true;
                lastActivityMillis = currentMillis;
            }
        }

        // Ação para o botão Next
        if (estadoBotaoNext == LOW) {
            if (elementoAtual < 118) { // Supondo que 118 é o último elemento
                elementoAtual++;
                myDFPlayer.play(elementoAtual);
                imprimirNumeroAtomico(elementoAtual);
                isPlaying = true;
                lastActivityMillis = currentMillis;
                delay(200);
            }
        }

        // Ação para o botão Previous
        if (estadoBotaoPrev == LOW) {
            if (elementoAtual > 1) {
                elementoAtual--;
                myDFPlayer.play(elementoAtual);
                imprimirNumeroAtomico(elementoAtual);
                isPlaying = true;
                lastActivityMillis = currentMillis;
                delay(200);
            }
        }

        // Verificação de estado contínua
        while (isPlaying) {
            if (myDFPlayer.readState() != 1) {
                isPlaying = false;
                lastActivityMillis = currentMillis;
            }
            delay(10);
        }

        // Verifica se o dispositivo está ocioso por 60 segundos
        if ((currentMillis - lastActivityMillis >= timeout) && !isPlaying) {
            myDFPlayer.play(1);
            imprimirNumeroAtomico(1);
            isPlaying = true;
            lastActivityMillis = currentMillis;
        }
    }
}

void verificarRequisicoes() {
    // Função para verificar se há requisições de funções
    // Você pode adicionar verificações específicas aqui
}

void imprimirNumeroAtomico(int elemento) {
    // Função para imprimir o número atômico
    Serial.print("Número atômico: ");
    Serial.println(elemento);
}

void watchdog() {
    // Função watchdog para verificar estado e reiniciar DFPlayer se necessário
    reiniciarDFPlayer(); // Reinicializa o DFPlayer
    liberarMemoria(); // Tenta liberar memória, se necessário
}

// Função para reinicializar o DFPlayer
void reiniciarDFPlayer() {
    mySoftwareSerial.end(); // Finaliza a comunicação serial
    delay(1000); // Aguardar 1 segundo antes de reiniciar
    mySoftwareSerial.begin(9600); // Reinicia a comunicação serial
    myDFPlayer.begin(mySoftwareSerial); // Reinicia o DFPlayer
    myDFPlayer.volume(20); // Volume inicial
}

// Função para liberar memória
void liberarMemoria() {
    // Adicione códigos específicos para liberar memória, se necessário
}
