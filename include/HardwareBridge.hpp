#ifndef HARDWARE_BRIDGE_HPP
#define HARDWARE_BRIDGE_HPP

#include <string>
#include <vector>

// Interfata pentru comunicarea hardware
class IHardwareBridge {
public:
    virtual ~IHardwareBridge() = default;
    
    // Trimite o comanda de aprindere LED
    // stradaId = id-ul strazii, pozitie = indexul LED-ului pe acea strada
    virtual void setLedStatus(int stradaId, int pozitie, bool status) = 0;
    
    // Trimite o comanda de bariera
    virtual void setBariera(int intersectieId, bool deschis) = 0;

    // Seteaza culoarea unui semafor ("G" = Verde, "R" = Rosu)
    virtual void setSemafor(int idSemafor, const std::string& culoare) = 0;

    // Verifica mesaje primite de la Arduino (Senzori)
    // Returneaza string gol daca nu exista mesaj, sau mesajul (ex: "C:1:80" = Congestie pe Strada 1, 80%)
    virtual std::string citesteMesajSenzor() = 0;
};

// Implementare concreta pentru portul Serial
class SerialHardwareBridge : public IHardwareBridge {
private:
    std::string portName;
    bool isConnected;
#ifdef __linux__
    int serial_fd = -1;
#endif

public:
    SerialHardwareBridge(const std::string& port = "COM3");
    ~SerialHardwareBridge() override;

    void setLedStatus(int stradaId, int pozitie, bool status) override;
    void setBariera(int intersectieId, bool deschis) override;
    void setSemafor(int idSemafor, const std::string& culoare) override;
    std::string citesteMesajSenzor() override;
};

#endif // HARDWARE_BRIDGE_HPP
