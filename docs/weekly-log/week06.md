# Week 6：仿真环境 · 学习日志

> 日期：YYYY-MM-DD ~ YYYY-MM-DD

## 本周目标

-

## 学到了什么

- ## day 1
<link> 三件套：<visual>（外观，给人看）/ <collision>（碰撞体，给算法算，通常用简化几何体以降低 MoveIt2 规划、Gazebo 物理解算的开销）/ <inertial>（质量+惯性矩阵，规则形状可按公式手算）
两个 <origin> 的参考系不是一回事：<visual><origin> 是几何体相对自身 link 坐标系的偏移（"皮囊怎么贴"）；<joint><origin> 是子连杆坐标系相对父连杆坐标系的偏移（"骨架怎么接"，等价于 DH 参数里的几何安装距离）
joint 类型：revolute（有限位旋转）/ continuous（无限位旋转）/ fixed（刚性连接）/ prismatic（直线滑动），机械臂主关节基本用 revolute
URDF 本身不发布 tf：需要 robot_state_publisher 读取 URDF + 订阅 /joint_states，做正向运动学，才能把 link 关系转成 /tf（动态关节）和 /tf_static（fixed 连接）广播出去
robot_description 是完整 XML 文本，不是文件路径：这是为 XACRO 展开解耦设计的——launch 文件先把 .xacro/.urdf 渲染/读取成字符串，节点只管解析纯 XML，不需要自己集成 XACRO 引擎
包组织三层解耦：arm_interfaces（接口契约）/ arm_description（纯资产包，URDF/launch/rviz，ament_cmake 但不写 C++）/ arm_basics（业务逻辑节点）

## 卡在哪 / 怎么解决的

ros2 launch 报 joint_state_publisher_gui 找不到包 —— 该包不在 ros-jazzy-desktop 默认依赖里，需要单独装：
bash
   sudo apt install ros-jazzy-joint-state-publisher ros-jazzy-joint-state-publisher-gui
RViz2 报 Frame [base_links] does not exist —— Fixed Frame 手滑多打了一个 s，改成 base_link（跟 URDF 里 link 的 name 严格字符串匹配）
RobotModel 显示 Status: Ok 但视口空白 —— 排查发现是 QoS 不兼容：robot_state_publisher 发布 /robot_description 用的是 Transient Local（只发一次，指望晚订阅者补收），而 RobotModel 默认订阅 QoS 是 Volatile（不接收历史消息）——这正是上周 tf2 笔记里"Transient Local 发布者 + Volatile 订阅者：能连接但收不到历史"那条结论的复现。把 Durability Policy 改成 Transient Local 解决
QoS 改对后仍报错，Status: Error → Topic: Error subscribing: Empty topic name —— Description Topic 分组里最上面的 Topic 字段本身是空的（跟下面几行 QoS 参数是分开的字段），手填 /robot_description 后彻底解决

四步排查完，RViz2 成功渲染出机械臂模型，URDF parsed OK + 三个 link 的 Transform OK 全部通过。

## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
