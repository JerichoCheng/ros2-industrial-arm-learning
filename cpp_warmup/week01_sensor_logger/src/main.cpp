// Week 1 小项目：传感器数据记录器
// 目标：Sensor 类 + std::vector<std::shared_ptr<Sensor>> 管理多个实例
//
// 这是骨架代码，不是完整答案——把标了 TODO 的地方补完。
// 卡住了参考 README.md 里的资源链接，别直接抄网上的答案。

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Sensor {
public:
    // TODO: 写一个构造函数，接收传感器名称（const std::string&），
    // 初始化 name_，reading_ 先设为 0.0

    // TODO: 写一个析构函数，打印一句话（比如 "[Sensor] xxx destroyed"）
    // 用来验证 shared_ptr 释放对象的时机

    // TODO: 写一个 read() 方法，模拟读数变化（比如每次调用 reading_ += 1.0）
    // 想一想：这个方法需不需要是 const？

    // TODO: 写一个 print() 方法（建议标 const），打印 name_ 和 reading_

private:
    std::string name_;
    double reading_{};
};

int main() {
    // TODO: 用 std::vector<std::shared_ptr<Sensor>> 创建至少 3 个传感器实例
    // 提示：std::make_shared<Sensor>("temp_01") 比 new 更符合 RAII 思想

    std::vector<std::shared_ptr<Sensor>> sensors;

    // TODO: 用基于范围的 for 循环遍历 sensors，
    // 对每个传感器调用 read() 然后 print()

    std::cout << "程序即将退出，观察下面析构函数的打印顺序：\n";

    // sensors 在这里离开作用域，shared_ptr 引用计数归零，
    // 观察析构函数是否被正确调用

    return 0;
}
