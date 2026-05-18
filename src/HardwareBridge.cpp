#include "HardwareBridge.hpp"
#include <iostream>
#include <chrono>

SerialHardwareBridge::SerialHardwareBridge(const std::string& port) : portName(port), isConnected(false) {
    // Aici se implementeaza deschiderea portului fizic (ex: CreateFile in Windows API)
    std::cout << "[HardwareBridge] Incercare conectare Arduino pe portul " << portName << "...\n";
    
    // Simulam conexiunea reusita
    isConnected = true;
    std::cout << "[HardwareBridge] Conexiune stabilita cu succes!\n";
}

SerialHardwareBridge::~SerialHardwareBridge() {
    if (isConnected) {
        std::cout << "[HardwareBridge] Se inchide conexiunea cu " << portName << "...\n";
    }
}

void SerialHardwareBridge::setLedStatus(int stradaId, int pozitie, bool status) {
    if (!isConnected) return;
    
    // Format protocol: L<stradaId>,<pozitie>,<1/0>
    // Arduino cu 74HC595 va parsa asta si va aprinde LED-ul
    std::string cmd = "L" + std::to_string(stradaId) + "," + std::to_string(pozitie) + "," + (status ? "1" : "0") + "\n";
    
    // TODO: WriteFile() / write() pe portul Serial fizic
    // Momentan printam in consola C++ doar pt debug arhitectural
    std::cout << "[Serial TX] -> " << cmd;
}

void SerialHardwareBridge::setBariera(int intersectieId, bool deschis) {
    if (!isConnected) return;
    std::string cmd = "B" + std::to_string(intersectieId) + "," + (deschis ? "1" : "0") + "\n";
    std::cout << "[Serial TX] -> " << cmd;
}

std::string SerialHardwareBridge::citesteMesajSenzor() {
    if (!isConnected) return "";
    
    // TODO: ReadFile() asincron de pe Serial.
    // Aici vom simula primirea unui mesaj de la Arduino (din senzorul HC-SR04).
    // Exemplu teoretic: Arduino trimite "C,1,90" adica Strada 1 blocata in proportie de 90%.
    
    return ""; // Empty pt moment
}
