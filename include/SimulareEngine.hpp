#ifndef SIMULARE_ENGINE_HPP
#define SIMULARE_ENGINE_HPP

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <map>
#include "ReteaRutiera.hpp"
#include "Vehicul.hpp"
#include "HardwareBridge.hpp"

class SimulareEngine {
private:
    ReteaRutiera retea;
    std::vector<std::unique_ptr<Vehicul>> vehiculeActive;
    std::vector<std::unique_ptr<Vehicul>> vehiculeInAsteptare;
    std::unique_ptr<IHardwareBridge> hwBridge;
    std::map<std::string, std::pair<int, int>> ultimeleLeduri;

    
    std::atomic<bool> isRunning;
    std::atomic<bool> isPaused;
    std::atomic<double> speedMultiplier;

    // Metoda privata rulata in thread-ul de simulare
    void buclaSimulare();

public:
    SimulareEngine();
    ~SimulareEngine();

    // Setup
    ReteaRutiera& getRetea() { return retea; }
    void adaugaVehicul(std::unique_ptr<Vehicul> v);

    // Control
    void porneste();
    void opreste();
    void togglePauza();
    void setViteza(double multiplicator);
};

#endif // SIMULARE_ENGINE_HPP
