# 🔌 Ghid Hardware: Diorama "Digital Twin" - Simulare Trafic Rutier

Acest document descrie **complet** tot ceea ce trebuie să cumperi, să conectezi și să programezi pentru a construi diorama fizică de 1m x 0.5m controlată de aplicația C++.

---

## 🧠 1. Microcontrollerul Ales: Arduino Uno / Arduino Nano

### De ce Arduino Uno/Nano și nu ESP32/ESP8266?
| Criterie | Arduino Uno/Nano | ESP32/ESP8266 |
|---|---|---|
| Comunicare cu C++ | ✅ Serial USB nativ, simplu | ⚠️ Mai complex (WiFi sau Serial separat) |
| Tensiune logică | 5V (ideal pentru LED-uri directe) | 3.3V (necesită level shifter) |
| Număr pini digitali | 14 (Uno) / 22 (Nano) | Mulți, dar mai greu de gestionat |
| Complexitate cod | ✅ Minimă | Medie |
| Disponibil în trusă | ✅ Probabil da | Probabil da |

> [!TIP]
> Recomandarea mea este **Arduino Nano**, deoarece este mai mic, mai ușor de plasat pe machetă, dar funcționează identic cu Uno. Codul este **100% același**.

### Specificații folosite:
- **Model:** Arduino Uno R3 sau Arduino Nano
- **Microcontroller:** ATmega328P
- **Frecvență:** 16 MHz
- **Tensiune logică:** 5V
- **Pini Digitali:** 14 (Uno) / 22 (Nano)
- **Comunicare PC:** USB Serial (CH340 sau FTDI)

---

## 📦 2. Lista Completă de Componente

### Componente Obligatorii
| # | Componentă | Cantitate | Rol în Proiect |
|---|---|---|---|
| 1 | Arduino Nano / Uno | 1-2 buc | Creierul hardware, interfață USB-Serial cu PC |
| 2 | IC Shift Register 74HC595 | 3-4 buc | Multiplexare LED-uri (1 IC = 8 LED-uri) |
| 3 | LED-uri albe/galbene (3mm sau 5mm) | 30-40 buc | Reprezintă mașinile pe străzi |
| 4 | LED-uri verzi (5mm) | 4 buc | Semafoare VERDE la intersecții |
| 5 | LED-uri roșii (5mm) | 4 buc | Semafoare ROȘU la intersecții |
| 6 | Rezistențe 220Ω | 40-50 buc | Limiteaza curentul pe fiecare LED |
| 7 | Breadboard-uri | 6+ buc | Montarea componentelor pentru cele 6 străzi |
| 8 | Fire jumper Male-Male/Female-Male | 50+ buc | Conectare componente |
| 9 | Cablu USB (tip B sau Micro-USB) | 1 buc | Conexiunea Arduino ↔ Laptop |
| 10 | **Sursă de alimentare 5V** | 1 buc | **CRITIC:** Alimentare separată (ex: MB102 sau încărcător telefon 5V/2A) pentru LED-uri și Servomotoare |

### Componente Opționale (Bonus la prezentare!)
| # | Componentă | Cantitate | Rol |
|---|---|---|---|
| A | Servo Motor SG90 | 2 buc | Bariere fizice la cele 2 ieșiri ale dioramei |

---

## 🗺️ 3. Schema Topologiei Dioramei (Planul Orașului)

Vom construi un oraș cu **4 intersecții** (I1, I2, I3, I4) legate prin **6 străzi** (muchiile grafului Dijkstra din C++):

```text
       I2----S3----> I3
        |             |
       S2            S4
        |             |
        v             v
       I1 <---S5---- I4
        |             |
       S1  Arduino   S6
        |             |
    Bariera1      Bariera2
```

> [!NOTE]
> Fiecare **stradă** = un șir de 8-10 LED-uri albe pe breadboard (multiplexate prin 74HC595).
> Fiecare **intersecție** = un LED verde + un LED roșu (semafor) + un punct de stradă.

---

## 🔗 4. Schema de Conectare Pin-cu-Pin

### 4.1 Shift Register 74HC595 #1 (Strada S1: 8 LED-uri)
| Pin 74HC595 | Conectare | Note |
|---|---|---|
| VCC (16) | 5V Arduino | Alimentare |
| GND (8) | GND Arduino | Masă comună |
| OE (13) | GND | Permanent ON (active LOW) |
| MR (10) | 5V | Nu reseta niciodata (active LOW) |
| **DS (14) - Data** | **D4 Arduino** | Fir de date serial |
| **SHCP (11) - Clock** | **D5 Arduino** | Ceas de sincronizare |
| **STCP (12) - Latch** | **D6 Arduino** | Latch (aplică datele la ieșiri) |
| Q0-Q7 (15,1-7) | Anod LED-uri S1[0..7] prin 220Ω | 8 ieșiri pentru 8 LED-uri |

### 4.2 Shift Register 74HC595 #2 (Strada S2: 8 LED-uri)
| Pin 74HC595 | Conectare | Note |
|---|---|---|
| VCC, GND, OE, MR | Identic cu #1 | La fel |
| **DS (14) - Data** | **Q7S de la IC #1** | Se înlănțuie! (Daisy-chain) |
| **SHCP (11)** | **D5 Arduino** | Același pin de ceas ca IC #1 |
| **STCP (12)** | **D6 Arduino** | Același pin Latch ca IC #1 |
| Q0-Q7 | Anod LED-uri S2[0..7] prin 220Ω | 8 ieșiri LED-uri |

### 4.2 Shift Register 74HC595 #2-6
Se înlănțuie (Daisy-chain) Pin 14 (Data) de la pinul 9 (Q7S) al IC-ului precedent.

> [!IMPORTANT]
> **Daisy-chain:** Toate IC-urile 74HC595 partajează același pin de Clock (D5) și Latch (D6). Singurul fir care se înlănțuie este Data (Q7S al unui IC → DS al urmatorului). Deci poți controla 48 de LED-uri cu doar **3 pini Arduino (D4, D5, D6)**!
>
> **MAPARE BIȚI (Atenție):** Exact cum ai conectat fizic: primul pin hardware pe Shift Register este Q1 și ultimul este Q0. Conexiunile fizice ale LED-urilor de pe stradă (indexate logic 0-7) sunt mapate intern de codul Arduino astfel încât indexul `i` să fie trimis la bitul `(i + 1) % 8`. Așadar, logica C++ nu e afectată, software-ul Arduino mută biții corect!
| **D5** | Clock Shift Register | Pin SHCP (11) al tuturor IC-urilor |
| **D6** | Latch Shift Register | Pin STCP (12) al tuturor IC-urilor |
| **D7** | Semafoare VERDE (I1, I2) | LED Verde prin 220Ω |
| **D8** | Semafoare ROȘU (I1, I2) | LED Rosu prin 220Ω |
| **D9** | Semafoare VERDE (I3, I4) | LED Verde prin 220Ω |
| **D10** | Semafoare ROȘU (I3, I4) | LED Rosu prin 220Ω |
| **D11** | Servo Barieră I1 (Opțional) | Semnal PWM Servo SG90 |
| **D12** | Servo Barieră I3 (Opțional) | Semnal PWM Servo SG90 |
| **5V Sursă Externă** | Alimentare componente | 74HC595 VCC, Servo VCC (A NU SE CONECTA LA ARDUINO 5V!) |
| **GND** | Masă comună | Toate GND (inclusiv GND-ul Arduino-ului) |
| **USB** | Comunicare cu PC | Cablu la Laptop (COM3 / /dev/ttyUSB0) |

---

## ⚡ 5. Schema Electrică pentru un LED (Repetabilă x30)

```
Arduino D5 (sau Q0-Q7 al 74HC595)
          |
          |
       [220Ω] ← Rezistentă limitatoare de curent
          |
         (+) LED (-)
          |
         GND
```

> [!WARNING]
> **NU conecta niciodată un LED direct la Arduino fără rezistență 220Ω!** Curentul maxim pe un pin Arduino este 40mA, iar un LED are nevoie de ~20mA. Fără rezistență, arzi LED-ul sau pinul Arduino!

---



---

## 🔌 7. Conectarea Servo Motor (Bariere Ieșire)

Barierele vor fi puse la finalul segmentelor S5 și S6. Când un LED ajunge la finalul străzii, PC-ul trimite comanda de ridicare.

| Pin Servo SG90 | Culoare fir | Conectare |
|---|---|---|
| Semnal PWM | Portocaliu | D11 (Bariera 1) / D12 (Bariera 2) |
| VCC | Roșu | **5V de la Sursa Externă** (NU de la Arduino) |
| GND | Maro/Negru | GND Comun |

**Funcționare (Logica C++ ↔ Arduino):**
1. LED-ul mașinii ajunge pe ultima poziție din S5.
2. C++ trimite prin Serial: `B,1,1\n` (Deschide bariera 1).
3. Brațul servo se ridică (90 grade).
4. LED-ul mașinii iese de pe hartă (se stinge).
5. C++ trimite `B,1,0\n` (Închide bariera).

---

## 📐 8. Planul Fizic al Dioramei (Aranjament pe Lemn)

```text
+------------------------------------------------------------------+
|  LEMN 100cm x 50cm                                               |
|                                                                  |
|       I1 ---------S1--------> I2                                 |
|       |                        |                                 |
|      S4                       S2                                 |
|       |                        |                                 |
|       v                        v                                 |
|       I3 <--------S3--------- I4                                 |
|       |                        |                                 |
|      S5                       S6                                 |
|       |                        |                                 |
|       v                        v                                 |
|    [Bariera 1]              [Bariera 2]                          |
|                                                                  |
|  [Arduino Nano]  [IC1-IC6 74HC595]                               |
|  (ascuns sub lemn sau intr-o cutie mica)                         |
+------------------------------------------------------------------+
```

**Legende:**
- `[I1] [I2] [I3] [I4]` = Intersecții (LED verde + LED roșu semafor)
- `===S1===` = Stradă din LED-uri albe (câte 8 per stradă)

---

## 💻 9. Protocolul de Comunicare USB-Serial (Rezumat)

### PC (C++) → Arduino (Comenzi)
| Format | Exemplu | Acțiune |
|---|---|---|
| `L<ic>,<bit>,<val>\n` | `L0,3,1\n` | Aprinde LED-ul 3 de pe IC-ul 0 |
| `B<id>,<stare>\n` | `B1,1\n` | Deschide bariera intersecției 1 |
| `SF<id>,<culoare>\n` | `SF1,G\n` | Semaforul 1 → Verde (Green) |
| `SF<id>,<culoare>\n` | `SF1,R\n` | Semaforul 1 → Roșu (Red) |

### Arduino → PC (Senzori)
Momentan nu există senzori pe dioramă care să trimită date înapoi către C++. Comunicarea este strict PC → Arduino.

---

## 🛒 10. Listă de Cumpărături (Dacă îți lipsesc componente)

> [!NOTE]
> Dacă ai deja trusa de electronică, verifică mai întâi ce ai. Probabil ai deja LED-uri, rezistențe și breadboard-uri.

| Componentă | Preț estimat (RON) | Unde găsești |
|---|---|---|
| 74HC595 (x4 buc) | ~10 RON | Optimus Digital, Sigmanortec |
| Servo SG90 | ~15 RON | eMAG, Optimus |
| LED-uri (set 100 buc) | ~10 RON | Optimus, orice magazin electro |
| Rezistențe 220Ω (set) | ~5 RON | Optimus |
| Fire Jumper set | ~15 RON | eMAG, Optimus |
| **TOTAL estimat** | **~63 RON** | (dacă ai Arduino și breadboard-uri) |
