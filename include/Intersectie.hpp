#ifndef INTERSECTIE_HPP
#define INTERSECTIE_HPP

#include <string>
#include "Vehicul.hpp"
#include "Observer.hpp"

class Intersectie : public Subject {
private:
    std::string nume;

public:
    Intersectie(const std::string& nume);
    
    std::string getNume() const;
    void vehiculIntra(const Vehicul& vehicul);
    void vehiculIese(const Vehicul& vehicul);
    void detecteazaColiziune(const Vehicul& v1, const Vehicul& v2);
};

#endif // INTERSECTIE_HPP
