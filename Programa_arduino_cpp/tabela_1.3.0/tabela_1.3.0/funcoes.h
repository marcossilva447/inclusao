#ifndef FUNCOES_H
#define FUNCOES_H

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Declarações de pinos
extern const int botaoNextPin;
extern const int botaoPrevPin;
extern const int botaoLantanideosPin;
extern const int botaoActinideosPin;
extern const int rxPin;
extern const int txPin;

// Declarações de variáveis globais
extern unsigned long previousMillis;
extern const long interval;
extern unsigned long lastActivityMillis;
extern const long timeout;

extern bool isPlaying;
extern bool isLoopEnabled;
extern int elementoAtual;
extern int linhaAtual;
extern int colunaAtual;
extern const int linhas;
extern const int colunas;
extern int tabelaPeriodica[7][18];

// Declaração de instâncias
extern DFRobotDFPlayerMini myDFPlayer;
extern SoftwareSerial mySoftwareSerial;

// Declaração de funções
void inicializarSistema();
void verificarEstados();
void verificarRequisicoes();
void imprimirNumeroAtomico(int elemento);
void watchdog();
void reiniciarDFPlayer();
void liberarMemoria();

#endif
