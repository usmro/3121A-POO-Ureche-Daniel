#ifndef RETEARUTIERA_HPP
#define RETEARUTIERA_HPP

#include <vector>
#include <memory>
#include "Strada.hpp"
#include "Intersectie.hpp"

class ReteaRutiera {
private:
    std::vector<std::unique_ptr<Strada>> strazi;
    std::vector<std::unique_ptr<Intersectie>> intersectii;

public:
    ReteaRutiera() = default;

    void adaugaStrada(std::unique_ptr<Strada> strada);
    void adaugaIntersectie(std::unique_ptr<Intersectie> intersectie);
    
    // Obține instanțe la străzi/intersecții (fără a le muta ownership-ul)
    Strada* getStrada(size_t index) const;
    Intersectie* getIntersectie(size_t index) const;
};

#endif // RETEARUTIERA_HPP
