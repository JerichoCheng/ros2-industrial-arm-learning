#include <iostream>
#include <string>

using namespace std;

class Sensor {
private:
    std::string name;
    double value;
public:
    Sensor(std::string n, double v) : name(n), value(v) {
        cout << "Sensor created " << name << ": " << value << endl;
    }

    Sensor() : name("unknown"), value(0.0) {
        cout << "Sensor created " << name << ": " << value << endl;
    }
    
    void update(double newValue) {
        value = newValue;
        cout << "Sensor updated " << name << ": " << value << endl;
    }

    ~Sensor() {
        cout << "Sensor destroyed " << name << ": " << value << endl;
    }
}; 

int main() {
    Sensor s1("Temperature", 25.0);
    Sensor s2;

    s1.update(26.5);
    s2.update(12.3);
    return 0;
}

