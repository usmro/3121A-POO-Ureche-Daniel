#ifndef STRADA_HPP
#define STRADA_HPP

#include <string>
#include <algorithm>

class Strada {
private:
    std::string nume;
    std::string sursa;
    std::string destinatie;
    double lungime;      // in metri
    double limitaViteza; // in km/h
    double penalizareHardware; // 0.0 (liber) pana la 100.0 (blocat complet)

public:
    Strada(const std::string& nume, const std::string& sursa, const std::string& destinatie, double lungime, double limitaViteza);
    
    std::string getNume() const;
    std::string getSursa() const;
    std::string getDestinatie() const;
    double getLungime() const;
    double getLimitaViteza() const;

    void setPenalizareHardware(double p) { penalizareHardware = p; }
    double getCost() const; // Folosit de algoritmul Dijkstra (bazat pe lungime / viteza + blocaje)
};

#endif // STRADA_HPP
