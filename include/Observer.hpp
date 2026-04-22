#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <string>
#include <vector>
#include <algorithm>

// Interfața pentru Observer
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const std::string& message) = 0;
};

// Interfața pentru Subject (Observabil)
class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void attach(IObserver* observer) = 0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify(const std::string& message) = 0;
};

// O implementare abstractă de bază pentru a nu repeta codul de listă
class Subject : public ISubject {
protected:
    std::vector<IObserver*> observers;
public:
    void attach(IObserver* observer) override {
        observers.push_back(observer);
    }
    
    void detach(IObserver* observer) override {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
    
    void notify(const std::string& message) override {
        for (auto observer : observers) {
            observer->update(message);
        }
    }
};

#endif // OBSERVER_HPP
