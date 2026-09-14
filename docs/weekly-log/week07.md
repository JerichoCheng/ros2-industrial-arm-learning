# Week 7：MoveIt2 运动规划 · 学习日志

## 本周目标

- 从"手写角度让机械臂转"进阶到"给目标位姿，MoveIt2 自动算怎么转"。跑通第一次运动规划（RViz 手动 + C++ 编程两种方式），checkpoint 是自己写的 C++ 节点让机械臂移动到指定位姿。

## 关键决策：切换到官方 Panda，而非沿用 Week6 的 simple_arm

Week6 是自建的 2 关节 simple_arm。Week7 改用官方 Franka Panda（通过 moveit2_tutorials + moveit_resources_panda_moveit_config），原因：

2 关节臂做运动规划展示效果有限，末端位姿的自由度约束容易不满足
官方 Panda 是 7 自由度，配置（SRDF、kinematics、controllers）都是现成的，不用先过一遍 MoveIt Setup Assistant
跟官方 Getting Started 教程对齐，踩坑少

代价：项目模型从 Week1-6 的自建臂换成了 Panda，连贯性上有个断层——这是刻意的权衡，之后如果接手真实机械臂（UR5e 等）本来就要重新走一遍 Setup Assistant，Panda 阶段先练手法。

## 学到了什么

- ## Day 1
- move_group 节点是运动规划的中枢：接收规划请求 → 用 OMPL 等算法算出无碰撞轨迹 → 把轨迹当 FollowJointTrajectory action 目标发给 controller 执行——执行这一步复用的正是 Week5 写过的 action、Week6 接过的 joint_trajectory_controller，MoveIt2 没有另起炉灶。
- SRDF 是 URDF 之上的语义层，核心是定义 planning group（比如 panda_arm = 7 个旋转关节联动求解）。
- 正运动学（FK，关节角→末端位姿）唯一确定；逆运动学（IK，末端位姿→关节角）对 7 自由度臂通常有无穷多解（冗余自由度）——这是 IK 无解/多解、以及"拖动末端会画出一条圆弧"等现象的根源。

- ## Day 2
- 拖拽末端交互控制柄 → Plan（只做规划，画面播放的是循环预览，不是真实运动）→ Execute（才是真正把轨迹发给 controller 执行）。
- 拖到不可达区域时控制柄实时变红（IK 求解失败的实时反馈，不需要点 Plan 才知道）。
- 切到 Joints 面板直接拖单个关节滑动条，末端画出一段圆弧——对比出"直接控制关节角（FK 唯一确定）"与"直接控制末端位姿（IK 多解/冗余自由度）"的区别。

## 卡在哪 / 怎么解决的

moveit2_tutorials clone 分支错误：该仓库没有按 ROS 版本单独开分支，Jazzy 用户应该用 main（不是 jazzy）。
pybind11 版本不匹配：moveit_task_constructor_core 的最新代码要求 pybind11 ≥3.0，但 Ubuntu 24.04 apt 源里的是 2.11.1。解法：sudo python3 -m pip install --break-system-packages "pybind11[global]"，装到系统级 CMake 能找到的位置，不影响已编译产物。
demo.launch.py xacro 参数不匹配（Invalid parameter "mock_sensor_commands"）：moveit2_tutorials（追最新代码）跟 moveit_resources（独立更新节奏）之间版本没完全对齐，是社区里反复出现的已知问题。绕开方式：改用 moveit_resources_panda_moveit_config 包自带的 demo.launch.py（跟 Panda 配置同源、天然对齐），而不是 moveit2_tutorials 的包装版。
WSLg 图形完全不显示：进程活着、日志正常，但任何窗口（包括最简单的 xeyes）都画不出来。排查链条：安全对话框（排除）→ RDP GFX 通道解码持续报错 0x80070490 → 根因是 Windows 侧 NVIDIA 独显驱动版本/状态问题。更新独显驱动（干净安装）+ 完整重启 Windows 解决,顺带确认了 RTX 3060 Laptop 对这门课的图形需求完全够用。
first_motion.cpp 编译报出一长串 _M_max_size/operator new 相关的标准库内部错误：表面像 GCC/libstdc++ 版本冲突，实际根因是翻译单元里用到了 placement new（std::vector/shared_ptr 内部大量使用），但没有任何路径间接引入 <new> 头文件。加一行 #include <new> 解决。
热身插曲：用 cat > file << 'EOF' ... EOF 写文件时，结尾的 EOF 终止符字面意义上被写进了文件内容里，导致编译器语法解析全乱、报出一堆看似跟 tf2/boost 相关但实际无意义的连锁错误。教训：heredoc 终止符最好用不容易和内容混淆的独特字符串（比如 MOVEIT_EOF）。

## 检查点是否通过

- [x]

## 代码/产出链接

-

## 下周计划微调

-
