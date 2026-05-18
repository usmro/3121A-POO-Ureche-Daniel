#include "ReteaRutiera.hpp"
#include <queue>
#include <limits>
#include <algorithm>

void ReteaRutiera::adaugaStrada(std::unique_ptr<Strada> strada) {
    if (strada) {
        strazi.push_back(std::move(strada));
    }
}

void ReteaRutiera::adaugaIntersectie(std::unique_ptr<Intersectie> intersectie) {
    if (intersectie) {
        mapNoduri[intersectie->getNume()] = intersectii.size();
        intersectii.push_back(std::move(intersectie));
    }
}

Strada* ReteaRutiera::getStrada(size_t index) const {
    if (index < strazi.size()) return strazi[index].get();
    return nullptr;
}

Intersectie* ReteaRutiera::getIntersectie(size_t index) const {
    if (index < intersectii.size()) return intersectii[index].get();
    return nullptr;
}

Intersectie* ReteaRutiera::getIntersectieNume(const std::string& nume) const {
    auto it = mapNoduri.find(nume);
    if (it != mapNoduri.end()) {
        return intersectii[it->second].get();
    }
    return nullptr;
}

void ReteaRutiera::updatePenalizareStrada(const std::string& numeStrada, double blocaj) {
    for (auto& s : strazi) {
        if (s->getNume() == numeStrada) {
            s->setPenalizareHardware(blocaj);
            break;
        }
    }
}

// Algoritmul Dijkstra
std::vector<Strada*> ReteaRutiera::calculeazaRutaOptima(const std::string& sursa, const std::string& destinatie) const {
    if (mapNoduri.find(sursa) == mapNoduri.end() || mapNoduri.find(destinatie) == mapNoduri.end()) {
        return {}; // Ruta imposibila
    }

    std::map<std::string, double> distante;
    std::map<std::string, Strada*> predecesor;
    for (const auto& i : intersectii) {
        distante[i->getNume()] = std::numeric_limits<double>::max();
    }
    distante[sursa] = 0.0;

    auto cmp = [&distante](const std::string& left, const std::string& right) {
        return distante[left] > distante[right];
    };
    std::priority_queue<std::string, std::vector<std::string>, decltype(cmp)> pq(cmp);
    pq.push(sursa);

    while (!pq.empty()) {
        std::string u = pq.top();
        pq.pop();

        if (u == destinatie) break;

        // Cauta muchiile care pleaca din `u`
        for (const auto& s : strazi) {
            if (s->getSursa() == u) {
                std::string v = s->getDestinatie();
                double cost = s->getCost();

                if (distante[u] + cost < distante[v]) {
                    distante[v] = distante[u] + cost;
                    predecesor[v] = s.get();
                    pq.push(v);
                }
            }
        }
    }

    std::vector<Strada*> ruta;
    std::string curent = destinatie;
    while (predecesor.find(curent) != predecesor.end()) {
        Strada* str = predecesor[curent];
        ruta.push_back(str);
        curent = str->getSursa();
    }
    
    std::reverse(ruta.begin(), ruta.end());
    return ruta;
}
