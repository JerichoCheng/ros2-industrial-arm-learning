# Week 3：ROS 2 核心体系 · 学习日志

## 本周目标

-

## 学到了什么

-## Day 1
- ROS 2三层架构是**递进包含**关系,不是三个独立部分:
  - ros_core:跑通ROS 2通信的最小集合(rcl、rclcpp/rclpy、DDS中间件、colcon/ament)
  - ros_base:core + 常用机器人库(tf2、common interfaces、诊断工具)
  - ros_desktop:base + GUI可视化工具(rviz2、rqt、demo示例包)——我们装的是这一层
  - 不需要背包名,遇到"某功能装不上"时知道从"我的环境可能只装了精简版本"这个角度排查
- colcon的定位:workspace级的**元构建工具**,不替代CMake,而是解决"多包依赖顺序+环境衔接+隔离归档"这几件CMake单包编译做不到的事——环境索引(AMENT_PREFIX_PATH)、生成setup脚本、包类型自动识别、build/install/log的物理隔离
- workspace四件套:`src`(唯一手动维护)、`build`(中间产物)、`install`(装好的产物+setup钩子)、`log`(每次调用的归档)
- **关键纠错**:`build`/`install`/`log`是colcon启动时**无条件创建的workspace对等目录**,跟`src`里有没有包无关——不是"扫描到包才懒加载创建"。0个包一样会生成这三个空壳目录,这一点和我最初的直觉推理(命令级vs包级懒加载)不一致,以实际跑出来的结果为准
- underlay(`/opt/ros/jazzy`,系统级)+ overlay(自己的`ros2_ws/install`)的source顺序逻辑

-## Day 2
- `ros2 pkg create --build-type ament_cmake --dependencies rclcpp` 生成的包结构：`package.xml`（元数据清单，`--dependencies`会自动写入`<depend>`）、`CMakeLists.txt`（自动带`find_package(rclcpp REQUIRED)`）、`include/<pkg>/`、`src/`
- 手写继承`rclcpp::Node`的节点类：构造函数`: Node("node_name")`调用父类构造（和Week1 Day3的自定义构造函数继承是同一套语法，只是父类换成了库提供的）
- `main`函数标准三段式：`rclcpp::init(argc, argv)`（建立通信上下文+装SIGINT处理器）→ `rclcpp::spin(std::make_shared<T>())`（阻塞式事件循环，常驻监听回调）→ `rclcpp::shutdown()`（显式释放上下文，RAII思路在进程级资源上的应用，没有析构函数自动做这件事）
- CMakeLists.txt里`target_link_libraries(... rclcpp::rclcpp)`和`ament_target_dependencies(... rclcpp)`的区别：前者是现代CMake imported target语法本身就带头文件路径，后者是ROS 2的统一封装（不用关心目标包内部是新式target还是老式变量导出），不是"谁能传头文件路径谁不能"的区别
- `ros2 run <pkg> <executable>`依赖`source install/setup.bash`把workspace推进`AMENT_PREFIX_PATH`，工具沿这条路径去`lib/<pkg>/`下找可执行文件
- `ros2 node info`验证了`rclcpp::Node`基类自带的基础设施：`/parameter_events`（pub+sub）、`/rosout`（日志publisher）、6个参数管理服务（get/set/list_parameters等）——全部不需要手写代码就存在；Action Servers/Clients则是空的，必须显式创建
- VSCode IntelliSense红线是编辑器自己的头文件索引问题，跟`colcon build`是否真正编译成功是两套独立机制；靠生成`compile_commands.json` + `.vscode/c_cpp_properties.json`指向它来修复

-## Day 3
- 进程/执行器/节点三层关系；SingleThreadedExecutor vs MultiThreadedExecutor
- spin族函数（spin/spin_some/spin_all/spin_once/spin_until_future_complete）
- rclcpp::spin(node) 本质是显式 executor 写法的语法糖
- DDS ROS_DOMAIN_ID：网络层真隔离，源码设置优先级高于命令行
- ROS 命名空间：字符串重映射，不隔离资源，命令行优先级高于源码

-## Day 4
- daemon node（守护节点）
ros2cli 第一次执行 ros2 node list 之类的指令时，会在后台常驻起一个隐藏节点 _ros2cli_daemon_0，之后每次执行 ros2 node 相关命令，本质上是在向这个已经在 跑的守护节点查询它缓存好的拓扑信息，而不是现场重新做一遍 DDS discovery。
原因：DDS 的节点发现本身有延迟，现场扫描要等一小段时间才能把网络里的节点找齐； 用一个常驻 daemon 把这个等待成本摊掉，只在它自己启动时付一次。
- --no-daemon：只换查询路径，不杀进程
加上 --no-daemon，CLI 不再问后台 daemon，而是自己临时起一个匿名节点 （命名形如 _ros2cli_<pid>）现场向 DDS 域广播探测。但原来的 daemon 进程依然在 后台跑着，如果同时加 -a，它作为一个隐藏节点仍然会被现场扫到、打印出来。
真正让它从列表消失的唯一办法是显式执行 ros2 daemon stop。
- --spin-time
只在 --no-daemon 时才有意义：现场发现如果扫完立刻返回，可能漏掉刚启动、发现 报文还没到的节点。--spin-time N 让这次临时发现多等 N 秒再汇总结果。
- 隐藏节点命名规范
以 _ 开头的节点是 hidden node，ros2 node list 默认不显示，需要 --all/-a 才会现形。daemon node 本身就是一个隐藏节点。
- 接口类型三段式
ros2 node info 里看到的类型字符串统一是 包名/类别/名字 三段式，例如 rcl_interfaces/msg/Log。中间那段是快速判断类型的关键：msg → topic， srv → service，action → action，不用靠字段名（Publishers/Service Servers…） 来分辨。


## 卡在哪 / 怎么解决的

- Day 1
- 对colcon build在0包情况下会不会创建build/install的预测,推理逻辑没问题但前提猜错了,以后遇到"预测和实际不一致"直接以实际输出为准,别死磕自己的推理
- mkdir的目标路径和cd的目标路径手滑写岔了,误建出一个不在预期位置的空壳workspace,处理这个问题的过程中顺便定下了workspace的最终路径

- Day 2
- 建包命令一开始漏了`--dependencies rclcpp`，后来重新按完整命令建包
- `source install/setup.bash`时手滑写成默认路径`~/ros2_ws`而非实际workspace路径`~/projects/ros2-industrial-arm-learning/ros2_ws`——和Day1同一类坑，需要继续留意
- 新终端`ros2 node info`一开始找不到节点，排查后是discovery/source环节的问题，第二次`ros2 node list`验证后确认节点已能被发现
- 对`target_link_libraries`不带头文件路径的判断用的是老式CMake的前提，没意识到`rclcpp::rclcpp`是现代imported target、本身就带usage requirements

- Day 3
- 域隔离测试初次结果和预期矛盾，排查后发现是一个 Day2 遗留、没关掉的旧节点进程干扰了判断，
  不是机制本身的问题——教训：结果异常先查环境残留，再怀疑原理

## 检查点是否通过

- [x]

## 代码/产出链接

-

## 下周计划微调

-
