#include "Masina.hpp"

Masina::Masina(const std::string& id, double vitezaMaxima) : Vehicul(id, vitezaMaxima) {}

double Masina::calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const {
    double vitezaReala = std::min(vitezaMaxima, limitaVitezaStrada);
    if (vitezaReala <= 0.0) return 999999.0;
    return lungimeStrada / vitezaReala;
}

void Masina::comportamentLaIntersectie() const {
    std::cout << "[Masina::" << id << "] incetineste si acorda prioritate la nevoie." << std::endl;
}
