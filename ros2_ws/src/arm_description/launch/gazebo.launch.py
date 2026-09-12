import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, RegisterEventHandler
from launch.event_handlers import OnProcessExit
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
import xacro

def generate_launch_description():
    pkg_share = get_package_share_directory('arm_description')

    # 1. 解析 xacro 生成完整 URDF 字符串
    xacro_file = os.path.join(pkg_share, 'urdf', 'simple_arm.xacro')
    robot_description_raw = xacro.process_file(xacro_file).toxml()

    # 2. robot_state_publisher 节点
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_description_raw, 'use_sim_time': True}]
    )

    # 3. 加载新版 Gazebo (ros_gz_sim)
    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py')
        ),
        launch_arguments={'gz_args': ' -r -v 4 empty.sdf'}.items()
    )

    # 3.5 新增：/clock 桥接，否则 use_sim_time 的节点收不到仿真时间
    clock_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=['/clock@rosgraph_msgs/msg/Clock[ignition.msgs.Clock'],
        output='screen'
    )

    # 4. 在 Gazebo 中刷入机械臂模型（新版可执行文件叫 create）
    spawn_entity = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=['-topic', 'robot_description', '-name', 'simple_arm'],
        output='screen'
    )

    # 5. 加载并激活 joint_state_broadcaster
    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_state_broadcaster'],
        output='screen'
    )

    # 6. 加载并激活 arm_controller
    arm_controller_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['arm_controller'],
        output='screen'
    )

    # 7. 事件依赖链：Spawn 退出 -> 启动 JSB -> JSB 退出 -> 启动 arm_controller
    spawn_to_jsb_handler = RegisterEventHandler(
        OnProcessExit(
            target_action=spawn_entity,
            on_exit=[joint_state_broadcaster_spawner]
        )
    )

    jsb_to_arm_handler = RegisterEventHandler(
        OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[arm_controller_spawner]
        )
    )

    return LaunchDescription([
        gazebo_launch,
        clock_bridge,
        robot_state_publisher_node,
        spawn_entity,
        spawn_to_jsb_handler,
        jsb_to_arm_handler
    ])