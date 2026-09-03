#include <vector>
#include <cstdio>
#include <string>


int sum(const std::vector<int> &v) {
    int total = 0;
    for (int n : v) {
        total += n;
    }
    return total;
}

// 1. const 引用 + 默认参数
void printVec(const std::vector<int> &v, const std::string &label = "data") {
    printf("%s:", label.c_str());
    for (int n : v) printf(" %d", n);
    printf("\n");
}

// 2. 引用重载(const vs 非const)
void show(int &x)       { printf("non-const: %d\n", x); }
void show(const int &x) { printf("const:     %d\n", x); }

int main() {
    std::vector<int> num = {1, 2, 3, 4, 5};
    int total = sum(num);
    printf("Sum = %d\n", total);
    printVec(num);
    printVec(num, "nums");
    int y = 10;
    show(y);
    const int z = 20;
    show(z);
    return 0;
}