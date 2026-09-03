# 8-Week Learning Roadmap

> Background: solid Python / C, zero C++ experience
> Project direction: industrial robotic arm — simulated grasping + trajectory planning
> Dev environment: Windows + WSL2 + Ubuntu 24.04 + ROS 2 Jazzy
> Primary textbook: *ROS 2 Robot Programming in Practice — Modern C++ and Python 3* (Textbook ①, main reference for Weeks 3-5 and 8)
> Supplementary textbook: *ROS 2 Intelligent Robot Development in Practice* (Textbook ②, main reference for Week 6 URDF/Gazebo simulation; Ch2 communication sections useful as a cross-reference; Ch7 vision/YOLO used in the post-8-week deepening track)
>
> Textbook ② is fundamentally a mobile-robot (wheeled-base) textbook, not an arm textbook. Its Ch5-6 (motor drivers, encoders, differential-drive kinematics) and Ch8-9 (SLAM, Nav2 navigation) aren't relevant to the arm and won't be used in the 8-week plan.

## Overview

```
Day 0        Environment setup (WSL2 + Ubuntu 24.04 + ROS 2 Jazzy)
Week 1-2     C++ crash course (building on existing C background)
Week 3       ROS 2 core concepts: build system / packages / nodes
Week 4       ROS 2 basic communication: topic / service + launch/params
Week 5       ROS 2 extended communication: action / custom interfaces / tf2
Week 6       Simulation environment: URDF + Gazebo + ros2_control (Textbook ② Ch4-led)
Week 7       Intro to MoveIt2 motion planning
Week 8       Project integration: grasping demo + debugging tools
```

---

## Week 0: Environment Setup

- [ ] Install WSL2, distro Ubuntu 24.04 LTS
- [ ] Install ROS 2 Jazzy Desktop (`sudo apt install ros-jazzy-desktop`)
- [ ] VS Code + Remote-WSL + C/C++ extension + ROS extension
- [ ] Configure Git, set up this repo
- [ ] Verify: `ros2 topic list` works, `rviz2` opens a window

**Checkpoint**: `ros2 run demo_nodes_cpp talker` and `listener` can communicate normally.

---

## Week 1-2: C++ Crash Course

Goal: build the minimum C++ skill set needed to read and write ROS 2 C++ code, focusing on what C doesn't have but ROS 2 code uses constantly (references, RAII/smart pointers, STL, inheritance & polymorphism, lambdas, CMake).

**Week 1 mini-project**: `Sensor` class + `std::vector<std::shared_ptr<Sensor>>` managing multiple sensor instances → [`cpp_warmup/week01_sensor_logger/`](./cpp_warmup/week01_sensor_logger/)

**Week 2 mini-project**: inheritance hierarchy `TemperatureSensor : public Sensor`, `DistanceSensor : public Sensor`, hand-written CMakeLists.txt → [`cpp_warmup/week02_sensor_hierarchy/`](./cpp_warmup/week02_sensor_hierarchy/)

**Checkpoint**: independently write a C++ program with at least two levels of inheritance, using `shared_ptr` to manage objects and a lambda as a callback, and compile it successfully with a hand-written CMakeLists.txt.

---

## Week 3: ROS 2 Core Concepts

- Create your first `ros2_ws` workspace (`ros2_ws/src/`)
- Create a C++ package with `ros2 pkg create`
- Hand-write a node class (inheriting `rclcpp::Node`), `colcon build` + `ros2 run`
- Inspect nodes with `ros2 node list` / `ros2 node info`

**Optional cross-reference**: when Textbook ① moves too fast on something, Textbook ② Ch2.1-2.4 (workspace/package/node, covered in both Python and C++) offers another angle.

**Checkpoint**: the package builds, the node runs, and it shows up in `ros2 node info`.

---

## Week 4: Basic Communication + Launch & Parameters

- Topic pub/sub, basic QoS
- Service server/client
- Launch scripts, parameter system (YAML)

**Optional cross-reference**: Textbook ② Ch2.5-2.6 (topics/services), Ch2.9 (parameters), Ch3.1 (Launch) — all covered in both Python and C++.

**Checkpoint**: independently write a minimal "publisher + subscriber + service + launch file" combo that comes up in one shot via `ros2 launch`.

---

## Week 5: Extended Communication + Coordinate Transforms

- Action communication (server/client, feedback/status/result)
- Custom interfaces (`.msg`/`.srv`/`.action`)
- tf2 coordinate system (static/dynamic transforms)

**Optional cross-reference**: Textbook ② Ch2.8 (actions), Ch3.2 (tf frame management, including static/dynamic broadcasting and listening in C++) — this section specifically uses "industrial-robot base frame / tool frame / object frame" as its example, which fits the arm project better than Textbook ①. Worth reading.

**Checkpoint**: can explain the difference between topic / service / action, and independently write an action server-client demo.

---

## Week 6: Simulation Environment (Textbook ② Ch4 + extensions)

The goal this week is to bring an arm "to life" in simulation. **URDF/XACRO/Gazebo now has a proper textbook reference — use Textbook ② Chapter 4** instead of figuring it out from scratch; `ros2_control` still isn't covered in depth by either textbook, so that part remains self-directed.

- Textbook ② Ch4.2-4.3: URDF modeling basics — link/joint syntax. The book's examples use a mobile robot, but the syntax is generic and transfers directly to an arm's structure (a chain of revolute/continuous joints)
- Textbook ② Ch4.4: XACRO optimization — using variables and macros to cut down repetitive URDF code, which saves real time given how many joints an arm has
- Textbook ② Ch4.5-4.6: filling in physical parameters, adding controller plugins, loading the model into Gazebo, motion-control simulation
  - **Suggestion**: once you've learned the syntax and workflow, don't model an arm from scratch — use the Franka Panda model bundled with the official MoveIt2 tutorials, or the examples in the `moveit2_tutorials` repo. The textbook teaches you to read/modify a model, not draw one from zero; spend the saved time on understanding concepts instead of tweaking coordinates
- `ros2_control` basics (self-directed, neither textbook covers this in depth): understand what `joint_trajectory_controller` does, and be able to send a joint a motion command from the command line and see it move in Gazebo

**Risk note**: Gazebo/GPU rendering tends to lag under WSL2. The fallback is to do everything as a virtual planning scenario in RViz2 + MoveIt2, skipping physics simulation entirely.

**Checkpoint**: RViz2 shows a draggable-joint skeleton, the model spawns in Gazebo, and a joint can be moved via command line.

---

## Week 7: Intro to MoveIt2 Motion Planning

- Install MoveIt2, follow the official Getting Started tutorial
- Understand planning groups and the `move_group` interface
- Manual planning via the RViz2 Motion Planning plugin
- Write a C++ node that triggers motion planning programmatically using `MoveGroupInterface`

**Checkpoint**: your own C++ node can move the Panda to a specified pose in simulation.

---

## Week 8: Project Integration

- GDB debugging, a look at ros2_tracing
- rosbag2 recording/playback
- Troubleshoot connectivity with `rqt_graph`, `ros2 topic echo`

**Final deliverable**:
- [ ] A C++ node: pre-grasp pose → close gripper → place pose → open gripper
- [ ] A single launch file that brings everything up
- [ ] Project README (structure, how to run, lessons learned)
- [ ] A demo video (30-60 seconds, worth keeping for your resume/portfolio)

---

## Post-8-Week Deepening Directions (optional)

- More sophisticated trajectory planning: obstacle avoidance, Cartesian paths, MoveIt Task Constructor
- **Real grasping (visual perception) — now has a concrete textbook reference**: Textbook ② Ch7 systematically covers camera calibration (7.2), using OpenCV in ROS 2 (7.3), and deep-learning/YOLO object detection (7.6-7.7). Working through these in order gets you a full "camera detects object position → feeds it to MoveIt2 for grasp planning" pipeline
- Swap the Panda for a more "industrial" arm like the UR5e
- Fill in remaining chapters of Textbook ①: plugin system, component system, lifecycle nodes, unit testing
- Real-world deployment (a physical arm / Jetson)
- Add unit tests + GitHub Actions CI + linting to the project
- **Optional: extend into mobile robotics / mobile manipulation**: if you later want the arm "on the move" (mounted on a mobile base for mobile grasping), Textbook ② has a full path ready — Ch5-6 (motor drivers/encoders/differential-drive kinematics, hardware-facing), Ch8 (SLAM mapping), Ch9 (Nav2 autonomous navigation). This is its own substantial direction — worth planning separately once the 8-week MVP is done and you're genuinely interested

---

## Study Method Reminders

- Spend 10 minutes at the end of each day writing down "what I got stuck on" (goes in `docs/weekly-log/`)
- Prefer official docs (docs.ros.org, moveit.picknik.ai) — the textbooks were written earlier and commands may have shifted since
- Don't get hung up in Week 1-2 on fully understanding the low-level mechanics of pointers/references — good enough is good enough
- If you're stuck for more than an hour, switch approach: read someone else's code, roll back to the last working version, or log it to a "to resolve" list
