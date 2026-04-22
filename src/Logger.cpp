#include "Logger.hpp"
#include <chrono>
#include <iomanip>

Logger::Logger(const std::string& filename, bool writeToFile) : writeToFile(writeToFile) {
    if (this->writeToFile) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Eroare la deschiderea fisierului de log: " << filename << std::endl;
        }
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::update(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    
    // Format timestamp
    char timeBuffer[26];
#ifdef _WIN32
    ctime_s(timeBuffer, sizeof(timeBuffer), &now_c);
#else
    ctime_r(&now_c, timeBuffer);
#endif

    std::string timeStr(timeBuffer);
    timeStr.pop_back(); // Remove newline

    std::string formattedMessage = "[" + timeStr + "] " + message;

    // Log to console
    std::cout << formattedMessage << std::endl;

    // Log to file
    if (writeToFile && logFile.is_open()) {
        logFile << formattedMessage << std::endl;
    }
}
