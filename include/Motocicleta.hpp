#ifndef MOTOCICLETA_HPP
#define MOTOCICLETA_HPP

#include "Vehicul.hpp"
#include <algorithm>

class Motocicleta : public Vehicul {
public:
    Motocicleta(const std::string& id, double vitezaMaxima = 80.0);
    double calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const override;
    void comportamentLaIntersectie() const override;
};

#endif // MOTOCICLETA_HPP
