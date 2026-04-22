#ifndef STRADA_HPP
#define STRADA_HPP

#include <string>

class Strada {
private:
    std::string nume;
    double lungime;      // in metri
    double limitaViteza; // in km/h sau m/s

public:
    Strada(const std::string& nume, double lungime, double limitaViteza);
    
    std::string getNume() const;
    double getLungime() const;
    double getLimitaViteza() const;
};

#endif // STRADA_HPP
