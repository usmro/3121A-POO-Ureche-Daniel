#include "SimulareEngine.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>

SimulareEngine::SimulareEngine()
    : isRunning(false), isPaused(false), speedMultiplier(3.0) {
  // Initializare punte hardware
#ifdef __linux__
  hwBridge = std::make_unique<SerialHardwareBridge>("/dev/ttyUSB0");
#else
  hwBridge = std::make_unique<SerialHardwareBridge>("COM3");
#endif
}

SimulareEngine::~SimulareEngine() { opreste(); }

void SimulareEngine::adaugaVehicul(std::unique_ptr<Vehicul> v) {
  if (v)
    vehiculeInAsteptare.push_back(std::move(v));
}

void SimulareEngine::porneste() {
  if (isRunning)
    return;
  isRunning = true;

  // Nu mai calculam rutele pt toate aici.
  // Rutarea se va face cand sunt introduse pe harta din vehiculeInAsteptare.

  std::cout << "[Simulare] Engine pornit. HIL (Hardware-In-The-Loop) activ.\n";
  std::thread([this]() { this->buclaSimulare(); }).detach();
}

void SimulareEngine::opreste() { isRunning = false; }
void SimulareEngine::togglePauza() { isPaused = !isPaused; }
void SimulareEngine::setViteza(double m) {
  if (m > 0)
    speedMultiplier = m;
}

void SimulareEngine::buclaSimulare() {
  auto lastTime = std::chrono::steady_clock::now();

  static bool firstTimeSemafor = true;
  static bool lastS1 = false;
  static bool lastS4 = false;

  while (isRunning) {
    if (!isPaused) {
      auto currentTime = std::chrono::steady_clock::now();

      // --- LOGICA SEMAFOARE INTERSECTII I1 SI I4 ---
      int seconds = std::chrono::duration_cast<std::chrono::seconds>(
                        currentTime.time_since_epoch())
                        .count();
      bool currentS1 = (seconds % 20) < 10;       // 10s Verde, 10s Rosu
      bool currentS4 = ((seconds + 5) % 20) < 10; // Decalat cu 5 secunde

      if (firstTimeSemafor || currentS1 != lastS1) {
        hwBridge->setSemafor(1, currentS1 ? "R" : "G"); // SF1 = I1 (D7/D8)
        lastS1 = currentS1;
      }
      if (firstTimeSemafor || currentS4 != lastS4) {
        hwBridge->setSemafor(2, currentS4 ? "R" : "G"); // SF2 = I4 (D9/D10)
        lastS4 = currentS4;
      }
      firstTimeSemafor = false;
      // ---------------------------------------------

      std::chrono::duration<double> dtDuration = currentTime - lastTime;
      double dt = dtDuration.count() * speedMultiplier; // delta time
      lastTime = currentTime;

      // --- Logica de Batching ---
      if (vehiculeActive.empty() && !vehiculeInAsteptare.empty()) {
        int masiniDeAdaugat = std::min(3, (int)vehiculeInAsteptare.size());
        std::cout << "[Batching] Introducem " << masiniDeAdaugat
                  << " vehicule noi pe harta.\n";

        std::string bariere[] = {"B1", "B2"};
        std::string intersectii[] = {"I1", "I2", "I3", "I4"};

        for (int i = 0; i < masiniDeAdaugat; ++i) {
          auto v = std::move(vehiculeInAsteptare.front());
          vehiculeInAsteptare.erase(vehiculeInAsteptare.begin());

          std::string start = bariere[rand() % 2];
          std::string mid = intersectii[rand() % 4];

          // OBLIGATORIU: end trebuie sa fie diferit de start, altfel masina
          // intra in oras (pana la mid) si se intoarce inapoi pe aceeasi strada
          // (U-Turn).
          std::string end = (start == "B1") ? "B2" : "B1";

          auto ruta1 = retea.calculeazaRutaOptima(start, mid);
          auto ruta2 = retea.calculeazaRutaOptima(mid, end);

          if (!ruta2.empty()) {
            ruta1.insert(ruta1.end(), ruta2.begin(), ruta2.end());
          }

          v->setRuta(ruta1);

          // Deschidem bariera pt intrare
          int idBariera = (start == "B1") ? 1 : 2;
          hwBridge->setBariera(idBariera, true);
          std::cout << "[Bariere] Deschidere " << start
                    << " pt intrare vehicul " << v->getId() << "\n";
          std::this_thread::sleep_for(std::chrono::milliseconds(300));
          hwBridge->setBariera(idBariera, false);

          vehiculeActive.push_back(std::move(v));
        }
      }

      // 2. Miscare Masini si Aprindere LED-uri fizice
      for (auto it = vehiculeActive.begin(); it != vehiculeActive.end();) {
        auto &v = *it;
        Strada *stradaCurenta = v->getStradaCurenta();

        if (!stradaCurenta) {
          // Vehiculul a ajuns la finalul traseului
          std::cout << "> [" << v->getId() << "] a iesit de pe diorama.\n";

          // Detectam bariera corecta din destinatia ultimei strazi din ruta
          auto ruta = v->getRuta();
          int idBariera = 1; // default B1
          if (!ruta.empty()) {
            std::string dest = ruta.back()->getDestinatie();
            if (dest == "B2") idBariera = 2;
            else idBariera = 1;
          }

          hwBridge->setBariera(idBariera, true);
          std::this_thread::sleep_for(std::chrono::milliseconds(600));
          hwBridge->setBariera(idBariera, false);

          // Stingem ultimul LED cand a iesit
          if (ultimeleLeduri.find(v->getId()) != ultimeleLeduri.end()) {
            auto oldLeds = ultimeleLeduri[v->getId()];
            for (auto l : oldLeds) {
              hwBridge->setLedStatus(l.first, l.second, false);
            }
            ultimeleLeduri.erase(v->getId());
          }

          it = vehiculeActive.erase(it);
          continue;
        }

        // --- LOGICA SEMAFOR ---
        // Blocam vehiculul in 2 situatii:
        // 1. Se APROPIE de I1/I4 (dest=="I1"/"I4") si e la >= 85%
        // 2. TOCMAI A INTRAT pe o strada din I1/I4 (sursa=="I1"/"I4") si e la < 10%
        //    EXCEPTIE: Daca drum duce la B1/B2 (iesire din oras), NU blocam!
        //    (vehiculul a trecut deja intersectia si se indreapta spre iesire)
        bool blocat = false;
        {
          double progresNorm = v->getProgresPeStradaCurenta() / stradaCurenta->getLungime();
          std::string dest  = stradaCurenta->getDestinatie();
          std::string sursa = stradaCurenta->getSursa();
          bool eStradaDeIesire = (dest == "B1" || dest == "B2");

          if (lastS1) {
            bool headingToI1  = (progresNorm >= 0.85 && dest == "I1");
            bool justLeftI1   = (progresNorm <  0.10 && sursa == "I1" && !eStradaDeIesire);
            if (headingToI1 || justLeftI1) {
              blocat = true;
              std::cout << "[Semafor ROSU] " << v->getId() << " blocat la I1...\r";
              std::cout.flush();
            }
          }
          if (lastS4) {
            bool headingToI4  = (progresNorm >= 0.85 && dest == "I4");
            bool justLeftI4   = (progresNorm <  0.10 && sursa == "I4" && !eStradaDeIesire);
            if (headingToI4 || justLeftI4) {
              blocat = true;
              std::cout << "[Semafor ROSU] " << v->getId() << " blocat la I4...\r";
              std::cout.flush();
            }
          }
        }

        bool aTerminatStrada = false;
        if (!blocat) {
          aTerminatStrada = v->avanseaza(dt, stradaCurenta->getLimitaViteza());
        }

        std::string numeStradaVeche = stradaCurenta->getNume();

        if (aTerminatStrada) {
          std::cout << "\n> [" << v->getId() << "] a terminat "
                    << numeStradaVeche << "\n";
        }

        // --- BUGFIX MAJOR: REFRESH LA STRADA CURENTA ---
        stradaCurenta = v->getStradaCurenta();

        if (!stradaCurenta) {
          // A terminat toata ruta! Stingem ultimele LED-uri.
          if (ultimeleLeduri.find(v->getId()) != ultimeleLeduri.end()) {
            auto oldLeds = ultimeleLeduri[v->getId()];
            for (auto l : oldLeds) {
              hwBridge->setLedStatus(l.first, l.second, false);
            }
            ultimeleLeduri.erase(v->getId());
          }
          it = vehiculeActive.erase(it);
          continue;
        }

        // Mapare LED-uri: gasim ID-ul strazii din denumire
        std::string nume = stradaCurenta->getNume();
        int idHardware = -1;
        bool invers = false;

        if (nume.find("S1") != std::string::npos)
          idHardware = 0;
        else if (nume.find("S2") != std::string::npos)
          idHardware = 1;
        else if (nume.find("S3") != std::string::npos)
          idHardware = 2;
        else if (nume.find("S4") != std::string::npos)
          idHardware = 3;
        else if (nume.find("S5") != std::string::npos)
          idHardware = 4;
        else if (nume.find("S6") != std::string::npos)
          idHardware = 5;

        if (nume.find("_inv") != std::string::npos)
          invers = true;

        // Daca suntem pe S1 sau S2, cablarea lor fizica este in sens opus!
        // Deci inversam logica de sens pentru ele
        if (idHardware == 0 || idHardware == 1) {
          invers = !invers;
        }

        if (idHardware != -1) {
          double progres =
              v->getProgresPeStradaCurenta() / stradaCurenta->getLungime();
          int baseIndexLed = (int)(progres * 8.0);
          if (baseIndexLed >= 8)
            baseIndexLed = 7;

          int numLeds = 2; // Default masina
          if (v->getId().find("MOTO") != std::string::npos)
            numLeds = 1;
          else if (v->getId().find("CAMION") != std::string::npos)
            numLeds = 3;

          std::vector<std::pair<int, int>> noileLeduri;
          for (int i = 0; i < numLeds; ++i) {
            int logicIndex = baseIndexLed - i;
            if (logicIndex >= 0 && logicIndex < 8) {
              int hwIndex = logicIndex;
              if (invers)
                hwIndex = 7 - hwIndex;
              noileLeduri.push_back({idHardware, hwIndex});
            }
          }

          bool pozitieNoua = false;
          auto &oldLeds = ultimeleLeduri[v->getId()];

          if (oldLeds != noileLeduri) {
            pozitieNoua = true;
            // Stingem ce nu mai este in "noileLeduri"
            for (auto oldL : oldLeds) {
              if (std::find(noileLeduri.begin(), noileLeduri.end(), oldL) ==
                  noileLeduri.end()) {
                hwBridge->setLedStatus(oldL.first, oldL.second, false);
              }
            }
            // Aprindem ce e nou
            for (auto newL : noileLeduri) {
              if (std::find(oldLeds.begin(), oldLeds.end(), newL) ==
                  oldLeds.end()) {
                hwBridge->setLedStatus(newL.first, newL.second, true);
              }
            }
            ultimeleLeduri[v->getId()] = noileLeduri;
          }

          if (!pozitieNoua) {
            // Printam progresul pe linie noua ca sa fie 100% vizibil in
            // terminal
            std::cout << "[Simulare] " << v->getId() << " parcurge " << nume
                      << " - LED Base: " << baseIndexLed
                      << " (Progres: " << (int)(progres * 100) << "%)\n";
          }
        }

        ++it;
      }
    } else {
      lastTime = std::chrono::steady_clock::now(); // reset
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)); // Tick la 100ms
  }
}
