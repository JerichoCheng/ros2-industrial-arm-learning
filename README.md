# ROS 2 Industrial Arm Learning Journey — From Zero C++ to a Simulated Grasping Demo

> Documenting my 8-week journey from zero C++ experience to building a simplified simulated industrial-arm grasping demo with ROS 2.
> Textbook: *ROS 2 Robot Programming in Practice — Modern C++ and Python 3* · Environment: Windows + WSL2 + Ubuntu 24.04 + ROS 2 Jazzy

[![Progress](https://img.shields.io/badge/progress-week%200%2F8-lightgrey)](./ROADMAP.md)
[![ROS2](https://img.shields.io/badge/ROS%202-Jazzy-blue)](https://docs.ros.org/en/jazzy/)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

---

## What this repo is

This isn't a "copy the tutorial" code dump — it's a **learning log with process attached**: what I did each week, where I got stuck, and how I worked through it, all recorded in `docs/weekly-log/`. The end goal is a working simulated arm grasping demo, and this repo doubles as a portfolio piece for my transition from C to C++ to ROS 2.

- Full 8-week plan: [ROADMAP.md](./ROADMAP.md)
- Weekly learning logs: [docs/weekly-log/](./docs/weekly-log/)
- Reference resources: [docs/resources.md](./docs/resources.md)

## Progress

| Week | Topic | Status | Output |
|---|---|---|---|
| Week 0 | Environment setup (WSL2 + ROS 2 Jazzy) | ✅ Done | — |
| Week 1-2 | C++ crash course | 🟨 In progress | [`cpp_warmup/`](./cpp_warmup/) |
| Week 3 | ROS 2 core: packages / nodes | ⬜ Not started | [`ros2_ws/src/`](./ros2_ws/src/) |
| Week 4 | Basic communication: topic / service / launch / params | ⬜ Not started | — |
| Week 5 | Extended communication: action / custom interfaces / tf2 | ⬜ Not started | — |
| Week 6 | Simulation: URDF / Gazebo / ros2_control | ⬜ Not started | — |
| Week 7 | Intro to MoveIt2 motion planning | ⬜ Not started | — |
| Week 8 | Project integration: grasping demo | ⬜ Not started | — |

> Legend: ⬜ Not started · 🟨 In progress · ✅ Done — check these off here as each week wraps up. The progress bar doesn't lie.

## Repo structure

```
.
├── README.md                  # you are here
├── ROADMAP.md                 # full 8-week plan + checkpoints
├── docs/
│   ├── weekly-log/            # weekly logs (goals/content/blockers/takeaways)
│   └── resources.md           # reference resource list
├── cpp_warmup/                # Weeks 1-2: C++ crash-course exercises (no ROS 2 dependency)
│   ├── week01_sensor_logger/
│   └── week02_sensor_hierarchy/
├── ros2_ws/                   # from Week 3 on: the actual ROS 2 colcon workspace
│   └── src/                   # packages get added here as the project progresses
└── .github/workflows/ci.yml   # simple build check (optional)
```

## Final demo goal (Week 8)

- [ ] A C++ node: move to pre-grasp pose → close gripper → move to place pose → open gripper
- [ ] A single launch file that brings up the simulation + MoveIt2 + the grasping node
- [ ] A demo video (30-60 seconds)

## How to run (will be updated as I go)

```bash
# Weeks 1-2: C++ crash-course exercises, hand-written CMake build, no ROS 2 dependency
cd cpp_warmup/week01_sensor_logger
mkdir build && cd build && cmake .. && make
./sensor_logger

# From Week 3 on: the ROS 2 workspace
cd ros2_ws
colcon build
source install/setup.bash
ros2 launch <your_package> <your_launch_file>.launch.py
```

## License

[MIT](./LICENSE) — do whatever you want with the code. The textbook content is copyrighted by its original author and is not reproduced in this repo.
