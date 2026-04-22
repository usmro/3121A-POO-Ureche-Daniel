#include "Camion.hpp"

Camion::Camion(const std::string& id, double vitezaMaxima) : Vehicul(id, vitezaMaxima) {}

double Camion::calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const {
    // Camionul merge considerabil mai incet chiar daca strada permite
    double vitezaReala = std::min(vitezaMaxima, limitaVitezaStrada * 0.8);
    if (vitezaReala <= 0.0) return 999999.0;
    return lungimeStrada / vitezaReala;
}

void Camion::comportamentLaIntersectie() const {
    std::cout << "[Camion::" << id << "] se asigura si franeaza lent la intrarea in intersectie datorita greutatii." << std::endl;
}
