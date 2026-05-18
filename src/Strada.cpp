#include "Strada.hpp"

Strada::Strada(const std::string& nume, const std::string& sursa, const std::string& destinatie, double lungime, double limitaViteza)
    : nume(nume), sursa(sursa), destinatie(destinatie), lungime(lungime), limitaViteza(limitaViteza), penalizareHardware(0.0) {}

std::string Strada::getNume() const { return nume; }
std::string Strada::getSursa() const { return sursa; }
std::string Strada::getDestinatie() const { return destinatie; }
double Strada::getLungime() const { return lungime; }
double Strada::getLimitaViteza() const { return limitaViteza; }

double Strada::getCost() const {
    // Costul de baza e dat de timp = distanta / viteza
    double costBaza = lungime / limitaViteza;
    
    // Penalizarea hardware (0 - 100). Daca senzorul zice ca e blocat 100%, costul explodeaza
    double multiplicator = 1.0 + (penalizareHardware * 10.0); // ex: la 100% blocaj, ocolirea e de 1000 de ori mai preferata
    
    return costBaza * multiplicator;
}
