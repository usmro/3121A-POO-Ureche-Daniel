#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Observer.hpp"
#include <fstream>
#include <string>
#include <iostream>

class Logger : public IObserver {
private:
    std::ofstream logFile;
    bool writeToFile;

public:
    Logger(const std::string& filename = "traffic_log.txt", bool writeToFile = true);
    ~Logger() override;

    // Metoda apelată când e notificat de Subject
    void update(const std::string& message) override;
};

#endif // LOGGER_HPP
