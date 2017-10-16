/*
 * ===================================================================
 * Prima consegna di Sistemi Embedded e Internet of things
 * Savini Gianni 731806
 * 
 * Follow the Light
 * ===================================================================
 */

 //Includo le librerie utili al programma
#include "TimerOne.h"     //link della libreria timer usata: http://code.google.com/p/arduino-timerone/
#include "Led.h"
#include "ButtonImpl.h"
#include "Potentiometer.h"

//Definisco le macro dei miei componenti
#define POT A0
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
#define FREQ 500                    //tempo che il led di inizio rimane acceso o spento
#define LOSE 3
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

void setup() {
  Serial.begin(9600);
  led1 = new Led(LED1);
  led2 = new Led(LED2);
  led3 = new Led(LED3);
  ledw = new Led(LED_W);
  
  btn1 = new ButtonImpl(BOTT1);
  btn2 = new ButtonImpl(BOTT2);
  btn3 = new ButtonImpl(BOTT3);
  pot = new Potentiometer(POT);
  btn1->att_int(firstState);
  
  Timer1.attachInterrupt(reset);              //Mettere attachInterrupt nel loop o altre funzioni (che non siano setup) da problemi, del tipo che parte l'interrupt
  Timer1.initialize(TIME_TO_GUESS);
}
//Note: ancora non gestico i punti e il potenziometro
void loop() {
  init_condition();                           //Primo stato
  btn1->dett_int(BOTT1);                      //Disattivo l'interrupt legato al bottone 1, tramite metodo implementato in ButtonImpl.cpp
  showSequence();                             //Secondo stato
  guessSequence();                            //Terzo stato
  //state == SECOND ? guessSequence() : reset();
 
  Serial.println("Uscito dal guessSequence");
  Serial.print("index: ");
  Serial.println(index);
  Serial.print("lengthOfSequence: ");
  Serial.println(lengthOfSequence);
  Serial.print("state: ");
  Serial.println(state);
}

void init_condition() {
  //Il timer parte subito dopo  l'inizio del loop, quindi come prima cosa lo fermo
  Timer1.stop();
  difficult = pot->readValue();
  //questo codice verrà messo in una funzione che riutilizzerò
  while(state == INIT) {
    ledw->switchOn();
    delay(FREQ);
    ledw->switchOff();
    delay(FREQ);   
  }
}
//Confronto la sequenza con i bottoni che premo  io, uno ad uno
void compareToSequence(int num) {
  Serial.println("compareToSequence");
  Serial.print("Sequenza di index: ");
  Serial.println(sequenceToGuess[index]);
  Serial.print("Numero ");
  Serial.println(num);
  Serial.print("index: ");
  Serial.println(index);
  Serial.print("lengthOfSequence: ");
  Serial.println(lengthOfSequence);
  
  digitalWrite(num, HIGH);
  delay(BOUCING);
  digitalWrite(num, LOW);
  //controllo che l'indice sia minore della lunghezza della sequenza...controllo che eseguo già nella funzione chiamante, quindi forse è superfluo
  if(index < lengthOfSequence) {
    sequenceToGuess[index] == num ? guess() : reset();
  }
}

void blinky(int pinToSwitchOn) {
  digitalWrite(pinToSwitchOn, HIGH);
  delay(FREQ);
  digitalWrite(pinToSwitchOn, LOW);
  delay(FREQ);
}

void showSequence() {
  for(int i = 0; i < lengthOfSequence ; i++) {
    Serial.println("primo for");
    blinky(sequenceToGuess[i]);
  }
  lengthOfSequence++;
  int pinToSwitchOn = random(LED1, LED3 + 1);
  sequenceToGuess[lengthOfSequence - 1] = pinToSwitchOn;
  blinky(pinToSwitchOn);
  state = SECOND;
}

void guessSequence() {
  Timer1.resume();
  while(state == SECOND && index < lengthOfSequence ) {
    //Controllo che bottone viene premuto, se nessuno di essi viene premuto, eseuguo una funzione vuota ( empty() ) per rispettare la sintassi deella ternaria.
    btn1->isPressed() ? compareToSequence(LED1) : btn2->isPressed() ? compareToSequence(LED2) : btn3->isPressed() ? compareToSequence(LED3) : empty();
  }
}
//Se indovino incremento index, in modo che al prossimo passaggio il confronto venga fatto sul numero successivo della sequenza
void guess() {
  Serial.println(Timer1.read());
  index++;
  // se l'indice è uguale alla lunghezza della sequenza, esegue la funzione  points, che assegna i punti
  if(index == lengthOfSequence){
    points();
    delay(500);
  }
}

void points() {
  Timer1.stop();
  Serial.println("punti");
  Serial.print("\n");
  state = FIRST;
  index = INIT;
  Timer1.restart();
  //Timer1.setPeriod(TIME_TO_GUESS);  //Non fa un reset ma inizializza un nuovo periodo...genera anomalie nel sistema ovvero:
}                                     //allo scadere del "vecchio periodo" si accende un led dei tre della sequenza, da quel momento si accende LEDW
//funzione vuota
void empty() {
  
}

void firstState() {
  Serial.println("Primo Interrupt");
  state = FIRST;
}
//volatile perchè ho provato a verificare che non si generassero conflitti fra interrupt e altre funzioni che la chiamano
volatile void reset() {
  btn1->att_int(firstState);
  Serial.println("GameOver");
  index = INIT;
  lengthOfSequence = INIT;
  state = INIT;
  //Timer1.restart();
}

