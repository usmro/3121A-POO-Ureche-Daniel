# Simulare de Trafic Rutier

Proiect C++ dezvoltat pentru cursul de Programare Orientată pe Obiecte (POO).
Tema proiectului este realizarea unei simulări în care vehiculele se deplasează pe o rețea de străzi coordonate de intersecții, respectând un set simplu de reguli de circulație.

Proiectul urmărește aplicarea concretă a fundamentelor POO (Încapsulare, Moștenire, Polimorfism și Compoziție), gestionarea codului sursă variat (Git), și folosirea instrumentelor moderne (CMake).

---

## 🛠️ Stadiul Curent al Implementării

Până în prezent s-au implementat următoarele module din cerințele obligatorii și facultative, structurate arhitectural sub forma primelor 3 mari etape:

### 1. Etapa de Setare & Logarea Evenimentelor (Pattern Observer)
- S-a creat **Logger**-ul proiectului folosind *Observer Pattern* (cerință facultativă), decuplând mecanismul de printare și salvare log în fișierul `traffic_log.txt` de clasele de rețea.
- S-au definit primele instrucțiuni din asamblarea codului via `CMake`.

### 2. Etapa Vehiculelor (Clase Abstracte și Polimorfism)
- S-a creat interfața/clasa abstractă de bază `Vehicul`.
- S-au definit clasele derivate **Vehicul → Masina, Motocicleta, Camion**.
- A fost validat polimorfismul primar: viteze maxime diferite rezultă într-un calcul diferit al timpilor de tranzit, alături de mesaje dinamice polimorfice pentru cum reacționează sau frânează șoferii când ajung la intrarea într-o intersecție.

### 3. Etapa Rețelei (Compoziția)
- Conceptul de Compoziție este demonstrat prin clasa `ReteaRutiera`, care încapsulează direct obiecte de tip `Intersectie` și `Strada`.
- S-a implementat transmiterea și delegarea evenimentelor: Străzile pot impune limite de viteză, în timp ce Intersecțiile notifică Loggerul instantaneu de fiecare dată când un vehicul le tranzitează sau când se generează un pre-text de "Coliziune".

---

## 🚀 Planuri de Viitor (Ce urmează să fie implementat)

Proiectul nu este finalizat 100%. În următoarele etape ne concentrăm pe exigențele finale de evaluare (Etapele 4 și 5):

- [ ] **Sistem Real-Time / Engine de Simulare:** Integrarea unei bucle avansate de simulare cu funcții explicite de control: `Start`, `Pauză` și `Accelerare` (cerință facultativă).
- [ ] **Export de date CSV:** Salvarea și interpretarea datelor rutiere logate la finalul executiei într-un sistem persitent (ex: istoric_stradal.csv) sub forma de statistici de trafic.
- [ ] **Suita de Teste Unitare:** Dezvoltarea unui fișier dedicat, automatizat la build cu reguli interne sau CAssert pentru ca proiectul să dispună de verificare corectă de rute și calcul viteză.
- [ ] **Documentația Teoretică (PDF/Markdown):** Includerea diagramelor UML ce vor justifica relațiile structurale ale claselor noastre și documentarea deciziilor finale.

---

## ⚙️ Cum se instalează / rulează

*Atenție: Proiectul folosește C++17 și CMake (minim 3.10).*
(Aceste instrucțiuni vor fi detaliate la momentul punerii la punct a build-ului final în Etapele avansate).

```bash
mkdir build
cd build
cmake ..
cmake --build .
./TrafficSimApp
```
