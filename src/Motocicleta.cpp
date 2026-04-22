#include "Motocicleta.hpp"

Motocicleta::Motocicleta(const std::string& id, double vitezaMaxima) : Vehicul(id, vitezaMaxima) {}

double Motocicleta::calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const {
    // Motocicletele se pot strecura mai bine si pot mentine o viteza mai mare
    // depasind cu putin limita de viteza declarativa (doar in simulare)
    double vitezaReala = std::min(vitezaMaxima, limitaVitezaStrada * 1.1);
    if (vitezaReala <= 0.0) return 999999.0;
    return lungimeStrada / vitezaReala;
}

void Motocicleta::comportamentLaIntersectie() const {
    std::cout << "[Motocicleta::" << id << "] accelereaza rapid pentru a trece prima cand are prioritate." << std::endl;
}
