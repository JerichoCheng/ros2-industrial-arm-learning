# Week 1：C++ 速成（第一周） · 学习日志

> 日期：2026-09-02 ~ 2026-09-06

## 本周目标

- C++ 速成（从 C 到 C++）

## 学到了什么

### Day 1
- g++ 编译流程
- namespace 的作用
- iostream 替代 stdio 的写法

### Day 2
- 引用 `&`：绑定后不能换目标，本质是变量的别名，不是"存地址的变量"
- const 正确性：`const T&` 传大对象既省拷贝又防误改；引用/指针的 const 参与重载判断，按值传参的 const 不参与
- 函数重载：`process(int&)` 和 `process(const int&)` 是合法的两个重载
- 默认参数：必须从右往左连续，`foo(int a=1, int b)` 编译报错

### Day 3
- struct 与 class 唯一的硬性区别是默认访问权限：struct 默认 public，class 默认 private；对成员函数和成员变量一视同仁
- 访问符（public/private）可以在类体里出现任意多次，插在任意位置，只影响它之后到下一个访问符之前的成员
- 构造函数：一旦自定义了任意一个构造函数，编译器就不再生成无参默认构造函数；多个构造函数（不同参数列表）可以像普通函数一样重载
- 析构函数 ~ClassName()：对象生命周期结束时自动调用，局部对象的析构顺序是构造顺序的倒序（LIFO）——是 Day4 RAII 的铺垫
- 构造函数初始化列表 `: name(n), value(v)` vs 函数体内赋值：初始化列表是直接构造成员，函数体赋值是先构造后覆盖；const 成员只能用初始化列表赋初值，函数体赋值会报错（找不到 operator=）


## 卡在哪 / 怎么解决的

| 问题 | 花了多久 | 怎么解决的 |
|------|----------|------------|
| C++引用和指针行为混淆 | 20分钟 | 写小测试代码运行对比，区分`r=y`是赋值不是重绑定引用 |
| g++编译报错 | 10分钟 | 查看编译错误日志，修正头文件缺失 |

## 检查点是否通过

- [ ]

## 代码/产出链接

- [Day1 代码](../../cpp_warmup/week01_sensor_logger/scratch/day1.cpp)
- [Day2 代码](../../cpp_warmup/week01_sensor_logger/scratch/day2.cpp)
- [Day3 代码](../../cpp_warmup/week01_sensor_logger/scratch/day3.cpp)

## 下周计划微调

-
