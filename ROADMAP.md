# 8 周学习路线图

> 学习者背景：已有 Python / C 基础，C++ 零基础
> 项目方向：工业机械臂仿真抓取 + 轨迹规划
> 开发环境：Windows + WSL2 + Ubuntu 24.04 + ROS 2 Jazzy

## 总览

```
第0天      环境搭建（WSL2 + Ubuntu 24.04 + ROS 2 Jazzy）
第1-2周    C++ 速成（面向已有 C 基础）
第3周      ROS 2 核心体系：构建系统 / 功能包 / 节点
第4周      ROS 2 基础通信：topic / service + launch/参数
第5周      ROS 2 扩展通信：action / 自定义接口 / tf2
第6周      仿真环境：URDF + Gazebo + ros2_control
第7周      MoveIt2 运动规划入门
第8周      项目整合：抓取 Demo + 调试工具
```

---

## Week 0：环境搭建

- [ ] 安装 WSL2，发行版 Ubuntu 24.04 LTS
- [ ] 安装 ROS 2 Jazzy Desktop（`sudo apt install ros-jazzy-desktop`）
- [ ] VS Code + Remote-WSL + C/C++ 插件 + ROS 插件
- [ ] 配置 Git，建好本仓库
- [ ] 验证：`ros2 topic list` 正常、`rviz2` 能弹窗

**检查点**：`ros2 run demo_nodes_cpp talker` 和 `listener` 能正常通信。

---

## Week 1-2：C++ 速成

目标：具备"读懂并写出 ROS 2 C++ 代码"所需的最小能力集，重点是 C 里没有、但 ROS 2 代码里高频出现的部分（引用、RAII/智能指针、STL、继承多态、lambda、CMake）。

**Week 1 小项目**：`Sensor` 类 + `std::vector<std::shared_ptr<Sensor>>` 管理多个传感器实例 → 对应 [`cpp_warmup/week01_sensor_logger/`](./cpp_warmup/week01_sensor_logger/)

**Week 2 小项目**：继承体系 `TemperatureSensor : public Sensor`、`DistanceSensor : public Sensor`，纯手写 CMakeLists.txt 编译 → 对应 [`cpp_warmup/week02_sensor_hierarchy/`](./cpp_warmup/week02_sensor_hierarchy/)

**检查点**：能独立写出一个包含至少两层继承、用 `shared_ptr` 管理对象、用 lambda 做回调的 C++ 小程序，手写 CMakeLists.txt 编译成功。

---

## Week 3：ROS 2 核心体系

- 创建第一个 `ros2_ws` workspace（`ros2_ws/src/`）
- `ros2 pkg create` 创建 C++ 功能包
- 手写节点类（继承 `rclcpp::Node`），`colcon build` + `ros2 run`
- `ros2 node list` / `ros2 node info` 观察节点

**检查点**：功能包能编译、节点能跑、能被 `ros2 node info` 查到。

---

## Week 4：基础通信 + 启动与参数

- topic 发布订阅、QoS 基础
- service 服务端/客户端
- launch 脚本、参数系统（YAML）

**检查点**：独立写出"发布者+订阅者+service+launch 文件"最小组合，`ros2 launch` 一键拉起。

---

## Week 5：扩展通信 + 坐标变换

- action 通信（服务端/客户端、反馈/状态/结果）
- 自定义接口（`.msg`/`.srv`/`.action`）
- tf2 坐标系统（静态/动态坐标变换）

**检查点**：能解释 topic / service / action 的区别，独立写出 action 服务端-客户端 demo。

---

## Week 6：仿真环境（教材外，扩展模块）

- URDF 建模：`robot_state_publisher` + `joint_state_publisher_gui` 可视化机械臂骨架
  - 建议直接用 MoveIt2 官方教程自带的 Franka Panda 模型，不从零设计 URDF
- Gazebo 仿真：spawn 模型进 Gazebo Harmonic
- ros2_control：`joint_trajectory_controller` 基础

**风险提示**：WSL2 下 Gazebo/GPU 渲染容易卡顿，退路是只做 RViz2 + MoveIt2 的虚拟规划场景，不做物理仿真。

**检查点**：RViz2 里能看到可拖动关节的骨架，Gazebo 里能 spawn 出来，能通过命令行让某关节转动。

---

## Week 7：MoveIt2 运动规划入门

- 安装 MoveIt2，跟随官方 Getting Started 教程
- 理解 planning group、`move_group` 接口
- RViz2 Motion Planning 插件手动规划
- 写 C++ 节点，用 `MoveGroupInterface` 编程方式触发运动规划

**检查点**：自己写的 C++ 节点能让 Panda 在仿真里移动到指定位姿。

---

## Week 8：项目整合

- GDB 调试、ros2_tracing 了解
- rosbag2 录制/回放
- `rqt_graph`、`ros2 topic echo` 排查连通性

**最终产出**：
- [ ] C++ 节点：预抓取位姿 → 闭合夹爪 → 放置位姿 → 张开夹爪
- [ ] 一键 launch 文件
- [ ] 项目 README（结构、运行方式、踩坑记录）
- [ ] 演示视频（30-60 秒，建议放进简历/作品集）

---

## 8 周后的深化方向（可选）

- 结合 RGB-D 相机做真实物体识别定位
- 更复杂轨迹规划：避障、笛卡尔路径、MoveIt Task Constructor
- 换成 UR5e 等更贴近工业场景的机械臂
- 补教材剩余章节：插件系统、组件系统、生命周期节点、单元测试
- 实物部署（真实机械臂 / Jetson）
- 给项目加单元测试 + GitHub Actions CI + 代码规范检查

---

## 学习方法提醒

- 每天结束留 10 分钟写"今天卡在哪"（对应 `docs/weekly-log/`）
- 优先看官方文档（docs.ros.org、moveit.picknik.ai），教材成书较早，命令可能已随版本更新
- Week 1-2 不用纠结完全搞懂指针/引用底层机制，够用就行
- 卡住超过 1 小时就换思路：看别人代码、退回上一版本，或先记录到"待解决清单"
