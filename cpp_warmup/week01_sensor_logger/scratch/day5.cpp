#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> logs = {
    "temp: 25.3",
    "distance: 1.2",
    "pressure: 101.3"
};

std::cout << "==== range-based for loop ====\n" << std::endl;
for (const auto& x : logs) 
{
    std::cout << x << '\n';
}

std::cout << "\n==== for loop with index ====\n" << std::endl;
for (auto it = logs.begin(); it != logs.end(); ++it) 
{
    std::cout << "long: "<<(*it).size() << '\n';
}

logs.push_back("humidity: 45.6");
std::cout << "\n==== after push_back ====\n" << std::endl;
std::cout << "vector size: " << logs.size() << std::endl;

return 0;
}
