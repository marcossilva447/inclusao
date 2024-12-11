#include "funcoes.h"
#include <avr/wdt.h>  // Biblioteca para watchdog timer

// Tabela periódica definida no arquivo principal
const int linhas = 7;
const int colunas = 18;
int tabelaPeriodica[linhas][colunas] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, // Primeiro período
    {3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 0}, // Segundo período
    {11, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 14, 15, 16, 17, 18}, // Terceiro período
    {19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36}, // Quarto período
    {37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54}, // Quinto período
    {55, 56, 0, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86}, // Sexto período, com lantanídeos substituídos por zero
    {87, 88, 0, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118} // Sétimo período, com actinídeos substituídos por zero
};

// Variáveis para controle do tempo de verificação
unsigned long previousCheckMillis = 0;
unsigned long previousWatchdogMillis = 0;
const long checkInterval = 10000; // Intervalo de 10 segundos
const long watchdogInterval = 60000; // Intervalo de 60 segundos

void setup() {
    inicializarSistema();
    wdt_disable(); // Desabilitar Watchdog Timer ao iniciar
    wdt_enable(WDTO_8S); // Habilitar Watchdog Timer com timeout de 8 segundos
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Verificação periódica do módulo DFPlayer a cada 10 segundos
    if (currentMillis - previousCheckMillis >= checkInterval) {
        previousCheckMillis = currentMillis;
        verificarRequisicoes();
    }

    // Chamada programada da função watchdog a cada 60 segundos
    if (currentMillis - previousWatchdogMillis >= watchdogInterval) {
        previousWatchdogMillis = currentMillis;
        watchdog(); // Chama a função watchdog
    }

    wdt_reset(); // Reset do Watchdog Timer
    verificarEstados();
}
