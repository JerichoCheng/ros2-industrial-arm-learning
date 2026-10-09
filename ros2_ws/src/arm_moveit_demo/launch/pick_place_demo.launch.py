import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def make_pick_place_node(context, *, params_file):
    """Apply non-empty launch arguments after the default YAML parameter file."""
    parameter_overrides = {}
    for parameter_name in ('pick_pose', 'place_pose'):
        if LaunchConfiguration(parameter_name).perform(context).strip():
            # ParameterValue uses ROS launch's YAML conversion. A value such as
            # "[0.28, -0.2, 0.5, 1.0, 0.0, 0.0, 0.0]" therefore becomes double[].
            parameter_overrides[parameter_name] = ParameterValue(
                LaunchConfiguration(parameter_name), value_type=None)

    parameters = [params_file]
    if parameter_overrides:
        parameters.append(parameter_overrides)

    return [Node(
        package='arm_moveit_demo',
        executable='pick_place_demo',
        output='screen',
        parameters=parameters,
    )]

def generate_launch_description():
    # 1. 动态索引 Panda MoveIt 配置包的 share 目录
    panda_config_pkg = get_package_share_directory('moveit_resources_panda_moveit_config')
    panda_demo_launch_path = os.path.join(panda_config_pkg, 'launch', 'demo.launch.py')

    # 2. 引入官方环境全家桶（robot_state_publisher + move_group + RViz2 + 虚拟控制器）
    panda_simulation = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(panda_demo_launch_path)
    )

    # 3. 声明我们自己的抓取序列业务节点，Pick/Place 位姿通过 YAML 参数文件注入
    pick_place_params = os.path.join(
        get_package_share_directory('arm_moveit_demo'),
        'config',
        'pick_place_params.yaml'
    )
    pick_pose_arg = DeclareLaunchArgument(
        'pick_pose',
        default_value='',
        description='Optional [x, y, z, qx, qy, qz, qw] override for the pick pose.',
    )
    place_pose_arg = DeclareLaunchArgument(
        'place_pose',
        default_value='',
        description='Optional [x, y, z, qx, qy, qz, qw] override for the place pose.',
    )
    pick_place_node = OpaqueFunction(
        function=make_pick_place_node,
        kwargs={'params_file': pick_place_params},
    )

    # 4. 用 TimerAction 延迟 6.0 秒拉起节点，留出环境初始化窗口，再由内部 10 秒超时兜底
    delayed_pick_place_node = TimerAction(
        period=6.0,
        actions=[pick_place_node]
    )

    return LaunchDescription([
        pick_pose_arg,
        place_pose_arg,
        panda_simulation,
        delayed_pick_place_node
    ])
