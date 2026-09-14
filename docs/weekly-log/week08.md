# Week 8：项目整合 · 学习日志

> 日期：YYYY-MM-DD ~ YYYY-MM-DD

## 本周目标

-

## 学到了什么

- ## Day 1
- **`ros2 run --prefix 'gdb -ex run --args'` 机制**：`--prefix` 是 `ros2 run` 提供的启动前缀注入点，把 gdb 包在真正的可执行文件外层；`-ex run` 让 gdb 加载完自动开始跑，不用手动敲 `run`。
- **gdb 三件套**：
  - `bt`（backtrace）：打印完整调用栈，倒着读，`#0` 是崩溃发生的最内层帧
  - `frame N`：切换到调用栈的第 N 层，只有切到变量真正声明的那一帧，`print` 才能看到它
  - `print 变量名`：打印当前帧里某个变量的值
- **动态符号表 vs DWARF 调试信息**：动态链接元数据（函数名）运行时必需，不会被剥离；DWARF（文件名/行号/局部变量映射）是可选的调试信息，默认构建不生成，需要 `-DCMAKE_BUILD_TYPE=Debug` 显式打开。这也是为什么这次 `debug_test.cpp`（自己编译，带调试信息）能看到 `debug_test.cpp:9`，而 `libmoveit_move_group_interface.so`（Week7 默认构建，无调试信息）只能看到地址 + 库文件名。
- **ros2_tracing（了解层面）**：基于 LTTng，解决的是跨进程、跨时间线的"为什么慢"问题（消息从发出到回调触发经过多久），和 gdb 解决的单点"为什么崩"是互补关系，8 周 demo 用不上，后续路线图性能优化时再深入。

## 卡在哪 / 怎么解决的

| 问题 | 花了多久 | 怎么解决的 |
1. **新终端未 source**：又一次踩了"新开终端不会自动 source `install/setup.bash`"的老坑，这次是 `ws_moveit` underlay 和 `ros2_ws` overlay 两层全空，导致 `ros2 pkg list`/`ros2 run` 找不到 `arm_moveit_demo`。用链式 source 修复：
```bash
   source ~/ws_moveit/install/setup.bash
   source ~/projects/ros2-industrial-arm-learning/ros2_ws/install/setup.bash
```
2. **`colcon build` 跑错目录**：在 `ros2_ws/src/arm_moveit_demo/` 里直接跑 `colcon build`，导致 colcon 把当前目录当成"临时工作区根"，凭空生成一套独立的 `build/`、`install/`，`debug_test` 被装进了这个"影子目录"而不是 `ros2_ws/install/`，造成 `ros2 run` 报 `No executable found`。和 Week6 Day4 记过的同一类问题（不在 workspace 根目录跑 colcon）再次发作。修法：删掉影子目录，回到 `ros2_ws` 根目录重新 build。

## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
