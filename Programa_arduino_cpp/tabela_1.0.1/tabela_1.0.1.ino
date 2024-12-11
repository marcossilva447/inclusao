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

// Declaração de variáveis
int pin_botao1 = 6;
int botao1 = 1;
unsigned long lastPressTime = 0;
unsigned long debounceDelay = 200; // atraso para debounce

// Declaração dos pinos para os eixos X e Y
int pin_eixo_x = A0;
int pin_eixo_y = A1;

// Variáveis para detecção de inatividade
unsigned long lastActivityTime = 0;
const unsigned long inactivityThreshold = 50000; // 50 segundos

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

// Função para tocar um áudio aleatório
void tocarAudioAleatorio() {
  int audioAleatorio = random(1, 118); // Gera um número aleatório entre 1 e 117
  myDFPlayer.play(audioAleatorio);
  imprimirNumeroAtomico(audioAleatorio - 1); // Atualiza o LCD com o número atômico do áudio
}

// Função para reiniciar o Arduino
void reiniciarArduino() {
  wdt_enable(WDTO_15MS); // Habilita o watchdog timer para reiniciar o Arduino em 15 ms 
  while (true) {} // Aguarda o watchdog reiniciar o Arduino
}

/* 
 * SETUP 
 */
void setup() {
  // Configura pinos dos botões    
  pinMode(pin_botao1, INPUT_PULLUP);
  
  // Comunicação serial com o módulo
  mySoftwareSerial.begin(9600);
  
  // Inicializa a serial do Arduino
  Serial.begin(115200);
  
  // Inicializa a comunicação com o LCD via I2C
  lcd.begin();
  lcd.backlight();
  lcd.print("Inicializando...");
  
  // Inicializa a semente do gerador de números aleatórios
  randomSeed(analogRead(A0));
  
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
}


/* 
 * LOOP 
 */

void loop() {
  // Leitura do botão
  botao1 = digitalRead(pin_botao1);
  unsigned long currentTime = millis();

  // Leitura dos eixos X e Y
  int eixoX = analogRead(pin_eixo_x);
  int eixoY = analogRead(pin_eixo_y);

  // Obtém o número do áudio que está sendo reproduzido no módulo DFPlayer Mini
  int numeroAudio = myDFPlayer.readCurrentFileNumber();

  // Chama a função para imprimir o número atômico no LCD
  imprimirNumeroAtomico(numeroAudio);

  // Verifica a inatividade
  if (currentTime - lastActivityTime > inactivityThreshold) {
    tocarAudioAleatorio();
    lastActivityTime = currentTime; // Reseta o temporizador de inatividade
  }

  // Detecta movimento do cursor (ou outra atividade)
  if (eixoX > 10 || eixoX < 1014 || eixoY > 10 || eixoY < 1014) {
    lastActivityTime = currentTime; // Atualiza o tempo da última atividade
  }

  // Exibe os valores lidos dos eixos X e Y no console
  Serial.print("Eixo X: ");
  Serial.print(eixoX);
  Serial.print(" | Eixo Y: ");
  Serial.println(eixoY);

  // Verifica se o botão 1 foi pressionado
  if (botao1 == LOW && currentTime - lastPressTime > debounceDelay) {
    lastPressTime = currentTime;
    
    if (numeroAudio == 0) { // Nenhum áudio tocando
      myDFPlayer.play(1); // Inicia a reprodução do áudio instrucional
      Serial.println("Tocando audio instrucional");
    } else {
      myDFPlayer.next(); // Avança para o próximo áudio
      Serial.print("Tocando próximo áudio: ");
      Serial.println(myDFPlayer.readCurrentFileNumber());
    }
  }

  // Verifica os casos individuais para movimentação do cursor analógico
  switch (numeroAudio) {
   case 1: // Áudio 1 está tocando (Instruções)
  if (eixoX == 0) {
    myDFPlayer.play(2); // Mover para a direita, mudar para áudio 2 (Hidrogênio)
  }
  break;
      
 case 2: // Áudio 2 - Hidrogênio (Número Atômico 1)
  if (eixoX == 0) {
    myDFPlayer.play(3); // Avança para Hélio (Número Atômico 2, Áudio 3)
  } else if (eixoX == 1022) {
    myDFPlayer.play(2); // Instruções específicas para Hidrogênio
  } else if (eixoY == 0) {
    myDFPlayer.play(4); // Navegação por grupo: vai para Lítio (Número Atômico 3, Áudio 4)
  } else if (eixoY == 1022) {
    myDFPlayer.play(2); // Retorna para Hidrogênio
  }
  break;

  case 3: // Áudio 3 - Hélio (Número Atômico 2)
  if (eixoX == 0) {
    myDFPlayer.play(4); // Avança para Lítio (Número Atômico 3, Áudio 4)
  } else if (eixoX == 1022) {
    myDFPlayer.play(2); // Retorna para Hidrogênio (Número Atômico 1, Áudio 2)
  } else if (eixoY == 0) {
    myDFPlayer.play(10); // Navegação por grupo: vai para Neônio (Número Atômico 10, Áudio 11)
  } else if (eixoY == 1022) {
    myDFPlayer.play(2); // Retorna para Hidrogênio
  }
  break;


  case 4: // Áudio 4 - Lítio (Número Atômico 3)
  if (eixoX == 0) {
    myDFPlayer.play(5); // Avança para Berílio (Número Atômico 4, Áudio 5)
  } else if (eixoX == 1022) {
    myDFPlayer.play(3); // Retorna para Hélio (Número Atômico 2, Áudio 3)
  } else if (eixoY == 0) {
    myDFPlayer.play(12); // Navegação por grupo: vai para Sódio (Número Atômico 11, Áudio 12)
  } else if (eixoY == 1022) {
    myDFPlayer.play(2); // Retorna para Hidrogênio (Número Atômico 1, Áudio 2)
  }
  break;


  case 5: // Áudio 5 - Berílio (Número Atômico 4)
  if (eixoX == 0) {
    myDFPlayer.play(6); // Avança para Boro (Número Atômico 5, Áudio 6)
  } else if (eixoX == 1022) {
    myDFPlayer.play(4); // Retorna para Lítio (Número Atômico 3, Áudio 4)
  } else if (eixoY == 0) {
    myDFPlayer.play(13); // Navegação por grupo: vai para Magnésio (Número Atômico 12, Áudio 13)
  } else if (eixoY == 1022) {
    myDFPlayer.play(4); // Retorna para Lítio (Número Atômico 3, Áudio 4)
  }
  break;

  case 6: // Áudio 6 - Boro (Número Atômico 5)
  if (eixoX == 0) {
    myDFPlayer.play(7); // Avança para Carbono (Número Atômico 6, Áudio 7)
  } else if (eixoX == 1022) {
    myDFPlayer.play(5); // Retorna para Berílio (Número Atômico 4, Áudio 5)
  } else if (eixoY == 0) {
    myDFPlayer.play(14); // Navegação por grupo: vai para Alumínio (Número Atômico 13, Áudio 14)
  } else if (eixoY == 1022) {
    myDFPlayer.play(5); // Retorna para Berílio (Número Atômico 4, Áudio 5)
  }
  break;

  case 7: // Áudio 7 - Carbono (Número Atômico 6)
  if (eixoX == 0) {
    myDFPlayer.play(8); // Avança para Nitrogênio (Número Atômico 7, Áudio 8)
  } else if (eixoX == 1022) {
    myDFPlayer.play(6); // Retorna para Boro (Número Atômico 5, Áudio 6)
  } else if (eixoY == 0) {
    myDFPlayer.play(15); // Navegação por grupo: vai para Silício (Número Atômico 14, Áudio 15)
  } else if (eixoY == 1022) {
    myDFPlayer.play(6); // Retorna para Boro (Número Atômico 5, Áudio 6)
  }
  break;


  case 8: // Áudio 8 - Nitrogênio (Número Atômico 7)
  if (eixoX == 0) {
    myDFPlayer.play(9); // Avança para Oxigênio (Número Atômico 8, Áudio 9)
  } else if (eixoX == 1022) {
    myDFPlayer.play(7); // Retorna para Carbono (Número Atômico 6, Áudio 7)
  } else if (eixoY == 0) {
    myDFPlayer.play(16); // Navegação por grupo: vai para Fósforo (Número Atômico 15, Áudio 16)
  } else if (eixoY == 1022) {
    myDFPlayer.play(7); // Retorna para Carbono (Número Atômico 6, Áudio 7)
  }
  break;


  case 9: // Áudio 9 - Oxigênio (Número Atômico 8)
  if (eixoX == 0) {
    myDFPlayer.play(10); // Avança para Flúor (Número Atômico 9, Áudio 10)
  } else if (eixoX == 1022) {
    myDFPlayer.play(8); // Retorna para Nitrogênio (Número Atômico 7, Áudio 8)
  } else if (eixoY == 0) {
    myDFPlayer.play(17); // Navegação por grupo: vai para Enxofre (Número Atômico 16, Áudio 17)
  } else if (eixoY == 1022) {
    myDFPlayer.play(8); // Retorna para Nitrogênio (Número Atômico 7, Áudio 8)
  }
  break;

case 10: // Áudio 10 - Flúor (Número Atômico 9)
  if (eixoX == 0) {
    myDFPlayer.play(11); // Avança para Neônio (Número Atômico 10, Áudio 11)
  } else if (eixoX == 1022) {
    myDFPlayer.play(9); // Retorna para Oxigênio (Número Atômico 8, Áudio 9)
  } else if (eixoY == 0) {
    myDFPlayer.play(18); // Navegação por grupo: vai para Cloro (Número Atômico 17, Áudio 18)
  } else if (eixoY == 1022) {
    myDFPlayer.play(9); // Retorna para Oxigênio (Número Atômico 8, Áudio 9)
  }
  break;

case 11: // Áudio 11 - Neônio (Número Atômico 10)
  if (eixoX == 0) {
    myDFPlayer.play(4); // Retorna para Lítio (Número Atômico 3, Áudio 4)
  } else if (eixoX == 1022) {
    myDFPlayer.play(10); // Retorna para Flúor (Número Atômico 9, Áudio 10)
  } else if (eixoY == 0) {
    myDFPlayer.play(18); // Navegação por grupo: vai para Argônio (Número Atômico 18, Áudio 19)
  } else if (eixoY == 1022) {
    myDFPlayer.play(10); // Retorna para Flúor (Número Atômico 9, Áudio 10)
  }
  break;

case 12: // Áudio 12 - Sódio (Número Atômico 11)
  if (eixoX == 0) {
    myDFPlayer.play(13); // Avança para Magnésio (Número Atômico 12, Áudio 13)
  } else if (eixoX == 1022) {
    myDFPlayer.play(11); // Retorna para Neônio (Número Atômico 10, Áudio 11)
  } else if (eixoY == 0) {
    myDFPlayer.play(20); // Navegação por grupo: vai para Potássio (Número Atômico 19, Áudio 20)
  } else if (eixoY == 1022) {
    myDFPlayer.play(4); // Retorna para Lítio (Número Atômico 3, Áudio 4)
  }
  break;

case 13: // Áudio 13 - Magnésio (Número Atômico 12)
  if (eixoX == 0) {
    myDFPlayer.play(14); // Avança para Alumínio (Número Atômico 13, Áudio 14)
  } else if (eixoX == 1022) {
    myDFPlayer.play(12); // Retorna para Sódio (Número Atômico 11, Áudio 12)
  } else if (eixoY == 0) {
    myDFPlayer.play(21); // Navegação por grupo: vai para Cálcio (Número Atômico 20, Áudio 21)
  } else if (eixoY == 1022) {
    myDFPlayer.play(5); // Retorna para Berílio (Número Atômico 4, Áudio 5)
  }
  break;

case 14: // Áudio 14 - Alumínio (Número Atômico 13)
  if (eixoX == 0) {
    myDFPlayer.play(15); // Avança para Silício (Número Atômico 14, Áudio 15)
  } else if (eixoX == 1022) {
    myDFPlayer.play(13); // Retorna para Magnésio (Número Atômico 12, Áudio 13)
  } else if (eixoY == 0) {
    myDFPlayer.play(31); // Navegação por grupo: vai para Gálio (Número Atômico 31, Áudio 31)
  } else if (eixoY == 1022) {
    myDFPlayer.play(6); // Retorna para Boro (Número Atômico 5, Áudio 6)
  }
  break;

case 15: // Áudio 15 - Silício (Número Atômico 14)
  if (eixoX == 0) {
    myDFPlayer.play(16); // Avança para Fósforo (Número Atômico 15, Áudio 16)
  } else if (eixoX == 1022) {
    myDFPlayer.play(14); // Retorna para Alumínio (Número Atômico 13, Áudio 14)
  } else if (eixoY == 0) {
    myDFPlayer.play(32); // Navegação por grupo: vai para Germânio (Número Atômico 32, Áudio 32)
  } else if (eixoY == 1022) {
    myDFPlayer.play(7); // Retorna para Carbono (Número Atômico 6, Áudio 7)
  }
  break;

case 16: // Áudio 16 - Fósforo (Número Atômico 15)
  if (eixoX == 0) {
    myDFPlayer.play(17); // Avança para Enxofre (Número Atômico 16, Áudio 17)
  } else if (eixoX == 1022) {
    myDFPlayer.play(15); // Retorna para Silício (Número Atômico 14, Áudio 15)
  } else if (eixoY == 0) {
    myDFPlayer.play(33); // Navegação por grupo: vai para Arsênio (Número Atômico 33, Áudio 33)
  } else if (eixoY == 1022) {
    myDFPlayer.play(8); // Retorna para Nitrogênio (Número Atômico 7, Áudio 8)
  }
  break;

case 17: // Áudio 17 - Enxofre (Número Atômico 16)
  if (eixoX == 0) {
    myDFPlayer.play(18); // Avança para Cloro (Número Atômico 17, Áudio 18)
  } else if (eixoX == 1022) {
    myDFPlayer.play(16); // Retorna para Fósforo (Número Atômico 15, Áudio 16)
  } else if (eixoY == 0) {
    myDFPlayer.play(34); // Navegação por grupo: vai para Selênio (Número Atômico 34, Áudio 34)
  } else if (eixoY == 1022) {
    myDFPlayer.play(9); // Retorna para Oxigênio (Número Atômico 8, Áudio 9)
  }
  break;

case 18: // Áudio 18 - Cloro (Número Atômico 17)
  if (eixoX == 0) {
    myDFPlayer.play(19); // Avança para Argônio (Número Atômico 18, Áudio 19)
  } else if (eixoX == 1022) {
    myDFPlayer.play(17); // Retorna para Enxofre (Número Atômico 16, Áudio 17)
  } else if (eixoY == 0) {
    myDFPlayer.play(35); // Navegação por grupo: vai para Bromo (Número Atômico 35, Áudio 35)
  } else if (eixoY == 1022) {
    myDFPlayer.play(10); // Retorna para Flúor (Número Atômico 9, Áudio 10)
  }
  break;

case 19: // Áudio 19 - Argônio (Número Atômico 18)
  if (eixoX == 0) {
    myDFPlayer.play(12); // Retorna para Sódio (Número Atômico 11, Áudio 12)
  } else if (eixoX == 1022) {
    myDFPlayer.play(18); // Retorna para Cloro (Número Atômico 17, Áudio 18)
  } else if (eixoY == 0) {
    myDFPlayer.play(36); // Navegação por grupo: vai para Criptônio (Número Atômico 36, Áudio 36)
  } else if (eixoY == 1022) {
    myDFPlayer.play(11); // Retorna para Neônio (Número Atômico 10, Áudio 11)
  }
  break;

case 20: // Áudio 20 - Potássio (Número Atômico 19)
  if (eixoX == 0) {
    myDFPlayer.play(21); // Avança para Cálcio (Número Atômico 20, Áudio 21)
  } else if (eixoX == 1022) {
    myDFPlayer.play(19); // Retorna para Argônio (Número Atômico 18, Áudio 19)
  } else if (eixoY == 0) {
    myDFPlayer.play(37); // Navegação por grupo: vai para Rubídio (Número Atômico 37, Áudio 37)
  } else if (eixoY == 1022) {
    myDFPlayer.play(12); // Retorna para Sódio (Número Atômico 11, Áudio 12)
  }
  break;

case 21: // Áudio 21 - Cálcio (Número Atômico 20)
  if (eixoX == 0) {
    myDFPlayer.play(22); // Avança para Escândio (Número Atômico 21, Áudio 22)
  } else if (eixoX == 1022) {
    myDFPlayer.play(20); // Retorna para Potássio (Número Atômico 19, Áudio 20)
  } else if (eixoY == 0) {
    myDFPlayer.play(38); // Navegação por grupo: vai para Estrôncio (Número Atômico 38, Áudio 38)
  } else if (eixoY == 1022) {
    myDFPlayer.play(13); // Retorna para Magnésio (Número Atômico 12, Áudio 13)
  }
  break;

case 22: // Áudio 22 - Escândio (Número Atômico 21)
  if (eixoX == 0) {
    myDFPlayer.play(23); // Avança para Titânio (Número Atômico 22, Áudio 23)
  } else if (eixoX == 1022) {
    myDFPlayer.play(21); // Retorna para Cálcio (Número Atômico 20, Áudio 21)
  } else if (eixoY == 0) {
    myDFPlayer.play(39); // Navegação por grupo: vai para Ítrio (Número Atômico 39, Áudio 39)
  } else if (eixoY == 1022) {
    myDFPlayer.play(14); // Retorna para Alumínio (Número Atômico 13, Áudio 14)
  }
  break;

case 23: // Áudio 23 - Titânio (Número Atômico 22)
  if (eixoX == 0) {
    myDFPlayer.play(24); // Avança para Vanádio (Número Atômico 23, Áudio 24)
  } else if (eixoX == 1022) {
    myDFPlayer.play(22); // Retorna para Escândio (Número Atômico 21, Áudio 22)
  } else if (eixoY == 0) {
    myDFPlayer.play(40); // Navegação por grupo: vai para Zircônio (Número Atômico 40, Áudio 40)
  } else if (eixoY == 1022) {
    myDFPlayer.play(15); // Retorna para Silício (Número Atômico 14, Áudio 15)
  }
  break;

case 24: // Áudio 24 - Vanádio (Número Atômico 23)
  if (eixoX == 0) {
    myDFPlayer.play(25); // Avança para Cromo (Número Atômico 24, Áudio 25)
  } else if (eixoX == 1022) {
    myDFPlayer.play(23); // Retorna para Titânio (Número Atômico 22, Áudio 23)
  } else if (eixoY == 0) {
    myDFPlayer.play(41); // Navegação por grupo: vai para Nióbio (Número Atômico 41, Áudio 41)
  } else if (eixoY == 1022) {
    myDFPlayer.play(16); // Retorna para Fósforo (Número Atômico 15, Áudio 16)
  }
  break;

case 25: // Áudio 25 - Cromo (Número Atômico 24)
  if (eixoX == 0) {
    myDFPlayer.play(26); // Avança para Manganês (Número Atômico 25, Áudio 26)
  } else if (eixoX == 1022) {
    myDFPlayer.play(24); // Retorna para Vanádio (Número Atômico 23, Áudio 24)
  } else if (eixoY == 0) {
    myDFPlayer.play(42); // Navegação por grupo: vai para Molibdênio (Número Atômico 42, Áudio 42)
  } else if (eixoY == 1022) {
    myDFPlayer.play(17); // Retorna para Enxofre (Número Atômico 16, Áudio 17)
  }
  break;

case 26: // Áudio 26 - Manganês (Número Atômico 25)
  if (eixoX == 0) {
    myDFPlayer.play(27); // Avança para Ferro (Número Atômico 26, Áudio 27)
  } else if (eixoX == 1022) {
    myDFPlayer.play(25); // Retorna para Cromo (Número Atômico 24, Áudio 25)
  } else if (eixoY == 0) {
    myDFPlayer.play(43); // Navegação por grupo: vai para Tecnécio (Número Atômico 43, Áudio 43)
  } else if (eixoY == 1022) {
    myDFPlayer.play(18); // Retorna para Cloro (Número Atômico 17, Áudio 18)
  }
  break;

case 27: // Áudio 27 - Ferro (Número Atômico 26)
  if (eixoX == 0) {
    myDFPlayer.play(28); // Avança para Cobalto (Número Atômico 27, Áudio 28)
  } else if (eixoX == 1022) {
    myDFPlayer.play(26); // Retorna para Manganês (Número Atômico 25, Áudio 26)
  } else if (eixoY == 0) {
    myDFPlayer.play(44); // Navegação por grupo: vai para Rutênio (Número Atômico 44, Áudio 44)
  } else if (eixoY == 1022) {
    myDFPlayer.play(19); // Retorna para Argônio (Número Atômico 18, Áudio 19)
  }
  break;

case 28: // Áudio 28 - Cobalto (Número Atômico 27)
  if (eixoX == 0) {
    myDFPlayer.play(29); // Avança para Níquel (Número Atômico 28, Áudio 29)
  } else if (eixoX == 1022) {
    myDFPlayer.play(27); // Retorna para Ferro (Número Atômico 26, Áudio 27)
  } else if (eixoY == 0) {
    myDFPlayer.play(45); // Navegação por grupo: vai para Ródio (Número Atômico 45, Áudio 45)
  } else if (eixoY == 1022) {
    myDFPlayer.play(20); // Retorna para Cálcio (Número Atômico 20, Áudio 21)
  }
  break;

case 29: // Áudio 29 - Níquel (Número Atômico 28)
  if (eixoX == 0) {
    myDFPlayer.play(30); // Avança para Cobre (Número Atômico 29, Áudio 30)
  } else if (eixoX == 1022) {
    myDFPlayer.play(28); // Retorna para Cobalto (Número Atômico 27, Áudio 28)
  } else if (eixoY == 0) {
    myDFPlayer.play(46); // Navegação por grupo: vai para Paládio (Número Atômico 46, Áudio 46)
  } else if (eixoY == 1022) {
    myDFPlayer.play(21); // Retorna para Escândio (Número Atômico 21, Áudio 22)
  }
  break;

case 30: // Áudio 30 - Cobre (Número Atômico 29)
  if (eixoX == 0) {
    myDFPlayer.play(31); // Avança para Zinco (Número Atômico 30, Áudio 31)
  } else if (eixoX == 1022) {
    myDFPlayer.play(29); // Retorna para Níquel (Número Atômico 28, Áudio 29)
  } else if (eixoY == 0) {
    myDFPlayer.play(47); // Navegação por grupo: vai para Prata (Número Atômico 47, Áudio 47)
  } else if (eixoY == 1022) {
    myDFPlayer.play(22); // Retorna para Titânio (Número Atômico 22, Áudio 23)
  }
  break;

case 31: // Áudio 31 - Gálio (Número Atômico 31)
  if (eixoX == 0) {
    myDFPlayer.play(32); // Avança para Germânio (Número Atômico 32, Áudio 32)
  } else if (eixoX == 1022) {
    myDFPlayer.play(30); // Retorna para Zinco (Número Atômico 30, Áudio 31)
  } else if (eixoY == 0) {
    myDFPlayer.play(48); // Navegação por grupo: vai para Índio (Número Atômico 49, Áudio 48)
  } else if (eixoY == 1022) {
    myDFPlayer.play(23); // Retorna para Titânio (Número Atômico 23, Áudio 24)
  }
  break;

case 32: // Áudio 32 - Germânio (Número Atômico 32)
  if (eixoX == 0) {
    myDFPlayer.play(33); // Avança para Arsênio (Número Atômico 33, Áudio 33)
  } else if (eixoX == 1022) {
    myDFPlayer.play(31); // Retorna para Gálio (Número Atômico 31, Áudio 31)
  } else if (eixoY == 0) {
    myDFPlayer.play(49); // Navegação por grupo: vai para Estanho (Número Atômico 50, Áudio 49)
  } else if (eixoY == 1022) {
    myDFPlayer.play(24); // Retorna para Vanádio (Número Atômico 23, Áudio 24)
  }
  break;

case 33: // Áudio 33 - Arsênio (Número Atômico 33)
  if (eixoX == 0) {
    myDFPlayer.play(34); // Avança para Selênio (Número Atômico 34, Áudio 34)
  } else if (eixoX == 1022) {
    myDFPlayer.play(32); // Retorna para Germânio (Número Atômico 32, Áudio 32)
  } else if (eixoY == 0) {
    myDFPlayer.play(50); // Navegação por grupo: vai para Antimônio (Número Atômico 51, Áudio 50)
  } else if (eixoY == 1022) {
    myDFPlayer.play(25); // Retorna para Cromo (Número Atômico 24, Áudio 25)
  }
  break;

case 34: // Áudio 34 - Selênio (Número Atômico 34)
  if (eixoX == 0) {
    myDFPlayer.play(35); // Avança para Bromo (Número Atômico 35, Áudio 35)
  } else if (eixoX == 1022) {
    myDFPlayer.play(33); // Retorna para Arsênio (Número Atômico 33, Áudio 33)
  } else if (eixoY == 0) {
    myDFPlayer.play(51); // Navegação por grupo: vai para Telúrio (Número Atômico 52, Áudio 51)
  } else if (eixoY == 1022) {
    myDFPlayer.play(26); // Retorna para Manganês (Número Atômico 25, Áudio 26)
  }
  break;

case 35: // Áudio 35 - Bromo (Número Atômico 35)
  if (eixoX == 0) {
    myDFPlayer.play(36); // Avança para Criptônio (Número Atômico 36, Áudio 36)
  } else if (eixoX == 1022) {
    myDFPlayer.play(34); // Retorna para Selênio (Número Atômico 34, Áudio 34)
  } else if (eixoY == 0) {
    myDFPlayer.play(52); // Navegação por grupo: vai para Iodo (Número Atômico 53, Áudio 52)
  } else if (eixoY == 1022) {
    myDFPlayer.play(27); // Retorna para Ferro (Número Atômico 26, Áudio 27)
  }
  break;

case 36: // Áudio 36 - Criptônio (Número Atômico 36)
  if (eixoX == 0) {
    myDFPlayer.play(19); // Retorna para Argônio (Número Atômico 18, Áudio 19)
  } else if (eixoX == 1022) {
    myDFPlayer.play(35); // Retorna para Bromo (Número Atômico 35, Áudio 35)
  } else if (eixoY == 0) {
    myDFPlayer.play(53); // Navegação por grupo: vai para Xenônio (Número Atômico 54, Áudio 53)
  } else if (eixoY == 1022) {
    myDFPlayer.play(28); // Retorna para Cobalto (Número Atômico 27, Áudio 28)
  }
  break;

case 37: // Áudio 37 - Rubídio (Número Atômico 37)
  if (eixoX == 0) {
    myDFPlayer.play(38); // Avança para Estrôncio (Número Atômico 38, Áudio 38)
  } else if (eixoX == 1022) {
    myDFPlayer.play(20); // Retorna para Potássio (Número Atômico 19, Áudio 20)
  } else if (eixoY == 0) {
    myDFPlayer.play(55); // Navegação por grupo: vai para Césio (Número Atômico 55, Áudio 55)
  } else if (eixoY == 1022) {
    myDFPlayer.play(12); // Retorna para Sódio (Número Atômico 11, Áudio 12)
  }
  break;

case 38: // Áudio 38 - Estrôncio (Número Atômico 38)
  if (eixoX == 0) {
    myDFPlayer.play(39); // Avança para Ítrio (Número Atômico 39, Áudio 39)
  } else if (eixoX == 1022) {
    myDFPlayer.play(37); // Retorna para Rubídio (Número Atômico 37, Áudio 37)
  } else if (eixoY == 0) {
    myDFPlayer.play(56); // Navegação por grupo: vai para Bário (Número Atômico 56, Áudio 56)
  } else if (eixoY == 1022) {
    myDFPlayer.play(13); // Retorna para Magnésio (Número Atômico 12, Áudio 13)
  }
  break;

case 39: // Áudio 39 - Ítrio (Número Atômico 39)
  if (eixoX == 0) {
    myDFPlayer.play(40); // Avança para Zircônio (Número Atômico 40, Áudio 40)
  } else if (eixoX == 1022) {
    myDFPlayer.play(38); // Retorna para Estrôncio (Número Atômico 38, Áudio 38)
  } else if (eixoY == 0) {
    myDFPlayer.play(57); // Navegação por grupo: vai para Lantânio (Número Atômico 57, Áudio 57)
  } else if (eixoY == 1022) {
    myDFPlayer.play(14); // Retorna para Alumínio (Número Atômico 13, Áudio 14)
  }
  break;

case 40: // Áudio 40 - Zircônio (Número Atômico 40)
  if (eixoX == 0) {
    myDFPlayer.play(41); // Avança para Nióbio (Número Atômico 41, Áudio 41)
  } else if (eixoX == 1022) {
    myDFPlayer.play(39); // Retorna para Ítrio (Número Atômico 39, Áudio 39)
  } else if (eixoY == 0) {
    myDFPlayer.play(58); // Navegação por grupo: vai para Háfnio (Número Atômico 58, Áudio 58)
  } else if (eixoY == 1022) {
    myDFPlayer.play(15); // Retorna para Silício (Número Atômico 14, Áudio 15)
  }
  break;

case 41: // Áudio 41 - Nióbio (Número Atômico 41)
  if (eixoX == 0) {
    myDFPlayer.play(42); // Avança para Molibdênio (Número Atômico 42, Áudio 42)
  } else if (eixoX == 1022) {
    myDFPlayer.play(40); // Retorna para Zircônio (Número Atômico 40, Áudio 40)
  } else if (eixoY == 0) {
    myDFPlayer.play(59); // Navegação por grupo: vai para Tântalo (Número Atômico 59, Áudio 59)
  } else if (eixoY == 1022) {
    myDFPlayer.play(16); // Retorna para Fósforo (Número Atômico 15, Áudio 16)
  }
  break;

case 42: // Áudio 42 - Molibdênio (Número Atômico 42)
  if (eixoX == 0) {
    myDFPlayer.play(43); // Avança para Tecnécio (Número Atômico 43, Áudio 43)
  } else if (eixoX == 1022) {
    myDFPlayer.play(41); // Retorna para Nióbio (Número Atômico 41, Áudio 41)
  } else if (eixoY == 0) {
    myDFPlayer.play(60); // Navegação por grupo: vai para Tungstênio (Número Atômico 60, Áudio 60)
  } else if (eixoY == 1022) {
    myDFPlayer.play(17); // Retorna para Enxofre (Número Atômico 16, Áudio 17)
  }
  break;

case 43: // Áudio 43 - Tecnécio (Número Atômico 43)
  if (eixoX == 0) {
    myDFPlayer.play(44); // Avança para Rutênio (Número Atômico 44, Áudio 44)
  } else if (eixoX == 1022) {
    myDFPlayer.play(42); // Retorna para Molibdênio (Número Atômico 42, Áudio 42)
  } else if (eixoY == 0) {
    myDFPlayer.play(61); // Navegação por grupo: vai para Rênio (Número Atômico 61, Áudio 61)
  } else if (eixoY == 1022) {
    myDFPlayer.play(18); // Retorna para Cloro (Número Atômico 17, Áudio 18)
  }
  break;

case 44: // Áudio 44 - Rutênio (Número Atômico 44)
  if (eixoX == 0) {
    myDFPlayer.play(45); // Avança para Ródio (Número Atômico 45, Áudio 45)
  } else if (eixoX == 1022) {
    myDFPlayer.play(43); // Retorna para Tecnécio (Número Atômico 43, Áudio 43)
  } else if (eixoY == 0) {
    myDFPlayer.play(62); // Navegação por grupo: vai para Ósmio (Número Atômico 62, Áudio 62)
  } else if (eixoY == 1022) {
    myDFPlayer.play(19); // Retorna para Argônio (Número Atômico 18, Áudio 19)
  }
  break;

case 45: // Áudio 45 - Ródio (Número Atômico 45)
  if (eixoX == 0) {
    myDFPlayer.play(46); // Avança para Paládio (Número Atômico 46, Áudio 46)
  } else if (eixoX == 1022) {
    myDFPlayer.play(44); // Retorna para Rutênio (Número Atômico 44, Áudio 44)
  } else if (eixoY == 0) {
    myDFPlayer.play(102); // Navegação por grupo: vai para Irídio (Número Atômico 77, Áudio 102)
  } else if (eixoY == 1022) {
    myDFPlayer.play(28); // Retorna para Níquel (Número Atômico 28, Áudio 29)
  }
  break;

case 46: // Áudio 46 - Paládio (Número Atômico 46)
  if (eixoX == 0) {
    myDFPlayer.play(47); // Avança para Prata (Número Atômico 47, Áudio 47)
  } else if (eixoX == 1022) {
    myDFPlayer.play(45); // Retorna para Ródio (Número Atômico 45, Áudio 45)
  } else if (eixoY == 0) {
    myDFPlayer.play(104); // Navegação por grupo: vai para Platina (Número Atômico 78, Áudio 104)
  } else if (eixoY == 1022) {
    myDFPlayer.play(29); // Retorna para Níquel (Número Atômico 28, Áudio 29)
  }
  break;

case 47: // Áudio 47 - Prata (Número Atômico 47)
  if (eixoX == 0) {
    myDFPlayer.play(48); // Avança para Cádmio (Número Atômico 48, Áudio 48)
  } else if (eixoX == 1022) {
    myDFPlayer.play(46); // Retorna para Paládio (Número Atômico 46, Áudio 46)
  } else if (eixoY == 0) {
    myDFPlayer.play(106); // Navegação por grupo: vai para Ouro (Número Atômico 79, Áudio 106)
  } else if (eixoY == 1022) {
    myDFPlayer.play(30); // Retorna para Cobre (Número Atômico 29, Áudio 30)
  }
  break;

case 48: // Áudio 48 - Cádmio (Número Atômico 48)
  if (eixoX == 0) {
    myDFPlayer.play(49); // Avança para Índio (Número Atômico 49, Áudio 49)
  } else if (eixoX == 1022) {
    myDFPlayer.play(47); // Retorna para Prata (Número Atômico 47, Áudio 47)
  } else if (eixoY == 0) {
    myDFPlayer.play(108); // Navegação por grupo: vai para Mercúrio (Número Atômico 80, Áudio 108)
  } else if (eixoY == 1022) {
    myDFPlayer.play(31); // Retorna para Gálio (Número Atômico 31, Áudio 31)
  }
  break;

case 49: // Áudio 49 - Índio (Número Atômico 49)
  if (eixoX == 0) {
    myDFPlayer.play(50); // Avança para Estanho (Número Atômico 50, Áudio 50)
  } else if (eixoX == 1022) {
    myDFPlayer.play(48); // Retorna para Cádmio (Número Atômico 48, Áudio 48)
  } else if (eixoY == 0) {
    myDFPlayer.play(110); // Navegação por grupo: vai para Tálio (Número Atômico 81, Áudio 110)
  } else if (eixoY == 1022) {
    myDFPlayer.play(32); // Retorna para Germânio (Número Atômico 32, Áudio 32)
  }
  break;

case 50: // Áudio 50 - Estanho (Número Atômico 50)
  if (eixoX == 0) {
    myDFPlayer.play(51); // Avança para Antimônio (Número Atômico 51, Áudio 51)
  } else if (eixoX == 1022) {
    myDFPlayer.play(49); // Retorna para Índio (Número Atômico 49, Áudio 49)
  } else if (eixoY == 0) {
    myDFPlayer.play(112); // Navegação por grupo: vai para Chumbo (Número Atômico 82, Áudio 112)
  } else if (eixoY == 1022) {
    myDFPlayer.play(33); // Retorna para Arsênio (Número Atômico 33, Áudio 33)
  }
  break;

case 51: // Áudio 51 - Antimônio (Número Atômico 51)
  if (eixoX == 0) {
    myDFPlayer.play(52); // Avança para Telúrio (Número Atômico 52, Áudio 52)
  } else if (eixoX == 1022) {
    myDFPlayer.play(50); // Retorna para Estanho (Número Atômico 50, Áudio 50)
  } else if (eixoY == 0) {
    myDFPlayer.play(114); // Navegação por grupo: vai para Bismuto (Número Atômico 83, Áudio 114)
  } else if (eixoY == 1022) {
    myDFPlayer.play(34); // Retorna para Selênio (Número Atômico 34, Áudio 34)
  }
  break;

case 52: // Áudio 52 - Telúrio (Número Atômico 52)
  if (eixoX == 0) {
    myDFPlayer.play(53); // Avança para Iodo (Número Atômico 53, Áudio 53)
  } else if (eixoX == 1022) {
    myDFPlayer.play(51); // Retorna para Antimônio (Número Atômico 51, Áudio 51)
  } else if (eixoY == 0) {
    myDFPlayer.play(116); // Navegação por grupo: vai para Polônio (Número Atômico 84, Áudio 116)
  } else if (eixoY == 1022) {
    myDFPlayer.play(35); // Retorna para Bromo (Número Atômico 35, Áudio 35)
  }
  break;

case 53: // Áudio 53 - Iodo (Número Atômico 53)
  if (eixoX == 0) {
    myDFPlayer.play(54); // Avança para Xenônio (Número Atômico 54, Áudio 54)
  } else if (eixoX == 1022) {
    myDFPlayer.play(52); // Retorna para Telúrio (Número Atômico 52, Áudio 52)
  } else if (eixoY == 0) {
    myDFPlayer.play(118); // Navegação por grupo: vai para Astato (Número Atômico 85, Áudio 118)
  } else if (eixoY == 1022) {
    myDFPlayer.play(36); // Retorna para Criptônio (Número Atômico 36, Áudio 36)
  }
  break;

case 54: // Áudio 54 - Xenônio (Número Atômico 54)
  if (eixoX == 0) {
    myDFPlayer.play(55); // Avança para Césio (Número Atômico 55, Áudio 55)
  } else if (eixoX == 1022) {
    myDFPlayer.play(53); // Retorna para Iodo (Número Atômico 53, Áudio 53)
  } else if (eixoY == 0) {
    myDFPlayer.play(86); // Navegação por grupo: vai para Radônio (Número Atômico 86, Áudio 86)
  } else if (eixoY == 1022) {
    myDFPlayer.play(36); // Retorna para Criptônio (Número Atômico 36, Áudio 36)
  }
  break;

case 55: // Áudio 55 - Césio (Número Atômico 55)
  if (eixoX == 0) {
    myDFPlayer.play(56); // Avança para Bário (Número Atômico 56, Áudio 56)
  } else if (eixoX == 1022) {
    myDFPlayer.play(54); // Retorna para Xenônio (Número Atômico 54, Áudio 54)
  } else if (eixoY == 0) {
    myDFPlayer.play(87); // Navegação por grupo: vai para Frâncio (Número Atômico 87, Áudio 87)
  } else if (eixoY == 1022) {
    myDFPlayer.play(37); // Retorna para Rubídio (Número Atômico 37, Áudio 37)
  }
  break;

case 56: // Áudio 56 - Bário (Número Atômico 56)
  if (eixoX == 0) {
    myDFPlayer.play(57); // Avança para Lantânio (Número Atômico 57, Áudio 57)
  } else if (eixoX == 1022) {
    myDFPlayer.play(55); // Retorna para Césio (Número Atômico 55, Áudio 55)
  } else if (eixoY == 0) {
    myDFPlayer.play(88); // Navegação por grupo: vai para Rádio (Número Atômico 88, Áudio 88)
  } else if (eixoY == 1022) {
    myDFPlayer.play(38); // Retorna para Estrôncio (Número Atômico 38, Áudio 38)
  }
  break;

case 57: // Áudio 57 - Lantânio (Número Atômico 57)
  if (eixoX == 0) {
    myDFPlayer.play(58); // Avança para Cério (Número Atômico 58, Áudio 58)
  } else if (eixoX == 1022) {
    myDFPlayer.play(56); // Retorna para Bário (Número Atômico 56, Áudio 56)
  } else if (eixoY == 0) {
    myDFPlayer.play(89); // Navegação por grupo: vai para Actínio (Número Atômico 89, Áudio 89)
  } else if (eixoY == 1022) {
    myDFPlayer.play(39); // Retorna para Ítrio (Número Atômico 39, Áudio 39)
  }
  break;

case 58: // Áudio 58 - Cério (Número Atômico 58)
  if (eixoX == 0) {
    myDFPlayer.play(59); // Avança para Praseodímio (Número Atômico 59, Áudio 59)
  } else if (eixoX == 1022) {
    myDFPlayer.play(57); // Retorna para Lantânio (Número Atômico 57, Áudio 57)
  } else if (eixoY == 0) {
    myDFPlayer.play(90); // Navegação por grupo: vai para Tório (Número Atômico 90, Áudio 90)
  } else if (eixoY == 1022) {
    myDFPlayer.play(40); // Retorna para Zircônio (Número Atômico 40, Áudio 40)
  }
  break;

case 59: // Áudio 59 - Praseodímio (Número Atômico 59)
  if (eixoX == 0) {
    myDFPlayer.play(60); // Avança para Neodímio (Número Atômico 60, Áudio 60)
  } else if (eixoX == 1022) {
    myDFPlayer.play(58); // Retorna para Cério (Número Atômico 58, Áudio 58)
  } else if (eixoY == 0) {
    myDFPlayer.play(91); // Navegação por grupo: vai para Protactínio (Número Atômico 91, Áudio 91)
  } else if (eixoY == 1022) {
    myDFPlayer.play(41); // Retorna para Nióbio (Número Atômico 41, Áudio 41)
  }
  break;

case 60: // Áudio 60 - Neodímio (Número Atômico 60)
  if (eixoX == 0) {
    myDFPlayer.play(61); // Avança para Promécio (Número Atômico 61, Áudio 61)
  } else if (eixoX == 1022) {
    myDFPlayer.play(59); // Retorna para Praseodímio (Número Atômico 59, Áudio 59)
  } else if (eixoY == 0) {
    myDFPlayer.play(92); // Navegação por grupo: vai para Urânio (Número Atômico 92, Áudio 92)
  } else if (eixoY == 1022) {
    myDFPlayer.play(42); // Retorna para Molibdênio (Número Atômico 42, Áudio 42)
  }
  break;

case 61: // Áudio 61 - Promécio (Número Atômico 61)
  if (eixoX == 0) {
    myDFPlayer.play(62); // Avança para Samário (Número Atômico 62, Áudio 62)
  } else if (eixoX == 1022) {
    myDFPlayer.play(60); // Retorna para Neodímio (Número Atômico 60, Áudio 60)
  } else if (eixoY == 0) {
    myDFPlayer.play(93); // Navegação por grupo: vai para Netúnio (Número Atômico 93, Áudio 93)
  } else if (eixoY == 1022) {
    myDFPlayer.play(43); // Retorna para Tecnécio (Número Atômico 43, Áudio 43)
  }
  break;

case 62: // Áudio 62 - Samário (Número Atômico 62)
  if (eixoX == 0) {
    myDFPlayer.play(63); // Avança para Európio (Número Atômico 63, Áudio 63)
  } else if (eixoX == 1022) {
    myDFPlayer.play(61); // Retorna para Promécio (Número Atômico 61, Áudio 61)
  } else if (eixoY == 0) {
    myDFPlayer.play(94); // Navegação por grupo: vai para Plutônio (Número Atômico 94, Áudio 94)
  } else if (eixoY == 1022) {
    myDFPlayer.play(44); // Retorna para Rutênio (Número Atômico 44, Áudio 44)
  }
  break;

case 63: // Áudio 63 - Európio (Número Atômico 63)
  if (eixoX == 0) {
    myDFPlayer.play(64); // Avança para Gadolínio (Número Atômico 64, Áudio 64)
  } else if (eixoX == 1022) {
    myDFPlayer.play(62); // Retorna para Samário (Número Atômico 62, Áudio 62)
  } else if (eixoY == 0) {
    myDFPlayer.play(95); // Navegação por grupo: vai para Amerício (Número Atômico 95, Áudio 95)
  } else if (eixoY == 1022) {
    myDFPlayer.play(45); // Retorna para Ródio (Número Atômico 45, Áudio 45)
  }
  break;

case 64: // Áudio 64 - Gadolínio (Número Atômico 64)
  if (eixoX == 0) {
    myDFPlayer.play(65); // Avança para Térbio (Número Atômico 65, Áudio 65)
  } else if (eixoX == 1022) {
    myDFPlayer.play(63); // Retorna para Európio (Número Atômico 63, Áudio 63)
  } else if (eixoY == 0) {
    myDFPlayer.play(96); // Navegação por grupo: vai para Cúrio (Número Atômico 96, Áudio 96)
  } else if (eixoY == 1022) {
    myDFPlayer.play(46); // Retorna para Paládio (Número Atômico 46, Áudio 46)
  }
  break;

case 65: // Áudio 65 - Térbio (Número Atômico 65)
  if (eixoX == 0) {
    myDFPlayer.play(66); // Avança para Disprósio (Número Atômico 66, Áudio 66)
  } else if (eixoX == 1022) {
    myDFPlayer.play(64); // Retorna para Gadolínio (Número Atômico 64, Áudio 64)
  } else if (eixoY == 0) {
    myDFPlayer.play(97); // Navegação por grupo: vai para Berquélio (Número Atômico 97, Áudio 97)
  } else if (eixoY == 1022) {
    myDFPlayer.play(47); // Retorna para Prata (Número Atômico 47, Áudio 47)
  }
  break;

case 66: // Áudio 66 - Disprósio (Número Atômico 66)
  if (eixoX == 0) {
    myDFPlayer.play(67); // Avança para Hólmio (Número Atômico 67, Áudio 67)
  } else if (eixoX == 1022) {
    myDFPlayer.play(65); // Retorna para Térbio (Número Atômico 65, Áudio 65)
  } else if (eixoY == 0) {
    myDFPlayer.play(98); // Navegação por grupo: vai para Califórnio (Número Atômico 98, Áudio 98)
  } else if (eixoY == 1022) {
    myDFPlayer.play(48); // Retorna para Cádmio (Número Atômico 48, Áudio 48)
  }
  break;

case 67: // Áudio 67 - Hólmio (Número Atômico 67)
  if (eixoX == 0) {
    myDFPlayer.play(68); // Avança para Érbio (Número Atômico 68, Áudio 68)
  } else if (eixoX == 1022) {
    myDFPlayer.play(66); // Retorna para Disprósio (Número Atômico 66, Áudio 66)
  } else if (eixoY == 0) {
    myDFPlayer.play(99); // Navegação por grupo: vai para Einstênio (Número Atômico 99, Áudio 99)
  } else if (eixoY == 1022) {
    myDFPlayer.play(49); // Retorna para Índio (Número Atômico 49, Áudio 49)
  }
  break;

case 68: // Áudio 68 - Érbio (Número Atômico 68)
  if (eixoX == 0) {
    myDFPlayer.play(69); // Avança para Túlio (Número Atômico 69, Áudio 69)
  } else if (eixoX == 1022) {
    myDFPlayer.play(67); // Retorna para Hólmio (Número Atômico 67, Áudio 67)
  } else if (eixoY == 0) {
    myDFPlayer.play(100); // Navegação por grupo: vai para Férmio (Número Atômico 100, Áudio 100)
  } else if (eixoY == 1022) {
    myDFPlayer.play(50); // Retorna para Estanho (Número Atômico 50, Áudio 50)
  }
  break;

case 69: // Áudio 69 - Túlio (Número Atômico 69)
  if (eixoX == 0) {
    myDFPlayer.play(70); // Avança para Itérbio (Número Atômico 70, Áudio 70)
  } else if (eixoX == 1022) {
    myDFPlayer.play(68); // Retorna para Érbio (Número Atômico 68, Áudio 68)
  } else if (eixoY == 0) {
    myDFPlayer.play(101); // Navegação por grupo: vai para Mendelévio (Número Atômico 101, Áudio 101)
  } else if (eixoY == 1022) {
    myDFPlayer.play(51); // Retorna para Antimônio (Número Atômico 51, Áudio 51)
  }
  break;

case 70: // Áudio 70 - Itérbio (Número Atômico 70)
  if (eixoX == 0) {
    myDFPlayer.play(71); // Avança para Lutécio (Número Atômico 71, Áudio 71)
  } else if (eixoX == 1022) {
    myDFPlayer.play(69); // Retorna para Túlio (Número Atômico 69, Áudio 69)
  } else if (eixoY == 0) {
    myDFPlayer.play(102); // Navegação por grupo: vai para Nobelio (Número Atômico 102, Áudio 102)
  } else if (eixoY == 1022) {
    myDFPlayer.play(52); // Retorna para Telúrio (Número Atômico 52, Áudio 52)
  }
  break;

case 71: // Áudio 71 - Lutécio (Número Atômico 71)
  if (eixoX == 0) {
    myDFPlayer.play(72); // Avança para Háfnio (Número Atômico 72, Áudio 72)
  } else if (eixoX == 1022) {
    myDFPlayer.play(70); // Retorna para Itérbio (Número Atômico 70, Áudio 70)
  } else if (eixoY == 0) {
    myDFPlayer.play(103); // Navegação por grupo: vai para Laurêncio (Número Atômico 103, Áudio 103)
  } else if (eixoY == 1022) {
    myDFPlayer.play(53); // Retorna para Iodo (Número Atômico 53, Áudio 53)
  }
  break;

case 72: // Áudio 72 - Háfnio (Número Atômico 72)
  if (eixoX == 0) {
    myDFPlayer.play(73); // Avança para Tântalo (Número Atômico 73, Áudio 73)
  } else if (eixoX == 1022) {
    myDFPlayer.play(71); // Retorna para Lutécio (Número Atômico 71, Áudio 71)
  } else if (eixoY == 0) {
    myDFPlayer.play(104); // Navegação por grupo: vai para Rutherfordium (Número Atômico 104, Áudio 104)
  } else if (eixoY == 1022) {
    myDFPlayer.play(54); // Retorna para Xenônio (Número Atômico 54, Áudio 54)
  }
  break;

case 73: // Áudio 73 - Tântalo (Número Atômico 73)
  if (eixoX == 0) {
    myDFPlayer.play(74); // Avança para Tungstênio (Número Atômico 74, Áudio 74)
  } else if (eixoX == 1022) {
    myDFPlayer.play(72); // Retorna para Háfnio (Número Atômico 72, Áudio 72)
  } else if (eixoY == 0) {
    myDFPlayer.play(105); // Navegação por grupo: vai para Dúbnio (Número Atômico 105, Áudio 105)
  } else if (eixoY == 1022) {
    myDFPlayer.play(55); // Retorna para Césio (Número Atômico 55, Áudio 55)
  }
  break;

case 74: // Áudio 74 - Tungstênio (Número Atômico 74)
  if (eixoX == 0) {
    myDFPlayer.play(75); // Avança para Rênio (Número Atômico 75, Áudio 75)
  } else if (eixoX == 1022) {
    myDFPlayer.play(73); // Retorna para Tântalo (Número Atômico 73, Áudio 73)
  } else if (eixoY == 0) {
    myDFPlayer.play(106); // Navegação por grupo: vai para Seabórgio (Número Atômico 106, Áudio 106)
  } else if (eixoY == 1022) {
    myDFPlayer.play(56); // Retorna para Bário (Número Atômico 56, Áudio 56)
  }
  break;

case 75: // Áudio 75 - Rênio (Número Atômico 75)
  if (eixoX == 0) {
    myDFPlayer.play(76); // Avança para Ósmio (Número Atômico 76, Áudio 76)
  } else if (eixoX == 1022) {
    myDFPlayer.play(74); // Retorna para Tungstênio (Número Atômico 74, Áudio 74)
  } else if (eixoY == 0) {
    myDFPlayer.play(107); // Navegação por grupo: vai para Bóhrio (Número Atômico 107, Áudio 107)
  } else if (eixoY == 1022) {
    myDFPlayer.play(57); // Retorna para Lantânio (Número Atômico 57, Áudio 57)
  }
  break;

case 76: // Áudio 76 - Ósmio (Número Atômico 76)
  if (eixoX == 0) {
    myDFPlayer.play(77); // Avança para Irídio (Número Atômico 77, Áudio 77)
  } else if (eixoX == 1022) {
    myDFPlayer.play(75); // Retorna para Rênio (Número Atômico 75, Áudio 75)
  } else if (eixoY == 0) {
    myDFPlayer.play(108); // Navegação por grupo: vai para Hássio (Número Atômico 108, Áudio 108)
  } else if (eixoY == 1022) {
    myDFPlayer.play(58); // Retorna para Cério (Número Atômico 58, Áudio 58)
  }
  break;

case 77: // Áudio 77 - Irídio (Número Atômico 77)
  if (eixoX == 0) {
    myDFPlayer.play(78); // Avança para Platina (Número Atômico 78, Áudio 78)
  } else if (eixoX == 1022) {
    myDFPlayer.play(76); // Retorna para Ósmio (Número Atômico 76, Áudio 76)
  } else if (eixoY == 0) {
    myDFPlayer.play(109); // Navegação por grupo: vai para Meitnério (Número Atômico 109, Áudio 109)
  } else if (eixoY == 1022) {
    myDFPlayer.play(59); // Retorna para Praseodímio (Número Atômico 59, Áudio 59)
  }
  break;

case 78: // Áudio 78 - Platina (Número Atômico 78)
  if (eixoX == 0) {
    myDFPlayer.play(79); // Avança para Ouro (Número Atômico 79, Áudio 79)
  } else if (eixoX == 1022) {
    myDFPlayer.play(77); // Retorna para Irídio (Número Atômico 77, Áudio 77)
  } else if (eixoY == 0) {
    myDFPlayer.play(110); // Navegação por grupo: vai para Darmstádio (Número Atômico 110, Áudio 110)
  } else if (eixoY == 1022) {
    myDFPlayer.play(60); // Retorna para Neodímio (Número Atômico 60, Áudio 60)
  }
  break;

case 79: // Áudio 79 - Ouro (Número Atômico 79)
  if (eixoX == 0) {
    myDFPlayer.play(80); // Avança para Mercúrio (Número Atômico 80, Áudio 80)
  } else if (eixoX == 1022) {
    myDFPlayer.play(78); // Retorna para Platina (Número Atômico 78, Áudio 78)
  } else if (eixoY == 0) {
    myDFPlayer.play(111); // Navegação por grupo: vai para Roentgênio (Número Atômico 111, Áudio 111)
  } else if (eixoY == 1022) {
    myDFPlayer.play(61); // Retorna para Promécio (Número Atômico 61, Áudio 61)
  }
  break;

case 80: // Áudio 80 - Mercúrio (Número Atômico 80)
  if (eixoX == 0) {
    myDFPlayer.play(81); // Avança para Tálio (Número Atômico 81, Áudio 81)
  } else if (eixoX == 1022) {
    myDFPlayer.play(79); // Retorna para Ouro (Número Atômico 79, Áudio 79)
  } else if (eixoY == 0) {
    myDFPlayer.play(112); // Navegação por grupo: vai para Copernício (Número Atômico 112, Áudio 112)
  } else if (eixoY == 1022) {
    myDFPlayer.play(62); // Retorna para Samário (Número Atômico 62, Áudio 62)
  }
  break;

case 81: // Áudio 81 - Tálio (Número Atômico 81)
  if (eixoX == 0) {
    myDFPlayer.play(82); // Avança para Chumbo (Número Atômico 82, Áudio 82)
  } else if (eixoX == 1022) {
    myDFPlayer.play(80); // Retorna para Mercúrio (Número Atômico 80, Áudio 80)
  } else if (eixoY == 0) {
    myDFPlayer.play(113); // Navegação por grupo: vai para Nihônio (Número Atômico 113, Áudio 113)
  } else if (eixoY == 1022) {
    myDFPlayer.play(63); // Retorna para Európio (Número Atômico 63, Áudio 63)
  }
  break;

case 82: // Áudio 82 - Chumbo (Número Atômico 82)
  if (eixoX == 0) {
    myDFPlayer.play(83); // Avança para Bismuto (Número Atômico 83, Áudio 83)
  } else if (eixoX == 1022) {
    myDFPlayer.play(81); // Retorna para Tálio (Número Atômico 81, Áudio 81)
  } else if (eixoY == 0) {
    myDFPlayer.play(114); // Navegação por grupo: vai para Fleróvio (Número Atômico 114, Áudio 114)
  } else if (eixoY == 1022) {
    myDFPlayer.play(51); // Retorna para Estanho (Número Atômico 50, Áudio 51)
  }
  break;

case 83: // Áudio 83 - Bismuto (Número Atômico 83)
  if (eixoX == 0) {
    myDFPlayer.play(84); // Avança para Polônio (Número Atômico 84, Áudio 84)
  } else if (eixoX == 1022) {
    myDFPlayer.play(82); // Retorna para Chumbo (Número Atômico 82, Áudio 82)
  } else if (eixoY == 0) {
    myDFPlayer.play(115); // Navegação por grupo: vai para Moscóvio (Número Atômico 115, Áudio 115)
  } else if (eixoY == 1022) {
    myDFPlayer.play(52); // Retorna para Antimônio (Número Atômico 51, Áudio 52)
  }
  break;

case 84: // Áudio 84 - Polônio (Número Atômico 84)
  if (eixoX == 0) {
    myDFPlayer.play(85); // Avança para Astato (Número Atômico 85, Áudio 85)
  } else if (eixoX == 1022) {
    myDFPlayer.play(83); // Retorna para Bismuto (Número Atômico 83, Áudio 83)
  } else if (eixoY == 0) {
    myDFPlayer.play(116); // Navegação por grupo: vai para Livermório (Número Atômico 116, Áudio 116)
  } else if (eixoY == 1022) {
    myDFPlayer.play(53); // Retorna para Telúrio (Número Atômico 52, Áudio 53)
  }
  break;

case 85: // Áudio 85 - Astato (Número Atômico 85)
  if (eixoX == 0) {
    myDFPlayer.play(86); // Avança para Radônio (Número Atômico 86, Áudio 86)
  } else if (eixoX == 1022) {
    myDFPlayer.play(84); // Retorna para Polônio (Número Atômico 84, Áudio 84)
  } else if (eixoY == 0) {
    myDFPlayer.play(117); // Navegação por grupo: vai para Tenessino (Número Atômico 117, Áudio 117)
  } else if (eixoY == 1022) {
    myDFPlayer.play(54); // Retorna para Iodo (Número Atômico 53, Áudio 54)
  }
  break;

case 86: // Áudio 86 - Radônio (Número Atômico 86)
  if (eixoX == 0) {
    myDFPlayer.play(87); // Avança para Frâncio (Número Atômico 87, Áudio 87)
  } else if (eixoX == 1022) {
    myDFPlayer.play(85); // Retorna para Astato (Número Atômico 85, Áudio 85)
  } else if (eixoY == 0) {
    myDFPlayer.play(118); // Navegação por grupo: vai para Oganessônio (Número Atômico 118, Áudio 118)
  } else if (eixoY == 1022) {
    myDFPlayer.play(55); // Retorna para Xenônio (Número Atômico 54, Áudio 55)
  }
  break;

case 87: // Áudio 87 - Frâncio (Número Atômico 87)
  if (eixoX == 0) {
    myDFPlayer.play(88); // Avança para Rádio (Número Atômico 88, Áudio 88)
  } else if (eixoX == 1022) {
    myDFPlayer.play(86); // Retorna para Radônio (Número Atômico 86, Áudio 86)
  } else if (eixoY == 0) {
    myDFPlayer.play(103); // Navegação por grupo: vai para Laurêncio (Número Atômico 103, Áudio 103)
  } else if (eixoY == 1022) {
    myDFPlayer.play(56); // Retorna para Césio (Número Atômico 55, Áudio 56)
  }
  break;

case 88: // Áudio 88 - Rádio (Número Atômico 88)
  if (eixoX == 0) {
    myDFPlayer.play(89); // Avança para Actínio (Número Atômico 89, Áudio 89)
  } else if (eixoX == 1022) {
    myDFPlayer.play(87); // Retorna para Frâncio (Número Atômico 87, Áudio 87)
  } else if (eixoY == 0) {
    myDFPlayer.play(104); // Navegação por grupo: vai para Rutherfordium (Número Atômico 104, Áudio 104)
  } else if (eixoY == 1022) {
    myDFPlayer.play(57); // Retorna para Bário (Número Atômico 56, Áudio 57)
  }
  break;

case 89: // Áudio 89 - Actínio (Número Atômico 89)
  if (eixoX == 0) {
    myDFPlayer.play(90); // Avança para Tório (Número Atômico 90, Áudio 90)
  } else if (eixoX == 1022) {
    myDFPlayer.play(88); // Retorna para Rádio (Número Atômico 88, Áudio 88)
  } else if (eixoY == 0) {
    myDFPlayer.play(105); // Navegação por grupo: vai para Dúbnio (Número Atômico 105, Áudio 105)
  } else if (eixoY == 1022) {
    myDFPlayer.play(58); // Retorna para Lantânio (Número Atômico 57, Áudio 58)
  }
  break;

case 90: // Áudio 90 - Tório (Número Atômico 90)
  if (eixoX == 0) {
    myDFPlayer.play(91); // Avança para Protactínio (Número Atômico 91, Áudio 91)
  } else if (eixoX == 1022) {
    myDFPlayer.play(89); // Retorna para Actínio (Número Atômico 89, Áudio 89)
  } else if (eixoY == 0) {
    myDFPlayer.play(106); // Navegação por grupo: vai para Seabórgio (Número Atômico 106, Áudio 106)
  } else if (eixoY == 1022) {
    myDFPlayer.play(59); // Retorna para Cério (Número Atômico 58, Áudio 59)
  }
  break;

case 91: // Áudio 91 - Protactínio (Número Atômico 91)
  if (eixoX == 0) {
    myDFPlayer.play(92); // Avança para Urânio (Número Atômico 92, Áudio 92)
  } else if (eixoX == 1022) {
    myDFPlayer.play(90); // Retorna para Tório (Número Atômico 90, Áudio 90)
  } else if (eixoY == 0) {
    myDFPlayer.play(107); // Navegação por grupo: vai para Bóhrio (Número Atômico 107, Áudio 107)
  } else if (eixoY == 1022) {
    myDFPlayer.play(60); // Retorna para Praseodímio (Número Atômico 59, Áudio 59)
  }
  break;

case 92: // Áudio 92 - Urânio (Número Atômico 92)
  if (eixoX == 0) {
    myDFPlayer.play(93); // Avança para Netúnio (Número Atômico 93, Áudio 93)
  } else if (eixoX == 1022) {
    myDFPlayer.play(91); // Retorna para Protactínio (Número Atômico 91, Áudio 91)
  } else if (eixoY == 0) {
    myDFPlayer.play(108); // Navegação por grupo: vai para Hássio (Número Atômico 108, Áudio 108)
  } else if (eixoY == 1022) {
    myDFPlayer.play(61); // Retorna para Neodímio (Número Atômico 60, Áudio 60)
  }
  break;

case 93: // Áudio 93 - Netúnio (Número Atômico 93)
  if (eixoX == 0) {
    myDFPlayer.play(94); // Avança para Plutônio (Número Atômico 94, Áudio 94)
  } else if (eixoX == 1022) {
    myDFPlayer.play(92); // Retorna para Urânio (Número Atômico 92, Áudio 92)
  } else if (eixoY == 0) {
    myDFPlayer.play(109); // Navegação por grupo: vai para Meitnério (Número Atômico 109, Áudio 109)
  } else if (eixoY == 1022) {
    myDFPlayer.play(62); // Retorna para Promécio (Número Atômico 61, Áudio 61)
  }
  break;

case 94: // Áudio 94 - Plutônio (Número Atômico 94)
  if (eixoX == 0) {
    myDFPlayer.play(95); // Avança para Amerício (Número Atômico 95, Áudio 95)
  } else if (eixoX == 1022) {
    myDFPlayer.play(93); // Retorna para Netúnio (Número Atômico 93, Áudio 93)
  } else if (eixoY == 0) {
    myDFPlayer.play(110); // Navegação por grupo: vai para Darmstádio (Número Atômico 110, Áudio 110)
  } else if (eixoY == 1022) {
    myDFPlayer.play(63); // Retorna para Samário (Número Atômico 62, Áudio 62)
  }
  break;

case 95: // Áudio 95 - Amerício (Número Atômico 95)
  if (eixoX == 0) {
    myDFPlayer.play(96); // Avança para Cúrio (Número Atômico 96, Áudio 96)
  } else if (eixoX == 1022) {
    myDFPlayer.play(94); // Retorna para Plutônio (Número Atômico 94, Áudio 94)
  } else if (eixoY == 0) {
    myDFPlayer.play(111); // Navegação por grupo: vai para Roentgênio (Número Atômico 111, Áudio 111)
  } else if (eixoY == 1022) {
    myDFPlayer.play(64); // Retorna para Gadolínio (Número Atômico 63, Áudio 64)
  }
  break;

case 96: // Áudio 96 - Cúrio (Número Atômico 96)
  if (eixoX == 0) {
    myDFPlayer.play(97); // Avança para Berquélio (Número Atômico 97, Áudio 97)
  } else if (eixoX == 1022) {
    myDFPlayer.play(95); // Retorna para Amerício (Número Atômico 95, Áudio 95)
  } else if (eixoY == 0) {
    myDFPlayer.play(112); // Navegação por grupo: vai para Copernício (Número Atômico 112, Áudio 112)
  } else if (eixoY == 1022) {
    myDFPlayer.play(65); // Retorna para Térbio (Número Atômico 64, Áudio 65)
  }
  break;

case 97: // Áudio 97 - Berquélio (Número Atômico 97)
  if (eixoX == 0) {
    myDFPlayer.play(98); // Avança para Califórnio (Número Atômico 98, Áudio 98)
  } else if (eixoX == 1022) {
    myDFPlayer.play(96); // Retorna para Cúrio (Número Atômico 96, Áudio 96)
  } else if (eixoY == 0) {
    myDFPlayer.play(113); // Navegação por grupo: vai para Nihônio (Número Atômico 113, Áudio 113)
  } else if (eixoY == 1022) {
    myDFPlayer.play(66); // Retorna para Disprósio (Número Atômico 65, Áudio 66)
  }
  break;

case 98: // Áudio 98 - Califórnio (Número Atômico 98)
  if (eixoX == 0) {
    myDFPlayer.play(99); // Avança para Einstênio (Número Atômico 99, Áudio 99)
  } else if (eixoX == 1022) {
    myDFPlayer.play(97); // Retorna para Berquélio (Número Atômico 97, Áudio 97)
  } else if (eixoY == 0) {
    myDFPlayer.play(114); // Navegação por grupo: vai para Fleróvio (Número Atômico 114, Áudio 114)
  } else if (eixoY == 1022) {
    myDFPlayer.play(67); // Retorna para Hólmio (Número Atômico 66, Áudio 67)
  }
  break;

case 99: // Áudio 99 - Einstênio (Número Atômico 99)
  if (eixoX == 0) {
    myDFPlayer.play(100); // Avança para Férmio (Número Atômico 100, Áudio 100)
  } else if (eixoX == 1022) {
    myDFPlayer.play(98); // Retorna para Califórnio (Número Atômico 98, Áudio 98)
  } else if (eixoY == 0) {
    myDFPlayer.play(117); // Navegação por grupo: vai para Tenessino (Número Atômico 117, Áudio 117)
  } else if (eixoY == 1022) {
    myDFPlayer.play(66); // Retorna para Disprósio (Número Atômico 66, Áudio 66)
  }
  break;

case 100: // Áudio 100 - Férmio (Número Atômico 100)
  if (eixoX == 0) {
    myDFPlayer.play(101); // Avança para Mendelévio (Número Atômico 101, Áudio 101)
  } else if (eixoX == 1022) {
    myDFPlayer.play(99); // Retorna para Einstênio (Número Atômico 99, Áudio 99)
  } else if (eixoY == 0) {
    myDFPlayer.play(118); // Navegação por grupo: vai para Oganessônio (Número Atômico 118, Áudio 118)
  } else if (eixoY == 1022) {
    myDFPlayer.play(67); // Retorna para Hólmio (Número Atômico 67, Áudio 67)
  }
  break;

case 101: // Áudio 101 - Mendelévio (Número Atômico 101)
  if (eixoX == 0) {
    myDFPlayer.play(102); // Avança para Nobelio (Número Atômico 102, Áudio 102)
  } else if (eixoX == 1022) {
    myDFPlayer.play(100); // Retorna para Férmio (Número Atômico 100, Áudio 100)
  } else if (eixoY == 0) {
    myDFPlayer.play(103); // Navegação por grupo: vai para Laurêncio (Número Atômico 103, Áudio 103)
  } else if (eixoY == 1022) {
    myDFPlayer.play(68); // Retorna para Érbio (Número Atômico 68, Áudio 68)
  }
  break;

case 102: // Áudio 102 - Nobelio (Número Atômico 102)
  if (eixoX == 0) {
    myDFPlayer.play(103); // Avança para Laurêncio (Número Atômico 103, Áudio 103)
  } else if (eixoX == 1022) {
    myDFPlayer.play(101); // Retorna para Mendelévio (Número Atômico 101, Áudio 101)
  } else if (eixoY == 0) {
    myDFPlayer.play(104); // Navegação por grupo: vai para Rutherfordium (Número Atômico 104, Áudio 104)
  } else if (eixoY == 1022) {
    myDFPlayer.play(69); // Retorna para Túlio (Número Atômico 69, Áudio 69)
  }
  break;

case 103: // Áudio 103 - Laurêncio (Número Atômico 103)
  if (eixoX == 0) {
    myDFPlayer.play(104); // Avança para Rutherfordium (Número Atômico 104, Áudio 104)
  } else if (eixoX == 1022) {
    myDFPlayer.play(102); // Retorna para Nobelio (Número Atômico 102, Áudio 102)
  } else if (eixoY == 0) {
    myDFPlayer.play(105); // Navegação por grupo: vai para Dúbnio (Número Atômico 105, Áudio 105)
  } else if (eixoY == 1022) {
    myDFPlayer.play(70); // Retorna para Itérbio (Número Atômico 70, Áudio 70)
  }
  break;

case 104: // Áudio 104 - Rutherfordium (Número Atômico 104)
  if (eixoX == 0) {
    myDFPlayer.play(105); // Avança para Dúbnio (Número Atômico 105, Áudio 105)
  } else if (eixoX == 1022) {
    myDFPlayer.play(103); // Retorna para Laurêncio (Número Atômico 103, Áudio 103)
  } else if (eixoY == 0) {
    myDFPlayer.play(106); // Navegação por grupo: vai para Seabórgio (Número Atômico 106, Áudio 106)
  } else if (eixoY == 1022) {
    myDFPlayer.play(71); // Retorna para Lutécio (Número Atômico 71, Áudio 71)
  }
  break;

case 105: // Áudio 105 - Dúbnio (Número Atômico 105)
  if (eixoX == 0) {
    myDFPlayer.play(106); // Avança para Seabórgio (Número Atômico 106, Áudio 106)
  } else if (eixoX == 1022) {
    myDFPlayer.play(104); // Retorna para Rutherfordium (Número Atômico 104, Áudio 104)
  } else if (eixoY == 0) {
    myDFPlayer.play(107); // Navegação por grupo: vai para Bóhrio (Número Atômico 107, Áudio 107)
  } else if (eixoY == 1022) {
    myDFPlayer.play(72); // Retorna para Háfnio (Número Atômico 72, Áudio 72)
  }
  break;

case 106: // Áudio 106 - Seabórgio (Número Atômico 106)
  if (eixoX == 0) {
    myDFPlayer.play(107); // Avança para Bóhrio (Número Atômico 107, Áudio 107)
  } else if (eixoX == 1022) {
    myDFPlayer.play(105); // Retorna para Dúbnio (Número Atômico 105, Áudio 105)
  } else if (eixoY == 0) {
    myDFPlayer.play(108); // Navegação por grupo: vai para Hássio (Número Atômico 108, Áudio 108)
  } else if (eixoY == 1022) {
    myDFPlayer.play(73); // Retorna para Tântalo (Número Atômico 73, Áudio 73)
  }
  break;

case 107: // Áudio 107 - Bóhrio (Número Atômico 107)
  if (eixoX == 0) {
    myDFPlayer.play(108); // Avança para Hássio (Número Atômico 108, Áudio 108)
  } else if (eixoX == 1022) {
    myDFPlayer.play(106); // Retorna para Seabórgio (Número Atômico 106, Áudio 106)
  } else if (eixoY == 0) {
    myDFPlayer.play(109); // Navegação por grupo: vai para Meitnério (Número Atômico 109, Áudio 109)
  } else if (eixoY == 1022) {
    myDFPlayer.play(74); // Retorna para Wolfrâmio (Número Atômico 74, Áudio 74)
  }
  break;

case 108: // Áudio 108 - Hássio (Número Atômico 108)
  if (eixoX == 0) {
    myDFPlayer.play(109); // Avança para Mêitnério (Número Atômico 109, Áudio 109)
  } else if (eixoX == 1022) {
    myDFPlayer.play(107); // Retorna para Bohrio (Número Atômico 107, Áudio 107)
  } else if (eixoY == 0) {
    myDFPlayer.play(43); // Navegação por grupo: vai para Tecnécio (Número Atômico 43, Áudio 43)
  } else if (eixoY == 1022) {
    myDFPlayer.play(77); // Retorna para Irídio (Número Atômico 77, Áudio 77)
  }
  break;

case 109: // Áudio 109 - Mêitnério (Número Atômico 109)
  if (eixoX == 0) {
    myDFPlayer.play(110); // Avança para Darmstácio (Número Atômico 110, Áudio 110)
  } else if (eixoX == 1022) {
    myDFPlayer.play(108); // Retorna para Hássio (Número Atômico 108, Áudio 108)
  } else if (eixoY == 0) {
    myDFPlayer.play(44); // Navegação por grupo: vai para Rutênio (Número Atômico 44, Áudio 44)
  } else if (eixoY == 1022) {
    myDFPlayer.play(78); // Retorna para Platina (Número Atômico 78, Áudio 78)
  }
  break;

case 110: // Áudio 110 - Darmstácio (Número Atômico 110)
  if (eixoX == 0) {
    myDFPlayer.play(111); // Avança para Roentgênio (Número Atômico 111, Áudio 111)
  } else if (eixoX == 1022) {
    myDFPlayer.play(109); // Retorna para Mêitnério (Número Atômico 109, Áudio 109)
  } else if (eixoY == 0) {
    myDFPlayer.play(45); // Navegação por grupo: vai para Ródio (Número Atômico 45, Áudio 45)
  } else if (eixoY == 1022) {
    myDFPlayer.play(79); // Retorna para Ouro (Número Atômico 79, Áudio 79)
  }
  break;

case 111: // Áudio 111 - Roentgênio (Número Atômico 111)
  if (eixoX == 0) {
    myDFPlayer.play(112); // Avança para Copernício (Número Atômico 112, Áudio 112)
  } else if (eixoX == 1022) {
    myDFPlayer.play(110); // Retorna para Darmstácio (Número Atômico 110, Áudio 110)
  } else if (eixoY == 0) {
    myDFPlayer.play(46); // Navegação por grupo: vai para Paládio (Número Atômico 46, Áudio 46)
  } else if (eixoY == 1022) {
    myDFPlayer.play(80); // Retorna para Mercúrio (Número Atômico 80, Áudio 80)
  }
  break;

case 112: // Áudio 112 - Copernício (Número Atômico 112)
  if (eixoX == 0) {
    myDFPlayer.play(113); // Avança para Nihônio (Número Atômico 113, Áudio 113)
  } else if (eixoX == 1022) {
    myDFPlayer.play(111); // Retorna para Roentgênio (Número Atômico 111, Áudio 111)
  } else if (eixoY == 0) {
    myDFPlayer.play(47); // Navegação por grupo: vai para Prata (Número Atômico 47, Áudio 47)
  } else if (eixoY == 1022) {
    myDFPlayer.play(81); // Retorna para Tálio (Número Atômico 81, Áudio 81)
  }
  break;

case 113: // Áudio 113 - Nihônio (Número Atômico 113)
  if (eixoX == 0) {
    myDFPlayer.play(114); // Avança para Fleróvio (Número Atômico 114, Áudio 114)
  } else if (eixoX == 1022) {
    myDFPlayer.play(112); // Retorna para Copernício (Número Atômico 112, Áudio 112)
  } else if (eixoY == 0) {
    myDFPlayer.play(48); // Navegação por grupo: vai para Cádmio (Número Atômico 48, Áudio 48)
  } else if (eixoY == 1022) {
    myDFPlayer.play(82); // Retorna para Chumbo (Número Atômico 82, Áudio 82)
  }
  break;

case 114: // Áudio 114 - Fleróvio (Número Atômico 114)
  if (eixoX == 0) {
    myDFPlayer.play(115); // Avança para Moscóvio (Número Atômico 115, Áudio 115)
  } else if (eixoX == 1022) {
    myDFPlayer.play(113); // Retorna para Nihônio (Número Atômico 113, Áudio 113)
  } else if (eixoY == 0) {
    myDFPlayer.play(49); // Navegação por grupo: vai para Índio (Número Atômico 49, Áudio 49)
  } else if (eixoY == 1022) {
    myDFPlayer.play(83); // Retorna para Bismuto (Número Atômico 83, Áudio 83)
  }
  break;

case 115: // Áudio 115 - Moscóvio (Número Atômico 115)
  if (eixoX == 0) {
    myDFPlayer.play(116); // Avança para Livermório (Número Atômico 116, Áudio 116)
  } else if (eixoX == 1022) {
    myDFPlayer.play(114); // Retorna para Fleróvio (Número Atômico 114, Áudio 114)
  } else if (eixoY == 0) {
    myDFPlayer.play(50); // Navegação por grupo: vai para Estanho (Número Atômico 50, Áudio 50)
  } else if (eixoY == 1022) {
    myDFPlayer.play(84); // Retorna para Polônio (Número Atômico 84, Áudio 84)
  }
  break;

case 116: // Áudio 116 - Livermório (Número Atômico 116)
  if (eixoX == 0) {
    myDFPlayer.play(117); // Avança para Tenessino (Número Atômico 117, Áudio 117)
  } else if (eixoX == 1022) {
    myDFPlayer.play(115); // Retorna para Moscóvio (Número Atômico 115, Áudio 115)
  } else if (eixoY == 0) {
    myDFPlayer.play(51); // Navegação por grupo: vai para Antimônio (Número Atômico 51, Áudio 51)
  } else if (eixoY == 1022) {
    myDFPlayer.play(85); // Retorna para Astato (Número Atômico 85, Áudio 85)
  }
  break;

case 117: // Áudio 117 - Tenessino (Número Atômico 117)
  if (eixoX == 0) {
    myDFPlayer.play(118); // Avança para Oganessônio (Número Atômico 118, Áudio 118)
  } else if (eixoX == 1022) {
    myDFPlayer.play(116); // Retorna para Livermório (Número Atômico 116, Áudio 116)
  } else if (eixoY == 0) {
    myDFPlayer.play(52); // Navegação por grupo: vai para Telúrio (Número Atômico 52, Áudio 52)
  } else if (eixoY == 1022) {
    myDFPlayer.play(86); // Retorna para Radônio (Número Atômico 86, Áudio 86)
  }
  break;

case 118: // Áudio 118 - Oganessônio (Número Atômico 118)
  if (eixoX == 0) {
    myDFPlayer.play(1); // Volta para Hidrogênio (Número Atômico 1, Áudio 1)
  } else if (eixoX == 1022) {
    myDFPlayer.play(117); // Retorna para Tenessino (Número Atômico 117, Áudio 117)
  } else if (eixoY == 0) {
    myDFPlayer.play(53); // Navegação por grupo: vai para Iodo (Número Atômico 53, Áudio 53)
  } else if (eixoY == 1022) {
    myDFPlayer.play(87); // Retorna para Frâncio (Número Atômico 87, Áudio 87)
  }
  break;  
}
}

