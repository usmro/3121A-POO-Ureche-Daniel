#ifndef MASINA_HPP
#define MASINA_HPP

#include "Vehicul.hpp"
#include <algorithm>

class Masina : public Vehicul {
public:
    Masina(const std::string& id, double vitezaMaxima = 50.0);
    double calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const override;
    void comportamentLaIntersectie() const override;
};

#endif // MASINA_HPP
