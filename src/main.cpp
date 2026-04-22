#include <iostream>
#include <vector>
#include <memory>
#include "Logger.hpp"
#include "Masina.hpp"
#include "Motocicleta.hpp"
#include "Camion.hpp"
#include "Strada.hpp"
#include "Intersectie.hpp"
#include "ReteaRutiera.hpp"

// Eliminam TestSubject si integram ReteaRutiera / Intersectie curenta.

int main() {
    std::cout << "--- Proiect Simulare Trafic Rutier - Etapa 3 ---\n" << std::endl;

    Logger logger("traffic_log.txt");
    
    // Configurare Retea
    ReteaRutiera retea;
    retea.adaugaStrada(std::make_unique<Strada>("Bulevardul Unirii", 2500.0, 50.0));
    retea.adaugaStrada(std::make_unique<Strada>("Soseaua Kiseleff", 3000.0, 60.0));
    
    auto intersectieVitan = std::make_unique<Intersectie>("Piata Alba Iulia");
    intersectieVitan->attach(&logger); // Observer integrat direct în Intersecție
    
    auto intersectieVictoriei = std::make_unique<Intersectie>("Piata Victoriei");
    intersectieVictoriei->attach(&logger);
    
    retea.adaugaIntersectie(std::move(intersectieVitan));
    retea.adaugaIntersectie(std::move(intersectieVictoriei));

    // Vehicule
    std::vector<std::unique_ptr<Vehicul>> vehicule;
    vehicule.push_back(std::make_unique<Masina>("MASINA-01", 60.0));
    vehicule.push_back(std::make_unique<Motocicleta>("MOTO-01", 100.0));
    vehicule.push_back(std::make_unique<Camion>("CAMION-01", 50.0));

    std::cout << "\n--- Simulare Traseu ---\n" << std::endl;
    Strada* strada1 = retea.getStrada(0);
    Intersectie* int1 = retea.getIntersectie(0);
    
    if (strada1 && int1) {
        for (const auto& v : vehicule) {
            double timp = v->calculeazaTimpParcurgere(strada1->getLungime(), strada1->getLimitaViteza());
            std::cout << v->getId() << " a parcurs " << strada1->getNume() << " in " << timp << " s." << std::endl;
            
            // Interactioneaza cu intersectia = declanseaza Observer Logging
            int1->vehiculIntra(*v);
            int1->vehiculIese(*v);
        }
        
        // Simulare fake coliziune
        int1->detecteazaColiziune(*vehicule[0], *vehicule[1]);
    }

    // Cleanup automat al detasamentelor (desi ar trebui facut din destructor in mod normal)
    if (int1) int1->detach(&logger);
    Intersectie* int2 = retea.getIntersectie(1);
    if (int2) int2->detach(&logger);

    return 0;
}
