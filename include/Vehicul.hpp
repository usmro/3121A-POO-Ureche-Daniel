#ifndef VEHICUL_HPP
#define VEHICUL_HPP

#include <string>
#include <iostream>
#include <vector>
#include "Strada.hpp"

class Vehicul {
protected:
    std::string id;
    double vitezaMaxima;
    std::vector<Strada*> rutaCurenta;
    size_t indexRuta;
    double progresPeStradaCurenta; // de la 0.0 la lungimea strazii

public:
    Vehicul(const std::string& id, double vitezaMaxima) 
        : id(id), vitezaMaxima(vitezaMaxima), indexRuta(0), progresPeStradaCurenta(0.0) {}
    virtual ~Vehicul() = default;

    virtual double calculeazaTimpParcurgere(double lungimeStrada, double limitaVitezaStrada) const = 0;
    virtual void comportamentLaIntersectie() const = 0;
    
    std::string getId() const { return id; }
    double getVitezaMaxima() const { return vitezaMaxima; }

    // Routing
    void setRuta(const std::vector<Strada*>& ruta) { 
        rutaCurenta = ruta; 
        indexRuta = 0;
        progresPeStradaCurenta = 0.0;
    }
    
    std::vector<Strada*> getRuta() const { return rutaCurenta; }
    
    Strada* getStradaCurenta() const {
        if (indexRuta < rutaCurenta.size()) return rutaCurenta[indexRuta];
        return nullptr;
    }

    // Avanseaza logic in timp (dt = delta time in secunde)
    // Returneaza true daca a terminat toata ruta
    bool avanseaza(double dt, double limitViteza) {
        if (indexRuta >= rutaCurenta.size()) return true;

        Strada* s = rutaCurenta[indexRuta];
        double vitezaReala = (lungimeStrada_helper(s) / calculeazaTimpParcurgere(s->getLungime(), limitViteza)); 
        // Viteza reala [m/s]. In loc de acest calcul, luam direct o viteza bazata pe polimorfism.
        
        progresPeStradaCurenta += (vitezaReala * dt);

        if (progresPeStradaCurenta >= s->getLungime()) {
            progresPeStradaCurenta = 0.0;
            indexRuta++;
            // Se invoca din afara trecerea prin intersectie
        }
        return false;
    }

private:
    double lungimeStrada_helper(Strada* s) const { return s ? s->getLungime() : 1.0; }
};

#endif // VEHICUL_HPP
