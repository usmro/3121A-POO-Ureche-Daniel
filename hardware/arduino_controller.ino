/*
  Cod Arduino: Traffic Diorama - Dumb Display Client
  Componente:
  - 6x 74HC595 (48 LED-uri pentru 6 strazi). Mapare pini: Q1..Q7, Q0.
  - 2x Servo (Bariere iesire).

  *** ATENTIE: NU ALIMENTATI LED-urile/SERVO-urile din pinul 5V de pe Arduino! ***
  *** Folositi strict SURSA EXTERNA 5V / 1A. Masele (GND) trebuie sa fie comune. ***
*/

#include <Servo.h>

// Pini 74HC595 (Shift Registers pentru LED-uri)
const int dataPin = 4;
const int clockPin = 5;
const int latchPin = 6;

// Pini Semafoare (I3, I4 conform GHID_HARDWARE.md)
const int semafor1Verde = 7;
const int semafor1Rosu = 8;
const int semafor2Verde = 9;
const int semafor2Rosu = 10;

// Pini Servomotoare (Bariere Iesire S5, S6)
const int servo1Pin = 11;
const int servo2Pin = 12;

Servo bariera1;
Servo bariera2;

// Starea curenta a celor 6 Shift-Registers (6 strazi x 8 LED-uri)
// strazile[0] corespunde primei strazi (IC1), strazile[5] ultimei strazi (IC6)
byte strazi[6] = {0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(semafor1Verde, OUTPUT);
  pinMode(semafor1Rosu, OUTPUT);
  pinMode(semafor2Verde, OUTPUT);
  pinMode(semafor2Rosu, OUTPUT);

  bariera1.attach(servo1Pin);
  bariera2.attach(servo2Pin);
  
  // Stare initiala: bariere jos (0 grade), semafoare rosii
  bariera1.write(0);
  bariera2.write(0);
  digitalWrite(semafor1Rosu, HIGH);
  digitalWrite(semafor2Rosu, HIGH);
  
  updateLEDs();
}

void loop() {
  // 1. Primirea Comenzilor din C++ (Aprindere LED, Bariere, Semafoare)
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    // Comanda LED: "L<id_strada_0_5>,<index_led_0_7>,<stare_0_1>"
    if (command.startsWith("L")) {
      int firstComma = command.indexOf(',');
      int secondComma = command.indexOf(',', firstComma + 1);
      
      if(firstComma > 0 && secondComma > 0) {
        int idStrada = command.substring(1, firstComma).toInt();
        int indexLed = command.substring(firstComma + 1, secondComma).toInt();
        int stare = command.substring(secondComma + 1).toInt();
        
        if (idStrada >= 0 && idStrada < 6 && indexLed >= 0 && indexLed < 8) {
          // Mapare Bit: Q1..Q7, Q0
          // Logica (0..7) -> Hardware (1,2,3,4,5,6,7,0)
          int hardwareBit = (indexLed + 1) % 8;
          
          if (stare == 1) {
            bitSet(strazi[idStrada], hardwareBit);
          } else {
            bitClear(strazi[idStrada], hardwareBit);
          }
          updateLEDs();
        }
      }
    }
    // Comanda Bariera: "B<id_bariera_1_2>,<stare_0_1>"
    else if (command.startsWith("B")) {
      int comma = command.indexOf(',');
      if (comma > 0) {
        int idBariera = command.substring(1, comma).toInt();
        int stare = command.substring(comma + 1).toInt();
        
        int unghi = (stare == 1) ? 90 : 0; // 90 deschis, 0 inchis
        if (idBariera == 1) bariera1.write(unghi);
        else if (idBariera == 2) bariera2.write(unghi);
      }
    }
    // Comanda Semafor: "SF<id_1_2>,<R_G>"
    else if (command.startsWith("SF")) {
      int comma = command.indexOf(',');
      if (comma > 0) {
        int idSemafor = command.substring(2, comma).toInt();
        String stare = command.substring(comma + 1);
        
        if (idSemafor == 1) {
          digitalWrite(semafor1Verde, (stare == "G") ? HIGH : LOW);
          digitalWrite(semafor1Rosu, (stare == "R") ? HIGH : LOW);
        } else if (idSemafor == 2) {
          digitalWrite(semafor2Verde, (stare == "G") ? HIGH : LOW);
          digitalWrite(semafor2Rosu, (stare == "R") ? HIGH : LOW);
        }
      }
    }
  }
}

// Scrie cei 6 bytes catre cele 6 Shift Registers inlantuite
void updateLEDs() {
  digitalWrite(latchPin, LOW);
  // ShiftOut se face in ordine inversa pentru inlantuire. 
  // Ultimul IC (#6) trebuie sa primeasca primul.
  for (int i = 5; i >= 0; i--) {
    shiftOut(dataPin, clockPin, MSBFIRST, strazi[i]);
  }
  digitalWrite(latchPin, HIGH);
}
