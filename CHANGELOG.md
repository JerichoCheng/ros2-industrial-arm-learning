# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added

- ROS 2 parameters and launch overrides for `pick_pose` and `place_pose`.
- Default pose configuration in `config/pick_place_params.yaml`.
- Bounded, typed action-server readiness checks for `panda_arm_controller/follow_joint_trajectory` and `panda_hand_controller/gripper_cmd` before the first MoveIt execution request.
- Phase II roadmap and Week 1 evolution record.

### Changed

- The legacy Phase I plan now lives at `docs/learning-roadmap.md`.
- The gripper MoveGroup path refreshes its current state and start state before each named-target move.

### Fixed

- Invalid pick/place pose arrays are rejected unless they contain exactly seven values.
- Planning and execution failures are logged and cause a non-zero demo-node exit.
- Controller action-server availability, rather than the launch file's fixed six-second delay, is now the gate before trajectory execution.

### Known Issues

- The startup/readiness race reproduced during manual QA (the arm controller became active about 0.7 seconds after `move_group` appeared) is directly gated: the node waits up to ten seconds for both typed controller action servers and exits on timeout. This does not guarantee that a controller cannot deactivate after the preflight check, and runtime regression remains to be executed in a Jazzy + MoveIt environment.

## [0.1.0] — Simulated Pick-and-Place MVP

Phase I completed the eight-week ROS 2 and MoveIt learning project: C++ foundations, ROS communication, URDF/Gazebo and `ros2_control`, Panda MoveIt motion planning, debugging practice, and the simulated pick-and-place demo. See [docs/weekly-log/](docs/weekly-log/) and [docs/learning-roadmap.md](docs/learning-roadmap.md).
