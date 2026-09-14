# Week 8：项目整合 · 学习日志

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

- ## Day 3-5
- **`MoveGroupInterface` 的 group 绑定是构造时定死的**：无法运行时切换控制目标，需要控制多个 group（`panda_arm` + `hand`）时应各自构造独立对象，而不是共用一个或反复重新构造（后者有action client初始化、等待服务就绪的显著开销）。
- **SRDF 里的 `group_state`（命名姿态）**：Panda 的 `hand` group 预定义了 `open`（0.035/0.035）和 `close`（0/0）两个命名姿态，可以 `setNamedTarget()` 直接调用，不用手算关节角度。同时存在一个 `panda_arm_hand` 组合 group（用于手臂+夹爪需要联动规划的场景），本次顺序执行的抓取序列用不上。
- **`move()` vs `plan()+execute()`**：两者在"失败能否被捕获"上没有本质区别（`move()` 一样返回 `MoveItErrorCode` 可以判断）；真正的区别是 `plan()+execute()` 拆开后，执行前多了一个能检查 `Plan` 对象的窗口（比如以后接真实硬件时加一道人工确认关卡）。夹爪这种关节空间点到点插值几乎不会规划出意外轨迹，用 `move()` 省事；手臂在三维空间里跑轨迹，保留检查窗口更稳妥。
- **`execute()`/`move()` 的阻塞语义**：底层依赖 `FollowJointTrajectory`/`GripperCommand` 等 action 的完整交互（等到 Result 才返回），返回时机械臂必然已停稳，`sleep_for()` 在功能正确性上是多余的，纯粹用于演示时的视觉节奏缓冲。

## Day 6
- **`LaunchDescription` 内的动作默认并发启动**：不是按列表顺序等前一个就绪才启动下一个，需要 `TimerAction`（纯墙钟延迟）或 `RegisterEventHandler`（事件驱动）人为制造依赖顺序。对这种"拉起一整套大型仿真栈+业务节点"的场景，`TimerAction` 给一个宽松估计的延迟窗口是最实用的做法。
- **`IncludeLaunchDescription` + `get_package_share_directory()`**：把第三方包（`moveit_resources_panda_moveit_config`）的 `demo.launch.py` 整个引入进自己的 launch 文件，用包名动态查路径而不是写死绝对路径——原理和 `ros2 pkg prefix`/`AMENT_PREFIX_PATH` 包索引机制一致，保证换机器/换用户名依然能跑。
- **两层等待机制的分工**：`TimerAction` 的延迟只防最坏情况（系统整体还没起来），真正兜住"controller 刚激活、还没完全就绪"这种慢半拍场景的，是 `MoveGroupInterface` 构造时自身对 action client 连接的阻塞行为（构造函数会一直等到能连上对应 planning group 的 action server 才返回）——这次实测日志里 `panda_hand_controller` 激活和 `pick_place_demo` 实际发出开爪请求只差 0.49 秒，验证了这个兜底机制真实生效。

## 卡在哪 / 怎么解决的

1. **新终端未 source**：又一次踩了"新开终端不会自动 source `install/setup.bash`"的老坑，这次是 `ws_moveit` underlay 和 `ros2_ws` overlay 两层全空，导致 `ros2 pkg list`/`ros2 run` 找不到 `arm_moveit_demo`。用链式 source 修复：
```bash
   source ~/ws_moveit/install/setup.bash
   source ~/projects/ros2-industrial-arm-learning/ros2_ws/install/setup.bash
```
2. **`colcon build` 跑错目录**：在 `ros2_ws/src/arm_moveit_demo/` 里直接跑 `colcon build`，导致 colcon 把当前目录当成"临时工作区根"，凭空生成一套独立的 `build/`、`install/`，`debug_test` 被装进了这个"影子目录"而不是 `ros2_ws/install/`，造成 `ros2 run` 报 `No executable found`。和 Week6 Day4 记过的同一类问题（不在 workspace 根目录跑 colcon）再次发作。修法：删掉影子目录，回到 `ros2_ws` 根目录重新 build。

- **`ros2 bag play` 是一次性任务，不会像常驻节点一样等你**：第一次没加 `--loop`，切终端敲验证命令时回放早已放完退出，看到的是"回放结束后"的假阴性结果，不是回放进行中的状态。加 `--loop` 后才稳定复现出 `Publisher count: 2`。
- **孤儿终端/未清理进程**：中途出现过一个多余的 `_ros2cli_` 订阅节点，是某个 `ros2 topic echo` 终端没有真正退出的残留——延续了 Week3 Day4 daemon 节点排查、Week5 Day3 环境污染排查里记过的同一类"先怀疑残留进程，再怀疑底层机制"的习惯。

1. **编译期链接 vs 运行期动态加载是两回事**：`colcon build` 时已经链接过 `libmoveit_move_group_interface.so`，但运行时报 `cannot open shared object file`——因为只 source 了 `ros2_ws` overlay，没 source `ws_moveit` underlay，`LD_LIBRARY_PATH` 里缺了库文件实际所在路径。跟"包索引找不到"（`AMENT_PREFIX_PATH`）是两套独立的环境变量机制，但病根同源：终端环境变量不完整。
2. **`robot_description` 参数超时**：直接跑 `pick_place_demo` 而没有先在另一个终端启动 Panda 仿真栈（`move_group`/`robot_state_publisher`/RViz2），`MoveGroupInterface` 构造时等 10 秒拿不到机器人模型直接报错退出。
3. **`moveit2_tutorials` 的 `demo.launch.py`（main 分支）已经不是 Panda demo 了**——现在默认配置的是 Kinova Gen3 + Robotiq 2F-85 夹爪（`mock_sensor_commands` 报错正是 Kinova 描述包 `kortex_description` 里的参数，与 Panda 无关）。改用 `moveit_resources_panda_moveit_config` 自带的 `demo.launch.py` 才是正确路径——这个包不受 `moveit2_tutorials` 改版影响，依然是完整的 Panda 配置。
4. **包名认知有误**：以为包名是 `panda_moveit_config`，实际 `package.xml` 里注册的真实名字是 `moveit_resources_panda_moveit_config`（带前缀）。之前用文件系统路径能直接读到 SRDF，从未暴露这个问题；这次改用 `ros2 pkg prefix`/`ros2 launch` 按包名查找才第一次撞上。
5. **夹爪 action 类型不匹配（本次最深的一处坑）**：Step1 开夹爪报 `Action client not connected to action server: panda_hand_controller/gripper_cmd`——控制器本身已正确加载激活，规划也成功，卡在"执行阶段把轨迹发给 action server"这一步。根因：`gripper_moveit_controllers.yaml` 里写的是旧式 `type: GripperCommand`，但 `ros2_controllers.yaml` 里 `panda_hand_controller` 实际类型是 `parallel_gripper_action_controller/GripperActionController`（Jazzy 起的新控制器），对外暴露的是 `control_msgs::action::ParallelGripperCommand`，与旧类型不匹配（`ros2 action list -t` 同时列出两个类型正是这个不匹配的直接证据）。这是 `panda_moveit_config` 官方配置包本身没跟上 MoveIt2 的 `parallel_gripper_action_controller` 迁移节奏（对应官方 PR moveit/moveit2#3260, Jazzy backport 2025-01）留下的遗留问题，不是本地环境或操作错误。修法：把 `gripper_moveit_controllers.yaml` 里 `panda_hand_controller` 的 `type` 改成 `ParallelGripperCommand`，重新编译该包并重启仿真栈。


## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
