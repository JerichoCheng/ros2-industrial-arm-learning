#include <iostream>
#include <string>
#include <memory>

using namespace std;

class Logger {

public:
    Logger() {
        cout << "Logger created" << endl;
    }
    ~Logger() {
        cout << "Logger destroyed" << endl;
    }    
};

int main() 
{
    std::shared_ptr<Logger> log1 = std::make_shared<Logger>();
    std::cout << "log1 use_count: " << log1.use_count() << std::endl;
    {
        std::shared_ptr<Logger> log2 = log1;
        std::cout << "log1 use_count: " << log1.use_count() << std::endl;
    }

    std::cout << "after inner scope, log1 use_count: " << log1.use_count() << std::endl;
    log1.reset();
    std::cout << "after reset, log1.reset(): " << std::endl;

    return 0;
}