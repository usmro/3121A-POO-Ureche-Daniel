#ifndef CAMION_HPP
#define CAMION_HPP

#include "Vehicul.hpp"
#include <algorithm>

class Camion : public Vehicul {
public:
    Camion(const std::string& id, double vitezaMaxima = 40.0);
    double calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const override;
    void comportamentLaIntersectie() const override;
};

#endif // CAMION_HPP
