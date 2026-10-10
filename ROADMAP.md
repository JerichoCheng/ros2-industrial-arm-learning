# Project Roadmap

## Phase I — ROS 2 Industrial Arm Learning

**Status:** Completed
**Release:** [`v0.1.0`](https://github.com/JerichoCheng/ros2-industrial-arm-learning/releases/tag/v0.1.0)

The original eight-week study plan is preserved in [docs/learning-roadmap.md](docs/learning-roadmap.md). Day-by-day implementation notes are in [docs/weekly-log/](docs/weekly-log/).

## Phase II — Robotics Systems Development

**Budget:** approximately 15 hours per week.

### Week 1 — Execution Reliability & Parameterized Targets (active)

- Parameterize `pick_pose` and `place_pose`.
- Expose pose overrides through launch arguments.
- Validate that every pose parameter contains seven elements.
- Detect planning failures and execution failures.
- Return non-zero when the demo node fails.
- Stabilize gripper `MoveGroupInterface` execution with a fresh current state.
- Add reliable controller-action readiness gating before trajectory execution.
- Run final default, planning-failure, and execution-failure regression checks.

### Week 2 — Task State Machine & Failure Recovery

Define explicit manipulation task states, transitions, and recoverable failure paths.

### Week 3 — Testing, Logging & CI

Add automated tests, structured diagnostics, and repeatable continuous integration checks.

### Week 4 — SE(3) Transform Utilities

Build well-tested transform helpers for pose composition, inversion, and frame-aware use.

### Week 5 — Synthetic Hand-Eye Calibration Data

Generate controlled simulation data for camera-to-robot calibration experiments.

### Week 6 — Calibration Solver & Error Analysis

Implement calibration solving, validation datasets, and quantitative error analysis.

### Week 7 — Generic Mecanum Base Simulation

Integrate a reusable omnidirectional base simulation as the mobile platform foundation.

### Week 8 — Phase Demo & Technical Report

Demonstrate the integrated work and document architecture, evidence, and remaining limits.

**Final Goal:** integrated industrial manipulation system — reliable execution, calibrated perception, and a mobile base.
