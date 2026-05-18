#ifndef RETEARUTIERA_HPP
#define RETEARUTIERA_HPP

#include <vector>
#include <memory>
#include <map>
#include <string>
#include "Strada.hpp"
#include "Intersectie.hpp"

class ReteaRutiera {
private:
    std::vector<std::unique_ptr<Strada>> strazi;
    std::vector<std::unique_ptr<Intersectie>> intersectii;
    
    // Pentru rutare rapida: mapa nume Intersectie -> id intern
    std::map<std::string, int> mapNoduri;

public:
    ReteaRutiera() = default;

    void adaugaStrada(std::unique_ptr<Strada> strada);
    void adaugaIntersectie(std::unique_ptr<Intersectie> intersectie);
    
    Strada* getStrada(size_t index) const;
    Intersectie* getIntersectie(size_t index) const;
    Intersectie* getIntersectieNume(const std::string& nume) const;
    
    // Dijkstra
    std::vector<Strada*> calculeazaRutaOptima(const std::string& sursa, const std::string& destinatie) const;
    
    // Integrare hardware IoT
    void updatePenalizareStrada(const std::string& numeStrada, double blocaj);
};

#endif // RETEARUTIERA_HPP
