# Week 2 小项目：传感器继承体系

## 目标

把第 1 周的 `Sensor` 改造成继承体系：`TemperatureSensor : public Sensor`、`DistanceSensor : public Sensor`，串联本周学到的：
- 继承与多态：`virtual` 函数、`override`、纯虚函数（抽象基类）
- Lambda 表达式 + `std::function`
- 手写 CMakeLists.txt（不依赖 ROS 2，验证真的懂构建流程）

这是检验第 1-2 周成果的标准：不是"看懂了"，而是"编译通过、跑得动"。

## 要求

1. `Sensor` 改成抽象基类：至少有一个纯虚函数 `virtual double read() = 0;`
2. `TemperatureSensor`、`DistanceSensor` 各自 `override read()`，返回不同风格的模拟数据
3. 用 `std::vector<std::shared_ptr<Sensor>>` 统一管理两种子类实例（体会多态：通过基类指针调用，实际执行子类的实现）
4. 用一个 `std::function<void(const Sensor&)>` 类型的回调（可以传 lambda），对每个传感器做一次"处理"（比如超过阈值就打印警告）

`src/main.cpp` 里已经搭好骨架，把 `TODO` 补完。

## 编译运行

```bash
mkdir build && cd build
cmake ..
make
./sensor_hierarchy
```

## 检查点自查

- [ ] 能解释清楚为什么基类析构函数要写成 `virtual`（想一想：如果不写会发生什么）
- [ ] 能用一句话说清楚"纯虚函数"和"普通虚函数"的区别
- [ ] lambda 回调能正常工作，且能说清 `[this]`、`[&]`、`[=]` 捕获方式的区别
