#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "Logger.hpp"
#include "Masina.hpp"
#include "Motocicleta.hpp"
#include "Camion.hpp"
#include "Strada.hpp"
#include "Intersectie.hpp"
#include "SimulareEngine.hpp"

int main() {
    std::cout << "--- Proiect Simulare Trafic Rutier - HIL (Hardware-In-The-Loop) ---\n" << std::endl;

    Logger logger("traffic_log.txt");
    SimulareEngine engine;
    
    // 1. Definim Graful orasului
    auto i1 = std::make_unique<Intersectie>("I1");
    auto i2 = std::make_unique<Intersectie>("I2");
    auto i3 = std::make_unique<Intersectie>("I3");
    auto i4 = std::make_unique<Intersectie>("I4");
    auto b1 = std::make_unique<Intersectie>("B1");
    auto b2 = std::make_unique<Intersectie>("B2");
    
    i1->attach(&logger); i2->attach(&logger); i3->attach(&logger);
    i4->attach(&logger); b1->attach(&logger); b2->attach(&logger);
    
    engine.getRetea().adaugaIntersectie(std::move(i1));
    engine.getRetea().adaugaIntersectie(std::move(i2));
    engine.getRetea().adaugaIntersectie(std::move(i3));
    engine.getRetea().adaugaIntersectie(std::move(i4));
    engine.getRetea().adaugaIntersectie(std::move(b1));
    engine.getRetea().adaugaIntersectie(std::move(b2));

    // Strazi bidirectionale (Dus si Intors)
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S1", "I1", "B1", 1000.0, 50.0));
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S1_inv", "B1", "I1", 1000.0, 50.0));

    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S2", "I2", "I1", 1500.0, 60.0));
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S2_inv", "I1", "I2", 1500.0, 60.0));

    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S3", "I2", "I3", 1000.0, 50.0));
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S3_inv", "I3", "I2", 1000.0, 50.0));

    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S4", "I3", "I4", 1500.0, 60.0));
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S4_inv", "I4", "I3", 1500.0, 60.0));

    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S5", "I4", "I1", 2000.0, 70.0));
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S5_inv", "I1", "I4", 2000.0, 70.0));

    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S6", "I4", "B2", 1000.0, 50.0));
    engine.getRetea().adaugaStrada(std::make_unique<Strada>("S6_inv", "B2", "I4", 1000.0, 50.0));

    // 2. Adaugam vehiculele in coada de asteptare
    for (int i = 1; i <= 5; ++i) {
        engine.adaugaVehicul(std::make_unique<Masina>("MASINA-" + std::to_string(i), 60.0));
        engine.adaugaVehicul(std::make_unique<Camion>("CAMION-" + std::to_string(i), 50.0));
        engine.adaugaVehicul(std::make_unique<Motocicleta>("MOTO-" + std::to_string(i), 80.0));
    }

    // 3. Pornim simularea
    engine.porneste();

    char comanda = ' ';

    std::cout << "\n=== COMENZI DIGITAL TWIN ===\n";
    std::cout << " [p] Pauza / Reluare\n";
    std::cout << " [q] Iesire\n";
    std::cout << "============================\n\n";

    while (comanda != 'q') {
        std::cin >> comanda;

        if (comanda == 'p') {
            engine.togglePauza();
        }
    }

    engine.opreste();
    
    // Cleanup Observer
    for (int k = 0; k < 6; ++k) {
        if (auto i = engine.getRetea().getIntersectie(k)) i->detach(&logger);
    }

    std::cout << "Aplicatie oprita.\n";
    return 0;
}
