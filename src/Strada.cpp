#include "Strada.hpp"

Strada::Strada(const std::string& nume, double lungime, double limitaViteza)
    : nume(nume), lungime(lungime), limitaViteza(limitaViteza) {}

std::string Strada::getNume() const {
    return nume;
}

double Strada::getLungime() const {
    return lungime;
}

double Strada::getLimitaViteza() const {
    return limitaViteza;
}
