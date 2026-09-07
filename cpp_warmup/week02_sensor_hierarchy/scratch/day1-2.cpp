#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

// 抽象基类
class Shape {
public:
    // 纯虚函数：计算面积
    virtual double area() const = 0;

    // 虚析构函数：确保派生类对象能被正确析构
    virtual ~Shape() {
        std::cout << "[析构] Shape 基类析构" << std::endl;
    }
};

// 派生类：圆形
class Circle : public Shape {
private:
    double radius;

public:
    explicit Circle(double r) : radius(r) {}

    double area() const override {
        return M_PI * radius * radius;
    }

    ~Circle() override {
        std::cout << "[析构] Circle 派生类析构" << std::endl;
    }
};

// 派生类：矩形
class Rectangle : public Shape {
private:
    double width;
    double height;

public:
    Rectangle(double w, double h) : width(w), height(h) {}

    double area() const override {
        return width * height;
    }

    ~Rectangle() override {
        std::cout << "[析构] Rectangle 派生类析构" << std::endl;
    }
};

int main() {
    // 使用 std::vector<std::shared_ptr<Shape>> 存储不同派生类对象
    std::vector<std::shared_ptr<Shape>> shapes;
    shapes.push_back(std::make_shared<Circle>(3.0));
    shapes.push_back(std::make_shared<Rectangle>(4.0, 5.0));
    shapes.push_back(std::make_shared<Circle>(1.5));

    // 遍历容器并动态绑定调用 area()，验证多态
    std::cout << "--- 多态计算面积 ---" << std::endl;
    for (const auto& shape : shapes) {
        std::cout << "面积: " << shape->area() << std::endl;
    }

    std::cout << "\n--- 准备退出 main 作用域并释放资源 ---" << std::endl;
    return 0;
}