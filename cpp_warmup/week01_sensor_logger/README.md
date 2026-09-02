# Week 1 小项目：传感器数据记录器

## 目标

用一个 `Sensor` 类 + `std::vector<std::shared_ptr<Sensor>>` 管理多个传感器实例，串联本周学到的：
- `class` 封装、访问控制、构造/析构函数
- 引用 `&` 与 `const` 正确性
- RAII 思想 + `std::shared_ptr`
- STL：`std::string`、`std::vector`、基于范围的 for 循环

## 要求

1. `Sensor` 类至少包含：名称（`std::string`）、当前读数（`double`）、一个 `read()` 方法（模拟读数变化）
2. 用 `std::vector<std::shared_ptr<Sensor>>` 管理若干个传感器实例
3. 遍历 vector，用基于范围的 for 循环打印每个传感器的状态
4. 观察对象析构的时机（可以在析构函数里打印一句话验证 `shared_ptr` 何时释放对象）

`src/main.cpp` 里已经搭好了类骨架和 `TODO` 标记，把 `TODO` 部分补完就是这周的产出。不要照抄现成代码——卡住了对着 [learncpp.com](https://www.learncpp.com/) 查、或翻教材对应章节，比直接抄一遍收获大得多。

## 编译运行

```bash
mkdir build && cd build
cmake ..
make
./sensor_logger
```

## 检查点自查

- [ ] 能说清楚这里为什么用 `shared_ptr` 而不是裸指针或直接用对象
- [ ] 编译没有警告（`-Wall` 打开的情况下）
- [ ] 程序退出时能在终端看到析构函数被正确调用
