#include "SimulareEngine.hpp"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm>

SimulareEngine::SimulareEngine()
    : isRunning(false), isPaused(false), speedMultiplier(1) {
  // Initializare punte hardware
  hwBridge = std::make_unique<SerialHardwareBridge>("/dev/ttyUSB0");
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
void SimulareEngine::setViteza(int m) {
  if (m > 0)
    speedMultiplier = m;
}

void SimulareEngine::buclaSimulare() {
  auto lastTime = std::chrono::steady_clock::now();

  while (isRunning) {
    if (!isPaused) {
      auto currentTime = std::chrono::steady_clock::now();
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
          std::string end = bariere[rand() % 2];

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

          // Trebuie sa verificam care a fost ultima strada pentru a sti ce
          // bariera sa deschidem Alternativ, ne putem uita la destinatia
          // initiala a vehiculului (cum a fost calculata ruta). Momentan
          // simulam deschiderea barierei corespunzatoare Daca s-a terminat pe
          // S1 sau S1_inv, e B1. Daca e S6, e B2. (Sau pur si simplu le
          // deschidem pe ambele scurt pentru test)
          hwBridge->setBariera(1, true);
          hwBridge->setBariera(2, true);
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
          hwBridge->setBariera(1, false);
          hwBridge->setBariera(2, false);

          // Stingem ultimul LED cand a iesit
          if (ultimeleLeduri.find(v->getId()) != ultimeleLeduri.end()) {
            auto oldLed = ultimeleLeduri[v->getId()];
            hwBridge->setLedStatus(oldLed.first, oldLed.second, false);
            ultimeleLeduri.erase(v->getId());
          }

          it = vehiculeActive.erase(it);
          continue;
        }

        // Calcul viteza
        double timpMax = v->calculeazaTimpParcurgere(
            stradaCurenta->getLungime(), stradaCurenta->getLimitaViteza());
        double vitezaMps = stradaCurenta->getLungime() / timpMax;

        bool aTerminatStrada =
            v->avanseaza(dt, stradaCurenta->getLimitaViteza());

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

        if (idHardware != -1) {
          double progres =
              v->getProgresPeStradaCurenta() / stradaCurenta->getLungime();
          int indexLed = (int)(progres * 8.0);
          if (indexLed >= 8)
            indexLed = 7;
          if (invers)
            indexLed = 7 - indexLed; // Merge invers fizic pe LED-uri

          // Verificam daca masina tocmai a intrat pe un LED nou fata de ultima ei pozitie
          bool pozitieNoua = true;
          if (ultimeleLeduri.find(v->getId()) != ultimeleLeduri.end()) {
            auto oldLed = ultimeleLeduri[v->getId()];
            if (oldLed.first == idHardware && oldLed.second == indexLed) {
              pozitieNoua = false; // A ramas pe acelasi LED
            } else {
              // S-a mutat! Stingem LED-ul din spatele ei
              hwBridge->setLedStatus(oldLed.first, oldLed.second, false);
            }
          }

          if (pozitieNoua) {
            // Aprindem noul LED unde se afla
            hwBridge->setLedStatus(idHardware, indexLed, true);
            // Actualizam ultima ei pozitie
            ultimeleLeduri[v->getId()] = {idHardware, indexLed};
          } else {
             // Printam progresul pe aceeasi linie ca sa nu para blocata consola
             std::cout << "[Simulare] " << v->getId() << " e inca pe LED-ul " << indexLed << " (Progres strada: " << (int)(progres * 100) << "%)\r";
             std::cout.flush();
          }
        }

        if (aTerminatStrada) {
          std::cout << "\n> [" << v->getId() << "] a terminat "
                    << stradaCurenta->getNume() << "\n";
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
