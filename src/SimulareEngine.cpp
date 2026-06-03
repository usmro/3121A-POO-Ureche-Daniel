#include "SimulareEngine.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>

SimulareEngine::SimulareEngine()
    : isRunning(false), isPaused(false), speedMultiplier(3.0),
      lastIntroductionTime(std::chrono::steady_clock::now() - std::chrono::seconds(5)),
      bariera1Deschisa(false), bariera2Deschisa(false) {
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

      // --- Logica de Introducere Spatiata a Vehiculelor ---
      if (!vehiculeInAsteptare.empty() && vehiculeActive.size() < 3) {
        auto timeSinceLastIntro = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastIntroductionTime).count();
        if (timeSinceLastIntro >= 4) {
          auto v = std::move(vehiculeInAsteptare.front());
          vehiculeInAsteptare.erase(vehiculeInAsteptare.begin());

          std::string start = (rand() % 2 == 0) ? "B1" : "B2";
          std::string intersectii[] = {"I1", "I2", "I3", "I4"};
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

          std::cout << "[Bariere] Auto-detectare deschidere " << start
                    << " pt intrare vehicul " << v->getId() << "\n";

          vehiculeActive.push_back(std::move(v));
          lastIntroductionTime = currentTime;
        }
      }

      // 2. Miscare Masini si calcul LED-uri active global
      std::set<std::pair<int, int>> noileLeduriGlobal;

      for (auto it = vehiculeActive.begin(); it != vehiculeActive.end();) {
        auto &v = *it;
        Strada *stradaCurenta = v->getStradaCurenta();

        // --- LOGICA SEMAFOR ---
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

        // Refresh la strada curenta
        stradaCurenta = v->getStradaCurenta();

        if (!stradaCurenta) {
          std::cout << "> [" << v->getId() << "] a iesit de pe diorama.\n";
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

        if (idHardware == 0 || idHardware == 1) {
          invers = !invers;
        }

        if (idHardware != -1) {
          double progres = v->getProgresPeStradaCurenta() / stradaCurenta->getLungime();
          int baseIndexLed = (int)(progres * 8.0);
          if (baseIndexLed >= 8)
            baseIndexLed = 7;

          int numLeds = 2; // Default masina
          if (v->getId().find("MOTO") != std::string::npos)
            numLeds = 1;
          else if (v->getId().find("CAMION") != std::string::npos)
            numLeds = 3;

          for (int i = 0; i < numLeds; ++i) {
            int logicIndex = baseIndexLed - i;
            if (logicIndex >= 0 && logicIndex < 8) {
              int hwIndex = logicIndex;
              if (invers)
                hwIndex = 7 - hwIndex;
              noileLeduriGlobal.insert({idHardware, hwIndex});
            }
          }

          if (ultimulIndexLedTiparit[v->getId()] != baseIndexLed) {
            std::cout << "[Simulare] " << v->getId() << " parcurge " << nume
                      << " - LED Base: " << baseIndexLed
                      << " (Progres: " << (int)(progres * 100) << "%)\n";
            ultimulIndexLedTiparit[v->getId()] = baseIndexLed;
          }
        }

        ++it;
      }

      // 3. Update global LED status on Arduino
      // Turn off LEDs that are no longer active
      for (const auto &oldL : leduriActiveGlobal) {
        if (noileLeduriGlobal.find(oldL) == noileLeduriGlobal.end()) {
          hwBridge->setLedStatus(oldL.first, oldL.second, false);
        }
      }
      // Turn on new active LEDs
      for (const auto &newL : noileLeduriGlobal) {
        if (leduriActiveGlobal.find(newL) == leduriActiveGlobal.end()) {
          hwBridge->setLedStatus(newL.first, newL.second, true);
        }
      }
      leduriActiveGlobal = noileLeduriGlobal;

      // 4. Update Barrier Status (State-Driven, Non-Blocking)
      bool deschideB1 = false;
      bool deschideB2 = false;

      for (const auto &v : vehiculeActive) {
        Strada *stradaCurenta = v->getStradaCurenta();
        if (stradaCurenta) {
          std::string nume = stradaCurenta->getNume();
          double progresNorm = v->getProgresPeStradaCurenta() / stradaCurenta->getLungime();

          if (nume == "S1" && progresNorm > 0.90) {
            deschideB1 = true;
          }
          if (nume == "S1_inv" && progresNorm < 0.10) {
            deschideB1 = true;
          }
          if (nume == "S6" && progresNorm > 0.90) {
            deschideB2 = true;
          }
          if (nume == "S6_inv" && progresNorm < 0.10) {
            deschideB2 = true;
          }
        }
      }

      if (deschideB1 != bariera1Deschisa) {
        bariera1Deschisa = deschideB1;
        hwBridge->setBariera(1, bariera1Deschisa);
      }

      if (deschideB2 != bariera2Deschisa) {
        bariera2Deschisa = deschideB2;
        hwBridge->setBariera(2, bariera2Deschisa);
      }

    } else {
      lastTime = std::chrono::steady_clock::now(); // reset
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)); // Tick la 100ms
  }
}

