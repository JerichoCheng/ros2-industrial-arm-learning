#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Sensor {
private:
    std::string name;
    double value;

public:
    Sensor(std::string sensor_name, double initial_val)
        :name(std::move(sensor_name)),value(initial_val){}
    
        ~Sensor(){
            std::cout<< "Destructor" << name << "Sensor Destruction"<< std::endl;
        }
    void read(){
        value += 1;
    }

    std::string getName() const {
        return name;
    }

    double getValue() const {
        return value;
    }
};

int main() {
    std::vector<std::shared_ptr<Sensor>> sensors;

    sensors.push_back(std::make_shared<Sensor>("Temperature", 25.0));
    sensors.push_back(std::make_shared<Sensor>("Pressure", 1.0));
    sensors.push_back(std::make_shared<Sensor>("Humidity", 50.0));

    std::cout << "Initial sensor readings:" << std::endl;
    for (const auto& sensor : sensors) {
        std::cout << sensor -> getName() << ": " << sensor->getValue() << std::endl;
    }

    std::cout << "\nReading updates..." << std::endl;
    for (const auto& sensor : sensors) {
        sensor->read();
    }
    for (const auto& sensor : sensors) {
        std::cout << sensor -> getName() << ": " << sensor->getValue() << std::endl;
    }

    std::cout << "\n--- Preparing to exit the `main` scope ---" << std::endl;
    return 0;
}



// 这里为什么要用 shared_ptr 管理这些 Sensor,而不是直接用 std::vector<Sensor> 存对象本身,或者用裸指针 std::vector<Sensor*>?
// 使用 `std::shared_ptr<Sensor>` 来管理 `Sensor` 对象有几个重要的原因，而不是直接使用 `std::vector<Sensor>` 或者裸指针 `std::vector<Sensor*>`：
// 1. **动态内存管理**：使用 `std::shared_ptr` 可以自动管理
// 对象的生命周期。当 `shared_ptr` 的引用计数为零时，它会自动释放所管理的对象，避免了内存泄漏的问题。而如果使用裸指针，程序员需要手动管理内存，这容易导致内存泄漏或悬空指针。
//2. **对象的多重所有权**：`std::shared_ptr` 允许多个指针共享同一个对象的所有权。这在某些情况下非常有用，例如当多个传感器对象需要被不同的组件或模块访问时，使用 `shared_ptr` 可以确保对象在所有者都不再需要它时才被销毁。
//3. **简化代码**：使用 `std::shared_ptr` 可以减少手动管理内存的复杂性，使代码更简洁和安全。程序员不需要担心何时释放对象，也不需要担心对象的生命周期问题。
// 对比普通对象（std::vector<Sensor>）：
// 多处共享：如果日志系统、控制系统、UI 界面都需要访问同一个传感器，传普通对象会导致拷贝（副本状态与原本脱节），或者需要小心维护原对象的生命周期以防引用失效。
// 支持多态：如果未来 Sensor 派生出 TempSensor、PressureSensor 等子类，std::vector<Sensor> 会发生对象切片（Object Slicing），必须使用指针才能保持多态性。
// 对比裸指针（std::vector<Sensor*>）：
// 生命周期与内存泄漏：裸指针不管理生命周期，如果 vector 被清空或发生异常退出，容易忘记 delete 导致内存泄漏。
// 悬挂指针（Dangling Pointer）：如果有多个模块持有裸指针，某一方提前 delete 就会让其他指针变为野指针；shared_ptr 采用引用计数，只要还有模块在持有该指针，对象就不会被析构，最后一个持有者析构时会自动释放堆内存。