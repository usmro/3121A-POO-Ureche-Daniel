#include "ReteaRutiera.hpp"

void ReteaRutiera::adaugaStrada(std::unique_ptr<Strada> strada) {
    if (strada) {
        strazi.push_back(std::move(strada));
    }
}

void ReteaRutiera::adaugaIntersectie(std::unique_ptr<Intersectie> intersectie) {
    if (intersectie) {
        intersectii.push_back(std::move(intersectie));
    }
}

Strada* ReteaRutiera::getStrada(size_t index) const {
    if (index < strazi.size()) {
        return strazi[index].get();
    }
    return nullptr;
}

Intersectie* ReteaRutiera::getIntersectie(size_t index) const {
    if (index < intersectii.size()) {
        return intersectii[index].get();
    }
    return nullptr;
}
