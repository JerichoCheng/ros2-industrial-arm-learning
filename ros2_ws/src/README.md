# ros2_ws/src

从 Week 3 开始，正式的 ROS 2 功能包会陆续放在这里，一个 `colcon` 工作空间管理所有包。

```bash
cd ros2_ws
ros2 pkg create --build-type ament_cmake my_first_pkg
colcon build
source install/setup.bash
```

建议每个功能包对应规划里的一个阶段性目标，比如：
- `week03_hello_node`：最简单的自定义节点
- `week04_pub_sub_service`：发布订阅 + service + launch
- `week05_action_tf2`：自定义 action 接口 + tf2 坐标变换
- `week06_arm_description` / `week06_arm_sim`：URDF 建模 + Gazebo 仿真
- `week07_arm_moveit_config` / `week07_pick_demo`：MoveIt2 配置与运动规划
- `week08_pick_and_place`：Week 8 最终整合的抓取 Demo 节点

不用严格照抄这个命名，跑得通、逻辑清楚比命名规范更重要。
