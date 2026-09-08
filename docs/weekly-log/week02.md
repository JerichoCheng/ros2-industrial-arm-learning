# Week 2：C++ 速成（第二周） · 学习日志

> 日期：2026-09-07 ~ 2026-09-14

## 本周目标

-

## 学到了什么

-## Day1-2
- 虚函数动态绑定 vs 默认的静态绑定（指针/引用按声明类型调用，加 virtual 才按实际类型）
- override：编译期核对签名，防止"重写"变成了不相关的新函数（const、参数、拼写都算数）
- 纯虚函数 = 0 / 抽象基类：禁止直接实例化，强制派生类实现
- 虚析构函数：base 指针释放派生类对象时，不加 virtual 会跳过派生类析构 → 内存泄漏
- 析构链：派生类析构执行完，编译器自动调用基类析构（跟 virtual 无关，virtual 只决定从哪一层开始）

-## Day 3 — Lambda 表达式 + std::function
- lambda 四段式语法:`[capture](参数) -> 返回类型 { 函数体 }`
- 按值捕获 `[x]`(定义时拷贝,之后外部改变不影响)vs 按引用捕获 `[&x]`(操作原变量,同步变化)
- 理解为什么 ROS 2 回调几乎都写 `[this]`——不只是读写变量,往往还要调用对象的其他成员函数
- `std::function<返回类型(参数类型)>`:类型擦除包装器,统一"签名相同但类型各异"的可调用对象(lambda / 函数指针 / 仿函数),使其能作为参数传递、存成员变量

-## Day 4：模板基础 (Templates)
- 模板实例化 (template instantiation)：编译器在**编译期**根据传入的类型 T，为每个用到的类型生成一份专属代码，而非运行期统一处理
- 函数模板语法：`template <typename T>` + 类型自动推导（如 `myMax(3, 5)` → T 推导为 int）
- 理解 `std::vector<T>`、`std::shared_ptr<T>` 本质上是模板类，`vector<Sensor>` 就是 T=Sensor 时生成的代码
- 权衡：多份实例化代码会增大可执行文件体积，换来的是零运行时开销 + 编译期类型检查
- 提前认识了 ROS 2 里的模板语法：`create_publisher<std_msgs::msg::String>(...)`

-## Day 5: CMake基础
- CMake的本质:不是编译器,是"构建系统生成器"——读CMakeLists.txt,生成对应平台的Makefile
- 核心命令四件套:
  - `project()` — 初始化项目
  - `add_executable(target, sources...)` — 声明一个可执行文件目标
  - `find_package(lib REQUIRED)` — 找库(只负责定位头文件/库文件路径)
  - `target_link_libraries(target PRIVATE lib)` — 真正把库链接到目标上
- 编译错误 vs 链接错误的区别:
  - 缺头文件 → 编译期报错
  - 有头文件但没link库 → 编译能过,链接期报 `undefined reference`
- `PRIVATE` vs `PUBLIC`:依赖是否需要"传播"给使用这个target的其他target。可执行文件(节点)一般用`PRIVATE`,因为不会被别的target链接。

-## Day 6-7：综合小项目 —— 传感器继承体系 + 手写 CMake
- **抽象基类 `Sensor`**：`read()` 声明为纯虚函数（`= 0`），析构函数声明为 `virtual`
- **派生类 `TemperatureSensor` / `DistanceSensor`**：各自实现独立的 `read()` 物理模拟逻辑
  （温度：20~30°C 区间平滑波动；距离：0.1~5m 正常漂移 + 30% 概率阶跃）
- **`shared_ptr` 管理**：`std::vector<std::shared_ptr<Sensor>>` 多态存储，验证了基类指针析构时因 `virtual` 析构函数正确调用到子类析构
- **lambda 回调**：`Sensor` 内部用 `std::function<void(const std::string&, double)>` 存回调，`main` 中用 lambda 注册，模拟 ROS2 `create_subscription` 的回调写法
- **手写 `CMakeLists.txt`**：`add_library(sensor_lib ...)` 编译核心逻辑为静态库，`add_executable(sensor_app ...)` + `target_link_libraries(sensor_app PRIVATE sensor_lib)` 链接，`target_include_directories(sensor_lib PUBLIC include/)` 传递头文件路径

## 卡在哪 / 怎么解决的

-## Day1-2：
- 第一反应以为 C++ 方法调用默认按"实际对象类型"分派（跟 Python 一样），实际默认是静态绑定，加 virtual 才是动态绑定——这条最容易踩坑，以后见 C++ 继承代码要下意识确认有没有 virtual
- override 关键字名字没猜对（猜成了 rewrite），但用法一说就理解了

-## Day6-7：
- 两个 `DistanceSensor` 实例最初用相同的 `mt19937` 种子（`42`）初始化，导致"看似随机"的读数序列实际完全重复。改用 `std::random_device{}()` 为每个实例生成独立种子后解决。这是多传感器仿真/单元测试里的典型坑。
## 检查点是否通过

- [x]

## 代码/产出链接

- [Day1-2 代码](../../cpp_warmup/week02_sensor_hierarchy/scratch/day1-2.cpp) 
- [Day3 代码](../../cpp_warmup/week02_sensor_hierarchy/scratch/day3.cpp) 
- [Day5 代码](../../cpp_warmup/week02_sensor_hierarchy/scratch/day5_CMakelists.txt) 

## 下周计划微调

-
