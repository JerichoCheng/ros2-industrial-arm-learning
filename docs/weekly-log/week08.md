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

- ## Day 2
- **rosbag2 录制/回放**：`ros2 bag record -a -o <name>` 录制，生成一个文件夹（`metadata.yaml` + `<name>_0.mcap`），ROS2 Iron/Jazzy 起默认存储后端从 SQLite3(`.db3`) 换成了 MCAP，为顺序追加写入/顺序回放的访问模式做了优化；文件名里的 `_0` 是分卷编号，`--max-bag-size`/`--max-bag-duration` 触发切分时会累加。
- **DDS 匿名发布-订阅模型**：订阅者只匹配 topic 名字+类型+QoS，不关心谁发布的；`ros2 bag play` 回放时，原节点和回放进程在 DDS 层面是两个平等的 publisher，`ros2 topic echo` 会把两路消息全部吞下打印，无法从消息本身区分来源。
- **`ros2 topic info -v`**：能看到某 topic 当前的 publisher/subscriber 数量、各自的节点名和 QoS，是"从发现层面区分有几路在发"的手段（但依然分不出具体某条消息来自哪一路）。
- **`rqt_graph`**：图形化查看节点-话题连接关系；默认下拉框是 "Nodes only"（只画节点，不画连线），要看实际的发布-订阅连线需切到 "Nodes/Topics (active)"。
- **排查闭环方法论**：制造问题（起 `bag play --loop`）→ 观察异常状态（`Publisher count: 2`）→ 移除干扰源（停 `bag play`）→ 确认状态恢复（`Publisher count` 掉回 1）。

## 卡在哪 / 怎么解决的

| 问题 | 花了多久 | 怎么解决的 |
1. **新终端未 source**：又一次踩了"新开终端不会自动 source `install/setup.bash`"的老坑，这次是 `ws_moveit` underlay 和 `ros2_ws` overlay 两层全空，导致 `ros2 pkg list`/`ros2 run` 找不到 `arm_moveit_demo`。用链式 source 修复：
```bash
   source ~/ws_moveit/install/setup.bash
   source ~/projects/ros2-industrial-arm-learning/ros2_ws/install/setup.bash
```
2. **`colcon build` 跑错目录**：在 `ros2_ws/src/arm_moveit_demo/` 里直接跑 `colcon build`，导致 colcon 把当前目录当成"临时工作区根"，凭空生成一套独立的 `build/`、`install/`，`debug_test` 被装进了这个"影子目录"而不是 `ros2_ws/install/`，造成 `ros2 run` 报 `No executable found`。和 Week6 Day4 记过的同一类问题（不在 workspace 根目录跑 colcon）再次发作。修法：删掉影子目录，回到 `ros2_ws` 根目录重新 build。

- **`ros2 bag play` 是一次性任务，不会像常驻节点一样等你**：第一次没加 `--loop`，切终端敲验证命令时回放早已放完退出，看到的是"回放结束后"的假阴性结果，不是回放进行中的状态。加 `--loop` 后才稳定复现出 `Publisher count: 2`。
- **孤儿终端/未清理进程**：中途出现过一个多余的 `_ros2cli_` 订阅节点，是某个 `ros2 topic echo` 终端没有真正退出的残留——延续了 Week3 Day4 daemon 节点排查、Week5 Day3 环境污染排查里记过的同一类"先怀疑残留进程，再怀疑底层机制"的习惯。

## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
