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

- ## day 2
- xacro:property：把 base/link1/link2 的半径、高度、质量提取为具名常量，文件里不再有裸露的物理尺寸数字。
- ${} 数学表达式：origin、geometry 的数值全部用表达式（如 ${link1_h/2}）实时计算，而不是手算好填值。
- xacro:macro：把重复的圆柱惯性张量公式收拢成 cylinder_inertial_z 一份定义，参数为 m r h *origin。
- block parameter（*origin）：与普通参数（XML 属性）传参方式完全不同——必须以子元素形式塞进调用标签内部。origin 的决定权交还给调用者（每个 link 自己指定质心偏移），而不是在宏内部硬编码，因为宏内部硬编码隐含了"该 link 一定沿 local z 轴延伸"的假设。

- ## day 3
- <collision> vs <visual> 的语义区别：<visual> 是给人看的渲染外观，<collision> 是给物理引擎/规划器算碰撞检测用的包围体，两者数据结构一样但用途完全不同
- collision 简化的分层原则：当前简单几何体(圆柱)场景下，collision 应该 1:1 原样复刻 visual——故意夸大会导致关节极限处自碰撞误报，故意内缩会导致抓取穿模；真实工业机械臂(不规则 mesh)才需要退化成凸包+安全边距，且安全边距更适合放在 MoveIt2 的 SRDF/ACM 层动态配置，而不是写死在 URDF 几何体里
零位自碰撞风险：如果 collision 体比 visual 厚且在 joint 原点附近重叠，哪怕关节角度是合法的 0 位，物理引擎在第一帧就会报自碰撞，机械臂还没开始动就"僵死"
- 宏该封装什么：cylinder_inertial_z 值得抽成宏，是因为它封装的是"容易算错的数学公式"；<visual>/<collision> 的 geometry+origin 只是平铺数据，没有出错风险的计算逻辑，因此故意选择手写三遍而不是再包一层宏——宏该封装计算逻辑，不是纯数据结构，且两者的未来演化方向不同（visual 会加材质/换 mesh，- - collision 只会简化，绑在一起会破坏独立演化空间）
- 差速轮插件 vs 机械臂控制架构的本质区别：
控制维度：差速轮是开环线速度/角速度驱动；机械臂需要全关节同步的位置/速度/加速度高精度轨迹跟踪
接口协议：差速轮插件直接订阅 Twist 操纵轮子；机械臂走标准化的 ros2_control（gazebo_ros2_control 接管硬件抽象层，JointTrajectoryController 通过 FollowJointTrajectory action 接收整条时间序列轨迹）
物理驱动：差速轮是运动学速度覆写；机械臂受重力/惯性影响，需要位置闭环 PID 或力矩(Effort)控制，PID 增益配在 ros2_control 控制器层的 YAML 里，不在 Gazebo 插件内部——这个硬件无关的控制层和硬件相关的驱动接口分离，正是后续从仿真迁移到真实 UR 臂能省下大量工作的原因

- ## day 4
一、概念学习：ros2_control 三层架构
controller_manager：管理者节点，负责加载/激活/卸载 controller，按固定频率驱动 update 循环
Controller（如 joint_trajectory_controller）：软件层，负责"做什么"——把一条时间戳+角度序列的轨迹，实时插值成每个控制周期该发的具体数值
Hardware Interface（如 gz_ros2_control）：硬件抽象层，负责"怎么做"——把插值结果通过 command_interface 写给电机（或仿真中的 Gazebo 关节），并通过 state_interface 把真实反馈读回来

关键结论：<ros2_control> 标签里声明的是硬件能力契约（能接受什么指令、能读到什么反馈）， 不是轨迹格式——两者不能互换，否则一旦换一种不基于轨迹的控制器（如点动控制），硬件层就要跟着重写。

command_interface / state_interface

每个 <joint> 需要同时声明双向通道：

command_interface（controller → 硬件，如 position）
state_interface（硬件 → controller，如 position + velocity）

velocity 反馈的作用：闭环阻尼判断、到位静止判定（zero-velocity check）、以及轨迹被抢占/急停时 新样条的初始边界条件（避免加速度突变造成冲击）。

<ros2_control> 与 <gazebo> 的职责分离
<ros2_control>：纯硬件抽象规范，与仿真器无关，未来换真实电机时这段基本不用改
<gazebo><plugin>：仿真器专属配置，声明"用哪个 .so 插件来实例化 controller_manager 并连接到 Gazebo 模型"
arm_controllers.yaml：controller_manager 的标准 ROS2 参数文件（复用 Week4 学过的 YAML 语法）， 声明要加载哪些 controller、每个的类型，以及各 controller 自己的 joints/command_interfaces/state_interfaces ——这些必须和 <ros2_control> XML 里声明的完全对齐,是一种"资源借用"契约(claim 机制)
joint_state_broadcaster

新流水线里 /joint_states 改由 joint_state_broadcaster（而不是 Day1 用的 joint_state_publisher_gui） 发布——它直接从硬件层的 state_interface 读真实反馈，robot_state_publisher 订阅它算出真实 /tf。 两者不能共存，否则同一话题多个发布源会导致 RViz/Gazebo 里模型抖动（"脑裂"）。

Launch 因果链

Gazebo 世界启动 → 模型 spawn（Gazebo 插件内部才会实例化 controller_manager）→ joint_state_broadcaster 激活（状态层先就绪）→ arm_controller 激活（执行层最后接入， 需要先能读到真实状态才能安全 claim 控制权）。用 RegisterEventHandler(OnProcessExit(...)) 串联，而不是像 Week4 那样把 Node 并排堆在一起——因为 controller_manager 是被 Gazebo 插件 动态创建的，不是自己起的 Node。

二、环境适配：新版 Gazebo（Harmonic）vs 经典 Gazebo

系统确认走的是 ros_gz_sim/ros_gz_bridge/gz_ros2_control 这一套新生态，与教材/大多数 网络教程默认的经典 Gazebo（gazebo_ros/gazebo_ros2_control）语法不同：

项目	经典 Gazebo	新版 Gazebo(Harmonic,本机环境)
启动 launch	gazebo.launch.py	gz_sim.launch.py(来自 ros_gz_sim)
spawn 可执行文件	spawn_entity.py	create
硬件插件类型	gazebo_ros2_control/GazeboSystem	gz_ros2_control/GazeboSimSystem
Gazebo 插件 filename/name	libgazebo_ros2_control.so / gazebo_ros2_control	libgz_ros2_control-system.so / gz_ros2_control::GazeboSimROS2ControlPlugin

新增依赖：新版 Gazebo 不会自动把仿真时钟同步给 ROS2（经典版会），任何标了 use_sim_time: true 的节点都收不到时间，必须显式加一条桥接：

python
Node(package='ros_gz_bridge', executable='parameter_bridge',
     arguments=['/clock@rosgraph_msgs/msg/Clock[ignition.msgs.Clock'])


## 卡在哪 / 怎么解决的

ros2 launch 报 joint_state_publisher_gui 找不到包 —— 该包不在 ros-jazzy-desktop 默认依赖里，需要单独装：
bash
   sudo apt install ros-jazzy-joint-state-publisher ros-jazzy-joint-state-publisher-gui
RViz2 报 Frame [base_links] does not exist —— Fixed Frame 手滑多打了一个 s，改成 base_link（跟 URDF 里 link 的 name 严格字符串匹配）
RobotModel 显示 Status: Ok 但视口空白 —— 排查发现是 QoS 不兼容：robot_state_publisher 发布 /robot_description 用的是 Transient Local（只发一次，指望晚订阅者补收），而 RobotModel 默认订阅 QoS 是 Volatile（不接收历史消息）——这正是上周 tf2 笔记里"Transient Local 发布者 + Volatile 订阅者：能连接但收不到历史"那条结论的复现。把 Durability Policy 改成 Transient Local 解决
QoS 改对后仍报错，Status: Error → Topic: Error subscribing: Empty topic name —— Description Topic 分组里最上面的 Topic 字段本身是空的（跟下面几行 QoS 参数是分开的字段），手填 /robot_description 后彻底解决
四步排查完，RViz2 成功渲染出机械臂模型，URDF parsed OK + 三个 link 的 Transform OK 全部通过。

block parameter 传参方式：第一次尝试沿用普通参数的属性写法（<xacro:cylinder_inertial_z ... origin="..."/>），语法不通，必须改成子元素形式。
隐藏假设风险：若宏内部把 origin 和惯性公式都写死为"沿 z 轴"，未来遇到横向安装的连杆会导致数值错误但不会报错——最危险的一类 bug。解决方式是把宏命名为 cylinder_inertial_z，显式标出假设，并把 origin 交给调用者。
同文件内的参数耦合：joint2 的安装高度依赖 link1_h，一开始也是硬编码字面量；顺手一并改成 ${link1_h} 引用，避免跨标签的数值不同步。

坑1：多根链导致 robot_state_publisher 硬崩溃（SIGABRT / exit code -6）

一开始只写了 3 个 <link>,忘记补 <joint> 把它们连起来。这不是普通 XML 语法错误 （那种会被 DOM 解析层优雅捕获、打印报错），而是触发了 kdl_parser 内部构建运动学树时 的一个底层断言（要求根节点数量恰好为 1）——断言失败直接调用 abort(),连日志都来不及打印, 进程原地暴毙。教训：碰到 exit code -6 类型的硬崩溃，要往"底层库断言失败"而不是 "上层可恢复异常"这个方向排查。

坑2：<plugin> 没被 <gazebo> 包裹，导致 gz_ros2_control 插件被静默丢弃（全程无任何报错）

这是今天耗时最长的一次排查。现象：spawner 一直报 "Could not contact service /controller_manager/list_controllers"，但 Gazebo 本身、spawn_entity、robot_state_publisher 全部正常，日志里从头到尾没有任何一行提到 ros2_control/gazebo_sim 相关的报错或警告—— 一次彻底的"沉默失败"。

排查路径：

确认插件 .so 文件确实存在于磁盘（find 验证）→ 排除"缺包"
用 gz sdf -p 把 xacro 展开后的 URDF 转换成 SDF 直接查看 → 发现 <plugin> 标签 完全没有出现在转换结果里
用最小可复现例子 + 二分法逐个排除变量：命名空间分隔符 ::、长路径参数、 多 link/joint 结构、<material> 标签——全部依次验证正常，均不是原因
最后用 diff 逐行比对"能正常工作的最小例子"和"真实文件展开结果"，才发现： 真实文件里 <plugin> 直接挂在 <robot> 下面,外层缺了 <gazebo>...</gazebo> 包裹 （大概率是之前几轮手动改插件名字时，标签配对不小心弄丢了）

教训：sdformat 的 URDF→SDF 转换器对于自己不认识的标签（包括裸露的 <plugin>、 自定义的 <ros2_control>）,采取的是"静默跳过、不报任何错"的策略。这意味着"没有报错" 不能作为"配置正确"的证据——遇到"应该生效但完全没反应"的情况，第一步就应该用 gz sdf -p <expanded.urdf> 直接检查转换结果里对应内容是否存在，而不是继续在 运行时日志里找线索。

## 检查点是否通过

- [x]

## 代码/产出链接

-

## 下周计划微调

-
