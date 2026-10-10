# Phase II — Week 1: Execution Reliability & Parameterized Targets

## Objective

Make the simulated pick-and-place execution path configurable and fail-fast, with an explicit controller-readiness gate before trajectories are sent.

## Scope

- Parameterize pick and place poses, including launch-time overrides and seven-element validation.
- Detect plan and execution failures and return a non-zero process exit.
- Stabilize gripper MoveGroup execution with an up-to-date start state.
- Replace the six-second startup timer as the effective controller-readiness gate.

## Completed

- Added pose parameterization, validation, launch overrides, and failure handling.
- Refreshed the gripper current/start state before named-target execution.
- Added typed action-client preflight checks for `panda_arm_controller/follow_joint_trajectory` and `panda_hand_controller/gripper_cmd`.
- Kept the six-second `TimerAction` only as a coarse startup delay; action-server readiness is now checked independently with a ten-second bounded timeout.

## Findings

Manual acceptance reproduced a real startup race: `move_group` became available before `panda_arm_controller/follow_joint_trajectory` was active, with the controller finishing activation roughly 0.7 seconds later. Increasing a fixed timer would not prove readiness. The Week 1 gate waits for the typed controller action endpoints before the first MoveIt execution request and exits non-zero if either endpoint is absent at timeout. It mitigates this startup race directly, but does not prove that a controller cannot deactivate after the preflight check.

## Remaining

- Run default, planning-failure, and execution-failure regressions in a ROS 2 Jazzy + MoveIt 2 environment.
- Consider runtime recovery policy for a controller that becomes unavailable after preflight.

## Evidence

- `b73f804` (cherry-pick of `8007c7a`): pose parameterization, launch overrides, validation, and plan/execute failure exits.
- `a2c51bf` (cherry-pick of `1bbb136`): gripper MoveGroup current-state/start-state stabilization.
- `fdca2e0`: typed controller action-server readiness gating with a bounded timeout before trajectory execution.
- Manual reproduction: approximately 0.7-second gap between `move_group` availability and arm-controller activation; the new preflight wait addresses that gap rather than relying on a longer timer.
