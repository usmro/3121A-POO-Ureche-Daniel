#include "HardwareBridge.hpp"
#include <chrono>
#include <iostream>

#ifdef __linux__
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

SerialHardwareBridge::SerialHardwareBridge(const std::string &port)
    : portName(port), isConnected(false) {
  std::cout << "[HardwareBridge] Incercare conectare Arduino pe portul "
            << portName << "...\n";

#ifdef __linux__
  serial_fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (serial_fd != -1) {
    struct termios options;
    tcgetattr(serial_fd, &options);

    // Configurare in mod RAW (fara caractere speciale de escape, fara echo)
    cfmakeraw(&options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Fara flow control hardware/software
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    options.c_cflag |= (CLOCAL | CREAD);

    tcsetattr(serial_fd, TCSANOW, &options);

    std::cout << "[HardwareBridge] Asteptam 2 secunde pentru reset-ul Arduino "
                 "(Bootloader)...\n";
    sleep(2); // CRITIC: Asteptam ca Arduino sa iasa din reset

    isConnected = true;
    std::cout
        << "[HardwareBridge] Conexiune stabilita cu succes (Linux I/O)!\n";
  } else {
    std::cout
        << "[HardwareBridge] EROARE: Nu s-a putut deschide portul fizic!\n";
  }
#else
  isConnected = true;
  std::cout
      << "[HardwareBridge] Conexiune stabilita cu succes (Mock Windows)!\n";
#endif
}

SerialHardwareBridge::~SerialHardwareBridge() {
  if (isConnected) {
    std::cout << "[HardwareBridge] Se inchide conexiunea cu " << portName
              << "...\n";
#ifdef __linux__
    if (serial_fd != -1)
      close(serial_fd);
#endif
  }
}

void SerialHardwareBridge::setLedStatus(int stradaId, int pozitie,
                                        bool status) {
  if (!isConnected)
    return;

  // Format protocol: L<stradaId>,<pozitie>,<1/0>
  // Arduino cu 74HC595 va parsa asta si va aprinde LED-ul
  std::string cmd = "L" + std::to_string(stradaId) + "," +
                    std::to_string(pozitie) + "," + (status ? "1" : "0") + "\n";

  std::cout << "[Serial TX] -> " << cmd;
#ifdef __linux__
  if (serial_fd != -1)
    write(serial_fd, cmd.c_str(), cmd.length());
#endif
}

void SerialHardwareBridge::setBariera(int intersectieId, bool deschis) {
  if (!isConnected)
    return;
  std::string cmd =
      "B" + std::to_string(intersectieId) + "," + (deschis ? "1" : "0") + "\n";
  std::cout << "[Serial TX] -> " << cmd;
#ifdef __linux__
  if (serial_fd != -1)
    write(serial_fd, cmd.c_str(), cmd.length());
#endif
}

std::string SerialHardwareBridge::citesteMesajSenzor() {
  if (!isConnected)
    return "";

  // TODO: ReadFile() asincron de pe Serial.
  // Aici vom simula primirea unui mesaj de la Arduino (din senzorul HC-SR04).
  // Exemplu teoretic: Arduino trimite "C,1,90" adica Strada 1 blocata in
  // proportie de 90%.

  return ""; // Empty pt moment
}
