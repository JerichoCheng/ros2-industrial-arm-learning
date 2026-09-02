# ROS 2 工业机械臂学习之路 · 从零 C++ 到仿真抓取 Demo

> 记录我从零 C++ 基础出发，用 8 周时间学习 ROS 2 并做出一个简化版工业机械臂仿真抓取 Demo 的完整过程。
> 教材：《ROS 2 机器人编程实战——基于现代 C++ 和 Python 3》 · 环境：Windows + WSL2 + Ubuntu 24.04 + ROS 2 Jazzy

[![Progress](https://img.shields.io/badge/progress-week%200%2F8-lightgrey)](./ROADMAP.md)
[![ROS2](https://img.shields.io/badge/ROS%202-Jazzy-blue)](https://docs.ros.org/en/jazzy/)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

---

## 这个仓库是什么

这不是一个"抄教程"的代码堆砌仓库，而是一份**带过程的学习记录**：每周做了什么、卡在哪、怎么解决的，都写进 `docs/weekly-log/`。最终目标是一个能跑起来的仿真机械臂抓取 Demo，也是我从 C 转 C++ 再到 ROS 2 的学习作品集。

- 完整 8 周计划见 [ROADMAP.md](./ROADMAP.md)
- 每周学习日志见 [docs/weekly-log/](./docs/weekly-log/)
- 参考资源清单见 [docs/resources.md](./docs/resources.md)

## 进度总览

| 周次 | 主题 | 状态 | 产出 |
|---|---|---|---|
| Week 0 | 环境搭建（WSL2 + ROS 2 Jazzy） | ✅ 已完成 | — |
| Week 1-2 | C++ 速成（面向 C 基础） | ⬜ 未开始 | [`cpp_warmup/`](./cpp_warmup/) |
| Week 3 | ROS 2 核心体系：功能包 / 节点 | ⬜ 未开始 | [`ros2_ws/src/`](./ros2_ws/src/) |
| Week 4 | 基础通信：topic / service / launch / 参数 | ⬜ 未开始 | — |
| Week 5 | 扩展通信：action / 自定义接口 / tf2 | ⬜ 未开始 | — |
| Week 6 | 仿真环境：URDF / Gazebo / ros2_control | ⬜ 未开始 | — |
| Week 7 | MoveIt2 运动规划入门 | ⬜ 未开始 | — |
| Week 8 | 项目整合：抓取 Demo | ⬜ 未开始 | — |

> 状态图例：⬜ 未开始 · 🟨 进行中 · ✅ 已完成 — 每完成一周就来这里勾一下，进度条骗不了人。

## 仓库结构

```
.
├── README.md                  # 你在这里
├── ROADMAP.md                 # 完整 8 周计划 + 检查点
├── docs/
│   ├── weekly-log/            # 每周学习日志（目标/内容/卡点/收获）
│   └── resources.md           # 参考资源清单
├── cpp_warmup/                # 第 1-2 周：C++ 速成练习（不依赖 ROS 2）
│   ├── week01_sensor_logger/
│   └── week02_sensor_hierarchy/
├── ros2_ws/                   # 第 3 周起：正式的 ROS 2 colcon 工作空间
│   └── src/                   # 各功能包会陆续加在这里
└── .github/workflows/ci.yml   # 简单的构建检查（可选）
```

## 最终 Demo 目标（Week 8）

- [ ] 一个 C++ 节点：移动到预抓取位姿 → 闭合夹爪 → 移动到放置位姿 → 张开夹爪
- [ ] 一个 launch 文件一键拉起仿真环境 + MoveIt2 + 抓取节点
- [ ] 演示视频（30-60 秒）

## 如何运行（会随进度更新）

```bash
# Week 1-2：C++ 速成练习，纯手写 CMake 编译，不依赖 ROS 2
cd cpp_warmup/week01_sensor_logger
mkdir build && cd build && cmake .. && make
./sensor_logger

# Week 3 起：ROS 2 工作空间
cd ros2_ws
colcon build
source install/setup.bash
ros2 launch <your_package> <your_launch_file>.launch.py
```

## License

[MIT](./LICENSE) — 代码随便用，教材内容版权归原作者所有，本仓库不包含教材原文。
