// Week 2 小项目：传感器继承体系
// 目标：抽象基类 Sensor + TemperatureSensor / DistanceSensor 子类
//      + lambda 回调 + std::vector<std::shared_ptr<Sensor>> 多态管理
//
// 骨架代码，不是完整答案——把 TODO 补完。

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ---- 抽象基类 ----
class Sensor {
public:
    explicit Sensor(std::string name) : name_(std::move(name)) {}

    // TODO: 析构函数要不要写成 virtual？想清楚为什么再写。
    virtual ~Sensor() = default;

    // TODO: 声明一个纯虚函数 read()，返回 double，不提供实现
    // virtual double read() = 0;

    const std::string& name() const { return name_; }

private:
    std::string name_;
};

// ---- 子类 1：温度传感器 ----
class TemperatureSensor : public Sensor {
public:
    explicit TemperatureSensor(std::string name) : Sensor(std::move(name)) {}

    // TODO: override read()，返回一个模拟温度值（比如围绕 25.0 波动）
};

// ---- 子类 2：距离传感器 ----
class DistanceSensor : public Sensor {
public:
    explicit DistanceSensor(std::string name) : Sensor(std::move(name)) {}

    // TODO: override read()，返回一个模拟距离值（比如围绕 100.0 波动）
};

int main() {
    // TODO: 创建至少一个 TemperatureSensor 和一个 DistanceSensor，
    // 都放进 std::vector<std::shared_ptr<Sensor>>（体会多态）
    std::vector<std::shared_ptr<Sensor>> sensors;

    // TODO: 用 std::function<void(const Sensor&, double)> 定义一个回调，
    // 比如接收传感器引用和读数，超过某个阈值就打印警告；
    // 用 lambda 表达式实现这个回调

    // TODO: 遍历 sensors，对每个传感器调用 read()，
    // 再把结果交给上面的回调处理

    return 0;
}
