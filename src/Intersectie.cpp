#include "Intersectie.hpp"

Intersectie::Intersectie(const std::string& nume) : nume(nume) {}

std::string Intersectie::getNume() const {
    return nume;
}

void Intersectie::vehiculIntra(const Vehicul& vehicul) {
    vehicul.comportamentLaIntersectie();
    notify("Vehiculul [" + vehicul.getId() + "] a intrat in intersectia " + nume);
}

void Intersectie::vehiculIese(const Vehicul& vehicul) {
    notify("Vehiculul [" + vehicul.getId() + "] a iesit din intersectia " + nume);
}

void Intersectie::detecteazaColiziune(const Vehicul& v1, const Vehicul& v2) {
    notify("COLIZIUNE in intersectia " + nume + " intre [" + v1.getId() + "] si [" + v2.getId() + "]!");
}
