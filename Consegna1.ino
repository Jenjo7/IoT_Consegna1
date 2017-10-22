/*
 * ===================================================================
 * Prima consegna di Sistemi Embedded e Internet of things
 * Savini Gianni 731806
 * 
 * Follow the Light
 * 
 * Note: per un malfunzionamento della libreria, ho aggiunto un metodo
 * che effutttua un reset del Timer1 senza attivare l'interrupt (cosa 
 * che invece fa il metodo start() di libreria). Questo metodo è da
 * aggiungere a TimerOne.h della libreria TimerOne scaricata dall'IDE:
 * 
 * ===================================================================
 */

 //Includo le librerie utili al programma
#include "TimerOne.h"     //link della libreria timer usata: http://code.google.com/p/arduino-timerone/
#include "Led.h"
#include "ButtonImpl.h"
#include "Potentiometer.h"

//Definisco le macro dei miei componenti
#define LED1 11
#define LED2 12
#define LED3 13
#define LED_W 8
#define BOTT1 3
#define BOTT2 4 
#define BOTT3 5
#define BOUCING 150

//Definisco macro utili al sistema
#define TIME_TO_GUESS 5000000      //tempo a disposizione del giocatore per indovinare
#define SET_LEV 128
#define MAX_DIFF 8
#define INIT 0
#define FIRST 1
#define SECOND 2
#define SIZE 20

Light* led1;
Light* led2;
Light* led3;
Light* ledw;

Button* btn1;
Button* btn2;
Button* btn3;

Potentiometer* pot;

int lengthOfSequence = INIT;      
int state = INIT;
int sequenceToGuess[SIZE];
int index = INIT;
int difficult;
int freq;
int score = 0;
bool gameover;    //modificabile solo nella funzione init_condition, e dall'interrupt del timer1


void setup() {
  Serial.begin(9600);
  led1 = new Led(LED1);
  led2 = new Led(LED2);
  led3 = new Led(LED3);
  ledw = new Led(LED_W);
  
  btn1 = new ButtonImpl(BOTT1);
  btn2 = new ButtonImpl(BOTT2);
  btn3 = new ButtonImpl(BOTT3);
  pot = new Potentiometer(A0);
  btn1->att_int(firstState);  
  Timer1.attachInterrupt(lose);              //Mettere attachInterrupt nel loop o altre funzioni (che non siano setup) da problemi, del tipo che parte l'interrupt
  Timer1.initialize(TIME_TO_GUESS);          //Quando viene eseguito, non fa altro che settare gameover a true, e il gioco finisce
}
//Note: ancora non gestico i punti e il potenziometro
void loop() {
  switch(state) {
    case INIT:
                init_condition();                           //Primo stato
                break;
    case FIRST:
                btn1->dett_int(BOTT1);                      //Disattivo l'interrupt legato al bottone 1, tramite metodo implementato in ButtonImpl.cpp
                showSequence();                             //Secondo stato
                break;
    case SECOND:
                guessSequence();                            //Terzo stato
                break;
  }
}

void init_condition() {
  //Il timer parte subito dopo  l'inizio del loop, quindi come prima cosa lo fermo
  Timer1.stop();
  gameover = false;       //Ogni volta che perdo, e reinizio una partita, deve essere false.
  Serial.println("Welcome to Follow the Light!");
  //questo codice verrà messo in una funzione che riutilizzerò
  while(state == INIT) {
    setDifficult();
    ledw->switchOn();
    delay(freq);
    ledw->switchOff();
    delay(freq);   
  }
}
//Confronto la sequenza con i bottoni che premo io, uno ad uno
void compareToSequence(int num) {
  digitalWrite(num, HIGH);
  delay(BOUCING);
  digitalWrite(num, LOW);
  sequenceToGuess[index] == num ? guess() : reset();
}

void blinky(int pinToSwitchOn) {
  digitalWrite(pinToSwitchOn, HIGH);
  delay(freq);
  digitalWrite(pinToSwitchOn, LOW);
  delay(freq);
}
//Mostra la sequenza contenuta nel vettore
void showSequence() {
  delay(500);
  for(int i = 0; i < lengthOfSequence ; i++) {
    blinky(sequenceToGuess[i]);
  }
  lengthOfSequence++;                            //Aggiungo un led alla sequenza     
  int pinToSwitchOn = random(LED1, LED3 + 1);
  sequenceToGuess[lengthOfSequence - 1] = pinToSwitchOn;
  blinky(pinToSwitchOn);
  state = SECOND;
  Timer1.restartOne();
}

void guessSequence() {
    //Controllo che bottone viene premuto, se nessuno di essi viene premuto, eseuguo una funzione vuota ( empty() ) per rispettare la sintassi deella ternaria.
    if(!gameover) {
      btn1->isPressed() ? compareToSequence(LED1) : btn2->isPressed() ? compareToSequence(LED2) : btn3->isPressed() ? compareToSequence(LED3) : empty();  
    } else {
      reset();
    }
}
//Se indovino incremento index, in modo che al prossimo passaggio il confronto venga fatto sul numero successivo della sequenza
void guess() {
  index++;
  // se l'indice è uguale alla lunghezza della sequenza, esegue la funzione  points, che assegna i punti
  if(index == lengthOfSequence){
    points();
  }
}
//Metodo che assegna i punti e riporta alla visualizzazione della sequenza con l'aggiunta di un led
void points() {
  score += lengthOfSequence * difficult;
  Timer1.stop();                              //Quando indovino stoppo il timer, so no parte l'interrupt
//  Timer1.setPeriod(5000000);                  //Codice opzionale
//  Timer1.stop();
  state = FIRST;                              //Permette di tornare a showSequence()
  index = INIT;                               //Permette di scorrre il vettore che contiene i numeri della sequenza
}                                    
//funzione vuota
void empty() {
  
}
//Setta difficolta del gioco (velocità col quale la sequenza si manifesta) e il moltiplicatore
void setDifficult() {
  freq = pot->readValue();          
  difficult = MAX_DIFF - freq/SET_LEV;
}

void firstState() {
  if(state == INIT) {
    Serial.println("Ready!");
    state = FIRST;
  }
}

void lose() {
  gameover = true;
}
//Metodo che riporta a default tutti i parametri
void reset() {
  btn1->att_int(firstState);                    //Riattacco l'interrupt a btn1
  Serial.print("GameOver :( score -> ");        //Stampa seriale dei punti
  Serial.println(score);  
  index = INIT;
  lengthOfSequence = INIT;
  score = INIT;
  state = INIT;
}

