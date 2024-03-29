//#include <Multiplexer.h>
//#include <EasyTransfer2.h>
//#include <LineFollow.h>
//#include <ArduinoRobotMotorBoard.h>

//PROGRAMA DE TESTES DO PLACAR
/*

  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>                         // Biblioteca Wire 
#include<SoftwareSerial.h>
#define syncTime 10000
#define tx 9
#define rx 10
#define pinledA 3
#define pinledB 2//mudei do pin 4 para o 12 green light
#define buttonInit 8//foi usado para teste no arduino da cesta o pin 2, no arduino do placar vai ser o 8

int pontuacaoA=0, pontuacaoB=0;
int cod = 48;//0
int estado=0, pres=0, tempo, qtdPlayers=0;
long tempoAtual=0, tempoInicioJogo;

enum nomeCodigos {ponto1, ponto2, pareando1, pareando2, comeco_jogo, fim_jogo, pareado1, pareado2};
int codigos[] = {1, 2, 3, 4, 5, 6, 9, 10};
SoftwareSerial HC12(tx,rx);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // set up the LCD's number of columns and rows:
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(buttonInit, INPUT);
  // Print a message to the LCD.
}


//vai esperar enviem algum dado, se receber então foi pareado
void search(){
  if(HC12.available()){
    cod=HC12.read()-48;
    if(cod == codigos[pareando1] || cod == codigos[pareando2] )
    qtdPlayers++;
    lcd.setCursor(0, 0);
    lcd.print("Jogador:"); lcd.print( (cod % 2) + 1); lcd.print("entrou");
    lcd.setCursor(0, 1);
    lcd.print(qtdPlayers); lcd.print(" players");
  }
}

int tempoJogo(){
  static int duracao[]={2, 3, 5, 10, 15}, it=-1;
  static const int tm = 5;
  (++it)%=tm;
  return duracao[it];
}

void gameTime(long tempo){
  long seg=-1, mn=0;
  if(tempo<60){
    lcd.setCursor(7, 1);
    lcd.print(tempo); 
  }
  mn = tempo/60;
  seg = tempo%60;
  lcd.setCursor(5, 1);
  lcd.print(mn);
  lcd.setCursor(7, 1);
  lcd.print(seg);
}

void loop() {
  switch(estado) {
    case 0: //WELLCOME 
      if(tempoAtual == 0) {
        lcd.setCursor(0, 0);
        lcd.print("Welcome to ");
        lcd.setCursor(0, 1);
        lcd.print("Basketball Game");
      }
      if(!digitalRead(buttonInit)){
        estado=1;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print("Waiting  Players");
        tempoAtual=millis();
      }
    break;
    case 1: //pareamento com jogadores
      if ( ((millis() - tempoAtual) <= syncTime) && (qtdPlayers<2) ) {
        search();
      }
      else {
        lcd.setCursor(0, 0);
        lcd.print("game time-pressB");
        lcd.setCursor(0,1);
        estado=2;
        tempoAtual=millis();
      }
    break;
    case 2: //duracao da partida
    //verifica se foi pressionado, sim->muda de estado, não->permanece e muda duração
      if(!digitalRead(buttonInit) && !pres){
        pres=!pres;
      }
      else if(digitalRead(buttonInit) && pres){
        pres=!pres;
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        tempo=tempoJogo();
        lcd.print("select: "); lcd.print(tempo); lcd.print(" min");
      }
      else if(!digitalRead(buttonInit) && pres){
        if((millis()-tempoAtual)>= 750){
          estado=3;
          tempo = tempo*60;
          lcd.setCursor(0,0);
        }
      }
      tempoAtual=millis();
    break;
    
    case 3: //
      lcd.print("Player 1");
      lcd.print("Player 2");
      tempoInicioJogo = millis();
    break;
    case 4:
      gameTime(tempo - tempoInicioJogo/1000);
    break;
  }
}
//https://www.hackster.io/YoussefSabaa/lcd-display-in-real-time-ea0b7b
