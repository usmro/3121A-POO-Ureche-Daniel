#include <iostream>
#include <cassert>
#include <cmath>
#include "../include/Masina.hpp"
#include "../include/Camion.hpp"
#include "../include/Strada.hpp"
#include "../include/Intersectie.hpp"

// Functie de utilitate pentru float compare
bool aproapeEgal(double a, double b, double epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
}

void testLogicaDeplasare() {
    std::cout << "[Test] Rulare testLogicaDeplasare..." << std::endl;
    Masina m("TEST-MASINA", 100.0);
    Camion c("TEST-CAMION", 50.0);

    // Test timp masina (viteza max 100, limita strada 100)
    double timpMasina = m.calculeazaTimpParcurgere(1000.0, 100.0);
    assert(aproapeEgal(timpMasina, 10.0)); // 1000 / 100 = 10 sec

    // Test timp camion (camionul limiteaza viteza reala la 80% din limita de viteza, daca e mai mica)
    // Limita viteza strada: 100. 80% e 80. Dar viteza maxima a camionului e 50.
    // Deci va merge cu 50. Timp: 1000 / 50 = 20 sec.
    double timpCamion = c.calculeazaTimpParcurgere(1000.0, 100.0);
    assert(aproapeEgal(timpCamion, 20.0));
    
    std::cout << "[Test] testLogicaDeplasare a trecut cu succes." << std::endl;
}

void testValidareTraseu() {
    std::cout << "[Test] Rulare testValidareTraseu..." << std::endl;
    // Constructorul nou: (nume, sursa, destinatie, lungime, limitaViteza)
    Strada s("Drum", "I1", "I2", 500.0, 50.0);
    assert(s.getNume() == "Drum");
    assert(s.getSursa() == "I1");
    assert(s.getDestinatie() == "I2");
    assert(aproapeEgal(s.getLungime(), 500.0));
    
    Intersectie i("Inter 1");
    assert(i.getNume() == "Inter 1");

    std::cout << "[Test] testValidareTraseu a trecut cu succes." << std::endl;
}

int main() {
    std::cout << "=== RULARE TESTE UNITARE ===" << std::endl;
    
    testLogicaDeplasare();
    testValidareTraseu();

    std::cout << "=== TOATE TESTELE AU TRECUT ===" << std::endl;
    return 0;
}
