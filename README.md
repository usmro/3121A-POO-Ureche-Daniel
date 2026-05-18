# Proiect POO: Simulare de Trafic Rutier (Digital Twin & Hardware-In-The-Loop)

**Autor:** Ureche Daniel (Grupa 3121A)  
**Materie:** Programare Orientată pe Obiecte (POO)

Acest proiect reprezintă o **Simulare de Trafic Rutier** avansată dezvoltată în C++, care evoluează de la conceptele clasice de OOP (Object-Oriented Programming) într-un sistem de tip **Digital Twin / Hardware-In-The-Loop (HIL)**. Simulatorul comunică bidirecțional cu o dioramă fizică (reprezentată printr-un Arduino, bariere și LED-uri) pentru a vizualiza în timp real traficul calculat de motorul C++.

---

## 🚀 Funcționalități Principale

1. **Motor de Simulare Real-Time (C++)**
   - Procesează mișcarea vehiculelor pe baza unor calcule fizice (distanță, limită de viteză pe stradă, limită de viteză a vehiculului).
   - Buclă de simulare non-blocantă, controlabilă prin comenzi: `Start`, `Pauză / Reluare`.
   
2. **Rutare Dinamică (Algoritmul Dijkstra)**
   - Vehiculele își calculează automat traseul optim de la punctul de intrare (Bariere) până la destinație, folosind algoritmul Dijkstra pe baza distanțelor (costurilor) din rețeaua rutieră.

3. **Integrare Hardware (Dioramă Fizică)**
   - **Protocol Serial (`IHardwareBridge`):** Aplicația C++ trimite semnale către portul serial.
   - **Shift Registers (74HC595):** Controlul precis al multiplexării LED-urilor pentru a aprinde segmentele de stradă fizice, reflectând poziția mașinii din cod.
   - **Bariere Automate:** Comenzi de deschidere/închidere pentru servomotoarele aflate la punctele de intrare/ieșire de pe dioramă.

4. **Sistem de Logare Avansat**
   - Monitorizare asincronă și salvare în fișierul `traffic_log.txt` pe baza **Observer Pattern**, decuplând intersecțiile de mecanismul de printare.

---

## 🛠️ Arhitectura și Concepte POO

Proiectul a fost proiectat respectând rigorile academice:
- **Încapsulare:** Starea internă a vehiculelor și străzilor este strict privată și controlată prin metode getter/setter.
- **Moștenire și Abstractizare:** O clasă de bază abstractă `Vehicul`, derivată în clasele concrete `Masina`, `Motocicleta`, `Camion`, fiecare având atribute proprii de gabarit și viteză.
- **Polimorfism:** Când rețeaua rulează bucla principală, apelează calculul timpului de parcurgere și al avansării abstract pe un vector de pointeri inteligenți `Vehicul*`, permițând comportamente personalizate la runtime (ex. camionul merge mai încet).
- **Compoziție:** Clasa `ReteaRutiera` încapsulează componentele `Strada` și `Intersectie` folosind `std::unique_ptr` gestionând perfect ciclul de viață al acestora.
- **Design Patterns:** *Observer Pattern* implementat prin interfețele `IObserver` / `ISubject` pentru logarea evenimentelor de tranzit.

---

## 📂 Structura Proiectului

```text
Proiect POO/
│
├── src/           # Codul sursă C++ (.cpp)
├── include/       # Fișierele header C++ (.hpp)
├── tests/         # Suita de teste unitare (main_tests.cpp)
├── hardware/      # Cod sursă Arduino (.ino) și scheme electronice
├── docs/          # Documentație (UML, Justificare POO, Ghid Hardware)
├── CMakeLists.txt # Sistemul de build automatizat
├── README.md      # Prezentarea proiectului
└── traffic_log.txt# Fișierul de log generat la execuție
```

---

## ⚙️ Cum se instalează și rulează

Proiectul folosește standardul **C++17** și sistemul de build **CMake** (minim 3.10).

### Compilarea proiectului
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Rularea Simulării
```bash
# Pe Windows
.\build\Debug\ProiectPOO.exe

# Pe Linux/WSL
./build/ProiectPOO
```

### Rularea Testelor Unitare
```bash
# Pe Windows
.\build\Debug\TestsPOO.exe

# Pe Linux/WSL
./build/TestsPOO
```

---
*Pentru detalii suplimentare referitoare la setup-ul hardware și configurarea pinilor pentru dioramă, consultați documentele din folderul `docs/`.*
