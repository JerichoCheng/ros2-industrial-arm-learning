import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():
    # 1. 动态索引 Panda MoveIt 配置包的 share 目录
    panda_config_pkg = get_package_share_directory('moveit_resources_panda_moveit_config')
    panda_demo_launch_path = os.path.join(panda_config_pkg, 'launch', 'demo.launch.py')

    # 2. 引入官方环境全家桶（robot_state_publisher + move_group + RViz2 + 虚拟控制器）
    panda_simulation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(panda_demo_launch_path)
    )

    # 3. 声明我们自己的抓取序列业务节点
    pick_place_node = Node(
        package='arm_moveit_demo',
        executable='pick_place_demo',
        output='screen'
    )

    # 4. 用 TimerAction 延迟 6.0 秒拉起节点，留出环境初始化窗口，再由内部 10 秒超时兜底
    delayed_pick_place_node = TimerAction(
        period=6.0,
        actions=[pick_place_node]
    )

    return LaunchDescription([
        panda_simulation,
        delayed_pick_place_node
    ])