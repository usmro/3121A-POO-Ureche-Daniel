#ifndef VEHICUL_HPP
#define VEHICUL_HPP

#include <string>
#include <iostream>

class Vehicul {
protected:
    std::string id;
    double vitezaMaxima;

public:
    Vehicul(const std::string& id, double vitezaMaxima) : id(id), vitezaMaxima(vitezaMaxima) {}
    virtual ~Vehicul() = default;

    // Metode polimorfice (Cerința: Polimorfism - viteze maxime diferite, comportament la intersecții)
    virtual double calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const = 0;
    virtual void comportamentLaIntersectie() const = 0;
    
    std::string getId() const { return id; }
    double getVitezaMaxima() const { return vitezaMaxima; }
};

#endif // VEHICUL_HPP
